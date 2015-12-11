# silly_player

A naive media player composed by Sam
===
* based on ffmpeg & SDL2
* mp4 supported
* tested on android 
* built & run in android studio

===
解7SDLActivity:
* SDL2显示使用一个SurfaceView(SDLActivity::mSurface)，用一个后台线程(Thread)SDLActivity.mSDLThread在上面绘图
* 如果后台线程SDLActivity.mSDLThread不停在绘图(没有退出)，就一直能看到界面
* 当后台线程结束时，SDLActivity被finish()掉
   即SDLActivity.mSDLThread.join()时，SDLActivity.mSingleton.finish();销毁掉Activity

