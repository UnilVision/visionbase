Frequency-tuned Salient Region Detection：一种快速显著物体检测算法
---------------------------------------------------------------

原论文名称：*Frequency-tuned Salient Region Detection*

原作者：Radhakrishna Achantay, Sheila Hemamiz, Francisco Estraday, and Sabine S¨usstrunk。(EPFL)

出处：CVPR09。

今天推荐的算法有关显著物体的检测。在机器视觉应用中，我们经常需要快速的定位场景中的主体区域。在没有任何先验知识的条件下，显著性提供了很好的度量来初始获取视觉的焦点。而利用显著性可以反过来对相机本身进行调节设定参数，或者对画面进行分割获取初始的兴趣区域。

本文所提及的算法在论文中被称为IG算法，其本质为一个DoG带通滤波过程，只是一端被设置为图像的直流分量，其显著图的计算为：

<a href="http://www.codecogs.com/eqnedit.php?latex=S(x, y) = \left \| \textbf{I}_u- \textbf{I}_{w}(x,y) \right \|" target="_blank"><img src="http://latex.codecogs.com/gif.latex?S(x, y) = \left \| \textbf{I}_u- \textbf{I}_{w}(x,y) \right \|" title="S(x, y) = \left \| \textbf{I}_u- \textbf{I}_{w}(x,y) \right \|" /></a>

其中<a href="http://www.codecogs.com/eqnedit.php?latex=\textbf{I}_u" target="_blank"><img src="http://latex.codecogs.com/gif.latex?\textbf{I}_u" title="\textbf{I}_u" /></a>为图像的直流分量，即对应每个颜色通道的均值。<a href="http://www.codecogs.com/eqnedit.php?latex=\textbf{I}_{w}(x,y)" target="_blank"><img src="http://latex.codecogs.com/gif.latex?\textbf{I}_{w}(x,y)" title="\textbf{I}_{w}(x,y)" /></a> 为高斯滤波后的图像。

IG算法的纯c的实现：
[https://github.com/UnilVision/visionbase/tree/master/saliency/IG](https://github.com/UnilVision/visionbase/tree/master/saliency/IG)

结果1：
![结果1](https://raw.github.com/UnilVision/visionbase/master/saliency/IG/r1.jpg)

结果2：
![结果1](https://raw.github.com/UnilVision/visionbase/master/saliency/IG/r2.jpg)

关于显著性检测这个命题再说几句废话吧。本文说介绍的算法和Ming-Ming Cheng的Global Contrast based Salient Region Detection等方法均属于无监督算法，通过视觉特征快速的确定画面内较为明显的区域。而另一个算法分支应用了机器学习，通过学习样本来预测新图像中的显著区域。

从单个类别，小样本的数据集上看，后者确实是有优势的。但是问题是在大多数情况下，我们需要的显著性检测是需要非常快速的，例如在数码相机之中自动确定显著区域并进行定焦，在图像识别算法前作为独立特征进行提取，快速确认焦点区域生成缩略图等等。而后者的思路更像Object Detection+Segmentation，也注定了其效率很难超越前者。而在实际的项目中，可以考虑的原则就是如果是针对某个特定物体的，那么后者也许会带来很好的效果，否则就使用第一类算法。

