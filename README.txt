解析SDLActivity:
1. SDL2显示使用一个SurfaceView(SDLActivity::mSurface)，用一个后台线程(Thread)SDLActivity.mSDLThread在上面绘图
2. 如果后台线程SDLActivity.mSDLThread不停在绘图(没有退出)，就一直能看到界面
3. 当后台线程结束时，SDLActivity被finish()掉
   即SDLActivity.mSDLThread.join()时，SDLActivity.mSingleton.finish();销毁掉Activity
