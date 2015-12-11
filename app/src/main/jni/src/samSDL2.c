#ifdef  __ANDROID__
#ifdef __cplusplus
extern "C"
{
#endif

//android headers
#include <jni.h>

//general headers
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <math.h>

//SDL2 headers
#include <SDL.h>

//ffmpeg headers
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

//media player headers
#include "android_log.h"
#include "audio.h"
#include "video.h"
#include "parse.h"
#include "player.h"

#ifdef __cplusplus
};
#endif

static VideoState *is; //global video state

static int stream_component_open(int stream_index)
{
    AVCodec *codec = NULL;
    AVCodecContext *codecCtx = NULL;
    SDL_AudioSpec desired_spec, spec;

    if(stream_index < 0 || stream_index >= is->pFormatCtx->nb_streams)
    {
        LOGD("stream index invalid: stream_index=%d, stream #=%d.\n", stream_index, is->pFormatCtx->nb_streams);
        return -1;
    }

    codec = avcodec_find_decoder(is->pFormatCtx->streams[stream_index]->codec->codec_id);
    if(!codec)
    {
        LOGD("unsupported codec.\n");
        return -1;
    }

    codecCtx = avcodec_alloc_context3(codec);
    if(avcodec_copy_context(codecCtx, is->pFormatCtx->streams[stream_index]->codec) != 0)
    {
        LOGD("could not build codecCtx");
        return -1;
    }

    if(codecCtx->codec_type == AVMEDIA_TYPE_AUDIO)
    {
        desired_spec.freq = codecCtx->sample_rate;
        desired_spec.format = AUDIO_S16SYS;
        desired_spec.channels = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);//codecCtx->channels;
        desired_spec.silence = 0;
        desired_spec.samples = codecCtx->frame_size;//SDL_AUDIO_BUFFER_SIZE;

        desired_spec.callback = audio_callback;
        desired_spec.userdata = is;

        if(SDL_OpenAudio(&desired_spec, &spec) < 0)
        {
            LOGD("SDL_OpenAudio(): %s.\n", SDL_GetError());
            return -1;
        }
        is->audio_hw_buf_size = spec.size;

        LOGD("spec.samples=%d (size of the audio buffer in samples)\n", spec.samples);
        LOGD("spec.freq=%d (samples per second)\n", spec.freq);
        LOGD("spec.channels=%d\n", spec.channels);
        LOGD("spec.format=%d (size & type of each sample)\n", spec.format);
    }

    //open decoder
    if(avcodec_open2(codecCtx, codec, NULL) < 0)
    {
        LOGD("avcodec_open2() error.\n");
        return -1;
    }

    switch(codecCtx->codec_type)
    {
    case AVMEDIA_TYPE_AUDIO:
        is->audio_stream_index = stream_index;
        is->audio_st = is->pFormatCtx->streams[stream_index];
        is->audio_ctx = codecCtx;

        //memset(&is->audio_pkt, 0, sizeof(is->audio_pkt));
        is->audio_pkt_ptr = (AVPacket *)av_malloc(sizeof(AVPacket));
        av_init_packet(is->audio_pkt_ptr);

        is->audio_buf_size = 0;
        is->audio_buf_index = 0;

        //prepare conversion facility (FLTP -> S16)
        is->out_buffer = (uint8_t *)av_malloc(MAX_AUDIO_FRAME_SIZE << 1); //why twice ???
        is->swr_ctx = swr_alloc();
        is->swr_ctx = swr_alloc_set_opts(is->swr_ctx,
                                         AV_CH_LAYOUT_STEREO,
                                         AV_SAMPLE_FMT_S16,
                                         is->audio_ctx->sample_rate, //44100
                                         av_get_default_channel_layout(is->audio_ctx->channels), //is->audio_ctx->channel_layout
                                         is->audio_ctx->sample_fmt,
                                         is->audio_ctx->sample_rate,
                                         0,
                                         NULL);
        swr_init(is->swr_ctx);
        break;
    case AVMEDIA_TYPE_VIDEO:
        is->video_stream_index = stream_index;
        is->video_st = is->pFormatCtx->streams[stream_index];
        is->video_ctx = codecCtx;
        is->frame_timer = (double)av_gettime() / 1000000.0;
        is->frame_last_delay = 40e-3; //40ms

        is->sws_ctx = sws_getContext(is->video_ctx->width, is->video_ctx->height,
                                     is->video_ctx->pix_fmt,
                                     is->video_ctx->width, is->video_ctx->height,
                                     PIX_FMT_YUV420P,SWS_BICUBIC,
                                     NULL, NULL, NULL);

        break;
    }

    LOGD("stream[%d] opened.\n", stream_index);
    return 0;
}

