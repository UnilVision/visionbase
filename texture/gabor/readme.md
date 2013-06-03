标准的Gabor滤波器的C语言实现
------------------------------------------

在图像识别，分类，分割等应用中，纹理都是重要的特征之一。在纹理分析中，基于Gabor滤波的纹理特征是极为常见的。本文不深究Gabor的原理，下面仅给出Gabor的基本滤波表达式，而注重在空间域上正确实现上的一些细节。

Gabor滤波器的实部：

<a href="http://www.codecogs.com/eqnedit.php?latex=R=\exp( -\frac{x'^2@plus;\gamma y'^2}{2\sigma^2})\cos(2\pi \frac{x'}{\lambda}@plus;\psi )" target="_blank"><img src="http://latex.codecogs.com/gif.latex?R=\exp( -\frac{x'^2+\gamma y'^2}{2\sigma^2})\cos(2\pi \frac{x'}{\lambda}+\psi )" title="R=\exp( -\frac{x'^2+\gamma y'^2}{2\sigma^2})\cos(2\pi \frac{x'}{\lambda}+\psi )" /></a>

Gabor滤波器虚部：

<a href="http://www.codecogs.com/eqnedit.php?latex=R=\exp( -\frac{x'^2@plus;\gamma y'^2}{2\sigma^2})\cos(2\pi \frac{x'}{\lambda}@plus;\psi )" target="_blank"><img src="http://latex.codecogs.com/gif.latex?R=\exp( -\frac{x'^2+\gamma y'^2}{2\sigma^2})\sin(2\pi \frac{x'}{\lambda}+\psi )" title="R=\exp( -\frac{x'^2+\gamma y'^2}{2\sigma^2})\sin(2\pi \frac{x'}{\lambda}+\psi )" /></a>

在空间域上进行滤波时，需要同时计算实部与虚部的值才能获得幅值信息。另外在滤波过程中需要正确的计算模板的大小，模板太小出来的结果是不正确的。

这里最重要的两个参数是<a href="http://www.codecogs.com/eqnedit.php?latex=\lambda" target="_blank"><img src="http://latex.codecogs.com/gif.latex?\lambda" title="\lambda" /></a>
以及<a href="http://www.codecogs.com/eqnedit.php?latex=\sigma" target="_blank"><img src="http://latex.codecogs.com/gif.latex?\sigma" title="\sigma" /></a>分别是波长以及卷积时的高斯标准差，用于控制模糊长度。

模板大小的估算方法为：

<a href="http://www.codecogs.com/eqnedit.php?latex=S = 1.0f / (1.414f * \sigma ); B = 1.5f / S" target="_blank"><img src="http://latex.codecogs.com/gif.latex?S = 1.0f / (1.414f * \sigma ); B = 1.5f / S" title="S = 1.0f / (1.414f * \sigma ); B = 1.5f / S" /></a>

而最终的模板大小可以取为2*B+1。


本文实现的Gabor滤波器接口如下:

	void gabor_filter(int gabor_theta_div,
				  float gabor_lambda, float gabor_sigma, float gabor_phase_offset,
				  unsigned char* src, int src_wid, int src_hei, int src_widstep,
				  unsigned char* dst, int dst_wid, int dst_hei, int dst_widstep,
				  int dst_offsetx, int dst_offsety);

其中

	gabor_theta_div 为gabor角度数目。
	gabor_lambda 波长
	gabor_sigma 高斯标准差
	gabor_phase_offset 相位偏移量

上述代码完成的c实现可以在以下地址获得：



两个尺度，8方向结果示例：

	src_img = cvLoadImage("test.png", 0);
	show_img = cvCreateImage(cvSize(src_img->width*8, src_img->height*2), IPL_DEPTH_8U, 1);
	gabor_filter(8, 3, 3, 0, src_img->imageData, src_img->width, src_img->height, src_img->widthStep, 
	    show_img->imageData, show_img->width, show_img->height, show_img->widthStep, 0, 0);
	gabor_filter(8, 3, 7, 0, src_img->imageData, src_img->width, src_img->height, src_img->widthStep,
	    show_img->imageData, show_img->width, show_img->height, show_img->widthStep, 0, src_img->height);


![](https://raw.github.com/UnilVision/visionbase/master/texture/gabor/gabor_vs/test.png)
![](https://raw.github.com/UnilVision/visionbase/master/texture/gabor/gabor_vs/out.jpg)


最后，关于Gabor的实际使用，上述实现是非常慢的，如果保持思路不变，即使经过高强度的代码优化依然不可能达到满意的效率。

最实际应用中，使用频域滤波实现Gabor效率更高。在空间域，另一个优化的方法是在水平和45度，90度的方向上可采用类似高斯核的分解方法，将二维模板分解为一维模板做两次滤波实现。而其余方向的Gabor则通过旋转定位到上述几个角度来实现。

