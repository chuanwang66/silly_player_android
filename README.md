silly_player
===

0. A naive media player composed by Sam
---
* based on ffmpeg & SDL2
* mp4 supported
* tested on android 
* built & run in android studio

1. Java端(SDLActivity)
---
* SDLActivity显示使用一个SurfaceView(SDLActivity::mSurface)，用一个后台线程(Thread)SDLActivity.mSDLThread在上面绘图
* SDLActivity.mSDLThread.join()等待后台线程join()才会触发SDLActivity.mSingleton.finish()销毁掉Activity
* 后台线程SDLActivity.mSDLThread一直在绘图(没有退出)，Activity界面就一直可见；直到后台线程退出...


*  public static native void nativeInit();
*  public static native void nativeMp4Filepath(String filepath); //多写一个native方法使mp4路径能从Java端传递到C端


2. C端
---
### jni/SDL/src/main/android/SDL_android_main.c
    void Java_org_libsdl_app_SDLActivity_nativeInit(JNIEnv* env, jclass cls, jobject obj){
        SDL_Android_Init(env, cls);

        SDL_SetMainReady();

        int status;
        char *argv[2];
        argv[0] = SDL_strdup("SDL_app");
        argv[1] = (char *)mp4_filepath;
        status = SDL_main(2, argv);
    }

### jni/src/samSDL2.c
   int main(int argc, char* argv[]){
       //主循环在此，界面不退出
       //(1)自行实现音视频解码播放，视频帧同步到音频帧
       //(2)SDL2的源码(.c & .h)：从SDL2源码搞过来的
       //(3)ffmpeg的二进制(.so & .h)：预先编好的
   }