static int openDecoder()
{
    int video_stream_index = -1;
    int audio_stream_index = -1;
    int i;

    is->video_stream_index = -1;
    is->audio_stream_index = -1;

    if(avformat_open_input(&is->pFormatCtx, is->filename, NULL, NULL) != 0)
    {
        LOGD("could not open video file[%s].\n", is->filename);
        return -1;
    }
    if(avformat_find_stream_info(is->pFormatCtx, NULL) < 0)
    {
        LOGD("could not find stream info.\n");
        return -1;
    }
    av_dump_format(is->pFormatCtx, 0, is->filename, 0);

    for(i=0; i<is->pFormatCtx->nb_streams; ++i)
    {
        int media_type = is->pFormatCtx->streams[i]->codec->codec_type;

        if(media_type == AVMEDIA_TYPE_VIDEO && video_stream_index == -1)
        {
            video_stream_index = i;
        }
        if(media_type == AVMEDIA_TYPE_AUDIO && audio_stream_index == -1)
        {
            audio_stream_index = i;
        }
    }

    if(video_stream_index >= 0)
    {
        if(stream_component_open(video_stream_index) < 0)
        {
            LOGD("%s: could not open video codecs.\n", is->filename);
            return -1;
        }

    }
    else
    {
        LOGD("%s: could not find video stream.\n", is->filename);
        return -1;
    }

    if(audio_stream_index >= 0)
    {
        if(stream_component_open(audio_stream_index) < 0)
        {
            LOGD("%s: could not open audio codecs.\n", is->filename);
            return -1;
        }
    }
    else
    {
        LOGD("%s: could not find audio stream.\n", is->filename);
        return -1;
    }

    return 0;
}

/** **************************** video displaying(main thread) **************************** **/
static Uint32 video_refresh_timer_cb(Uint32 delay, void *opaque){
    SDL_Event event;
    event.type = FF_REFRESH_EVENT;
    event.user.data1 = opaque;
    SDL_PushEvent(&event);
    return 0;
}

static void video_refresh_timer(void *userdata){
    VideoState *is = (VideoState *)userdata;
    VideoPicture *vp;
    double actural_delay, delay, diff;

    //decoder not opened -> check later
    if(!is->video_st){
        SDL_AddTimer(100, video_refresh_timer_cb, is);
        return;
    }
    assert(is->video_st != NULL);

    //YUV image not ready -> check later
    if(is->pictq_size == 0){
        SDL_AddTimer(1, video_refresh_timer_cb, is);
        return;
    }
    assert(is->pictq_size > 0);

    //
    vp = &is->pictq;

#if 1
    //maintain delay & pts
    delay = vp->pts - is->frame_last_pts;
    if(delay <= 0 || delay >= 1.0){ //unit: second
        delay = is->frame_last_delay;
    }
    delay = fmax(delay, AV_SYNC_THRESHOLD);
    is->frame_last_delay = delay;
    is->frame_last_pts = vp->pts;

    //(update delay to sync to audio)
    diff = vp->pts - get_audio_clock(is);
    if(fabs(diff) <= AV_NOSYNC_THRESHOLD){ //if it's possible to sync
        if(diff <= -delay){
            delay = 0; //speed video up
        }else{
            delay = 2 * delay; //slow video down
        }
    }
    is->frame_timer += delay;

    actural_delay = is->frame_timer - (av_gettime() / 1000000.0);
    actural_delay = fmax(actural_delay, 0.010);
#else
    actural_delay = 0.04;
#endif

    //schedule the next refresh
    SDL_AddTimer((int)(actural_delay * 1000 + 0.5), video_refresh_timer_cb, is);

    //FINALLY, a YUV image is waiting for us to display!
    video_display(is);

    //hunger for more, please decoding!
    SDL_LockMutex(is->pictq_mutex);
    --is->pictq_size;
    SDL_CondSignal(is->pictq_cond);
    SDL_UnlockMutex(is->pictq_mutex);
}

int main(int argc, char* argv[])
{
    SDL_Event sdlEvent;
    SDL_Thread *parse_tid;
    uint64_t refresh_cnt;
    int i;

    is = av_mallocz(sizeof(VideoState)); //memory allocation with alignment, why???
    strncpy(is->filename, argv[1], sizeof(is->filename));
    is->pictq_mutex = SDL_CreateMutex();
    is->pictq_cond = SDL_CreateCond();

    //register all formats & codecs
    av_register_all();

    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER))
    {
        LOGD("SDL_Init() error: %s\n", SDL_GetError());
        exit(1);
    }

    if(openDecoder() != 0)
    {
        av_free(is);
        return -1;
    }

    //parsing thread (reading packets from stream)
    parse_tid = SDL_CreateThread(parse_thread, "PARSING_THREAD", is);
    if(!parse_tid)
    {
        LOGD("cannot create parsing thread.\n");
        av_free(is);
        return -1;
    }

    //audio-decoding thread (audio pkt --> frame --> audio buffer)
    SDL_PauseAudio(0);

    //video-decoding thread (video pkt --> frame --> YUV image)
    SDL_CreateThread(video_thread, "VIDEO_DECODING_THREAD", is);

    //////////////////////////////// the window ////////////////////////////////
    //once AVCodecContext is known, the size of window is known
    video_init(is);
    SDL_AddTimer(40, video_refresh_timer_cb, is);

    refresh_cnt = 0;
    for(;;){
        SDL_WaitEvent(&sdlEvent);
        switch(sdlEvent.type){
        case FF_REFRESH_EVENT:
            //LOGD("[fre%ld]", ++refresh_cnt);
            video_refresh_timer(sdlEvent.user.data1);
            break;
        case SDL_QUIT:
            LOGD("event:quit\n");
            is->quit = 1;
            break;
        default:
            //LOGD("event:%d\n", sdlEvent.type);
            break;
        }

        if(is->quit) break;
    }

    SDL_Quit();
    return 0;
}

#endif