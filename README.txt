����SDLActivity:
1. SDL2��ʾʹ��һ��SurfaceView(SDLActivity::mSurface)����һ����̨�߳�(Thread)SDLActivity.mSDLThread�������ͼ
2. �����̨�߳�SDLActivity.mSDLThread��ͣ�ڻ�ͼ(û���˳�)����һֱ�ܿ�������
3. ����̨�߳̽���ʱ��SDLActivity��finish()��
   ��SDLActivity.mSDLThread.join()ʱ��SDLActivity.mSingleton.finish();���ٵ�Activity
