## 轮廓线扫描算法：Theo Pavlidis' Algorithm ##

在介绍Pavlidis算法之前，先介绍个网址:
[http://www.imageprocessingplace.com/downloads_V3/root_downloads/tutorials/contour_tracing_Abeer_George_Ghuneim/index.html](http://www.imageprocessingplace.com/downloads_V3/root_downloads/tutorials/contour_tracing_Abeer_George_Ghuneim/index.html)
这个网站详细介绍了现有常见的几种Contour Tracing算法，英文好的同学直接浏览那边的文章吧。这里引用他们的[图](http://www.imageprocessingplace.com/downloads_V3/root_downloads/tutorials/contour_tracing_Abeer_George_Ghuneim/theo.html)作为解释。

先介绍下Pavlidis的几个关键定义：当前像素位置，当前方向，P1，P2，P3三个像素点:

- 当前像素位置:

	任意一个左边像素为背景的前景边界像素都可以作为起始像素，由算法来决定移动的下个位置。

- 当前方向

	当前像素有4个方向，左，上，右，下。每次移动后方向可能会改变，例如从(i,j)移动到(i,j+1)，那么方向就变成了下。	

- P1，P2，P3像素

	有了当前像素和方向，P1，P2，P3像素的定义如下图所示：

	![Alt text](http://www.imageprocessingplace.com/downloads_V3/root_downloads/tutorials/contour_tracing_Abeer_George_Ghuneim/theopic1.GIF)

	在Figure 1中，当前方向为上，如果当前方向为左，那么P1，P2，P3就会变成左下像素，左边像素以及左上像素。总之从当前方向的视角看出，P1，P2，P3是与上图一致的。补充几张图：

	![Alt text]()![Alt text]()![Alt text]()

有了以上定义，算法的步骤如下:


1. 找到一个初始起点，并设置其方向为上（TOP）
2. 如果P1是前景，那么移动到P2，再移动到P1。

	![Alt text](http://www.imageprocessingplace.com/downloads_V3/root_downloads/tutorials/contour_tracing_Abeer_George_Ghuneim/theopic2.GIF)

3. 如果P1不是前景，那么判断P2是否是前景。如果P2是前景，移动到P2。

	![Alt text](http://www.imageprocessingplace.com/downloads_V3/root_downloads/tutorials/contour_tracing_Abeer_George_Ghuneim/theopic3.GIF)

4. 如果P1，P2都不是前景，那么判断P3是否为前景，如果是，那么先右转，再移动到P3。

	![Alt text](http://www.imageprocessingplace.com/downloads_V3/root_downloads/tutorials/contour_tracing_Abeer_George_Ghuneim/theopic4.GIF)

5. 回到第2步，直到在同一个位置右转3次以上或者返回起始点3次以上结束。


算法标准实现可参考：

[https://github.com/UnilVision/visionbase/tree/master/binary/contour tracing/Pavlidis](https://github.com/UnilVision/visionbase/tree/master/binary/contour_tracing/Pavlidis)

参考结果：

![Alt text]()
