silly_player
===

0. A naive media player composed by Sam
---
* based on ffmpeg & SDL2
* mp4 supported
* tested on android 
* built & run in android studio

1. Java��(SDLActivity)
---
* SDLActivity��ʾʹ��һ��SurfaceView(SDLActivity::mSurface)����һ����̨�߳�(Thread)SDLActivity.mSDLThread�������ͼ
* SDLActivity.mSDLThread.join()�ȴ���̨�߳�join()�Żᴥ��SDLActivity.mSingleton.finish()���ٵ�Activity
* ��̨�߳�SDLActivity.mSDLThreadһֱ�ڻ�ͼ(û���˳�)��Activity�����һֱ�ɼ���ֱ����̨�߳��˳�...


*  public static native void nativeInit();
*  public static native void nativeMp4Filepath(String filepath); //��дһ��native����ʹmp4·���ܴ�Java�˴��ݵ�C��


2. C��
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
       //��ѭ���ڴˣ����治�˳�
       //(1)����ʵ������Ƶ���벥�ţ���Ƶ֡ͬ������Ƶ֡
       //(2)SDL2��Դ��(.c & .h)����SDL2Դ��������
       //(3)ffmpeg�Ķ�����(.so & .h)��Ԥ�ȱ�õ�
   }


