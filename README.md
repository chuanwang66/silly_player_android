# silly_player

A naive media player composed by Sam
===
* based on ffmpeg & SDL2
* mp4 supported
* tested on android 
* built & run in android studio

===
��7SDLActivity:
* SDL2��ʾʹ��һ��SurfaceView(SDLActivity::mSurface)����һ����̨�߳�(Thread)SDLActivity.mSDLThread�������ͼ
* �����̨�߳�SDLActivity.mSDLThread��ͣ�ڻ�ͼ(û���˳�)����һֱ�ܿ�������
* ����̨�߳̽���ʱ��SDLActivity��finish()��
   ��SDLActivity.mSDLThread.join()ʱ��SDLActivity.mSingleton.finish();���ٵ�Activity

