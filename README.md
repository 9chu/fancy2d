fancy2D
===================================

  **警告：该项目已封存并不再进行后续维护**
  
  **警告：考虑到引入了第三方文件导致混乱的版权问题和代码极差的可维护性**

  **警告：该项目极有可能在日后被删除**

  **WARNING: The author is working on a new project.**

  **WARNING: This project is no longer in maintenance.**

  **WARNING: DO NOT use it.**

fancy2D是一款开源的Windows平台上的C++2D游戏封装框架，通过对DirectX进行封装提供了以下功能：

* 基于Direct3D9封装的2D图形绘制API，包括精灵动画、粒子等，并封装了简易的fx接口，为简单3D绘制提供可能；

* 基于DirectSound封装的音频回放支持，支持wav和ogg格式；

* 基于DirectShow封装的视频播放API，允许外挂ax解码器支持任意视频格式；

* 提供自有的frp资源包（含C#打包工具及源码）和fmm网格模型格式（含3DSMax导出插件支持及源码）；

* 提供基于fancy2D封装的fancyUI简易的GUI库。


目前，fancy2D的主要目的为作者个人学习之用，并用于开发一些简单的2D游戏。

作者不推荐将其用于商业项目，且由于时间有限，fancy2D项目暂时缺乏文档支持。

若需要函数帮助，请多阅读代码中给出的注释，或者使用doxygen产生帮助文档。


源代码需要在VS2013下编译。


对于源代码的任何疑问可以在百度贴吧上 @⑨chu


关于fmm网格模型格式3DSMAX导出插件的使用
-----------------------------------
工程中已自带2010和2012版本3dsmax的sdk，请于解决方案中配置生成32或者64位版本的插件。

生成完毕后将插件（如FMMExporter_x64.dle）放入3dsmax的插件目录中（如3ds Max 2012\plugins），

即可在导出选项中导出到fmm模型格式。
