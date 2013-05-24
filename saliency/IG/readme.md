Frequency-tuned Salient Region Detection：一种快速显著物体检测算法
---------------------------------------------------------------

原论文名称：*Frequency-tuned Salient Region Detection*

原作者：Radhakrishna Achantay, Sheila Hemamiz, Francisco Estraday, and Sabine S¨usstrunk。(EPFL)

出处：CVPR09。

今天推荐的算法有关显著物体的检测。在机器视觉应用中，我们经常需要快速的定位场景中的主体区域。在没有任何先验知识的条件下，显著性提供了很好的度量来初始获取视觉的焦点。而利用显著性可以反过来对相机本身进行调节设定参数，或者对画面进行分割获取初始的兴趣区域。

本文所提及的算法在论文中被称为IG算法，其本质为一个DoG带通滤波过程，只是一端被设置为图像的直流分量，其显著图的计算为：

<a href="http://www.codecogs.com/eqnedit.php?latex=S(x, y) = \left \| \textbf{I}_u- \textbf{I}_{w}(x,y) \right \|" target="_blank"><img src="http://latex.codecogs.com/gif.latex?S(x, y) = \left \| \textbf{I}_u- \textbf{I}_{w}(x,y) \right \|" title="S(x, y) = \left \| \textbf{I}_u- \textbf{I}_{w}(x,y) \right \|" /></a>

其中<a href="http://www.codecogs.com/eqnedit.php?latex=\textbf{I}_u" target="_blank"><img src="http://latex.codecogs.com/gif.latex?\textbf{I}_u" title="\textbf{I}_u" /></a>为图像的直流分量，即对应每个颜色通道的均值。<a href="http://www.codecogs.com/eqnedit.php?latex=\textbf{I}_{w}(x,y)" target="_blank"><img src="http://latex.codecogs.com/gif.latex?\textbf{I}_{w}(x,y)" title="\textbf{I}_{w}(x,y)" /></a> 为高斯滤波后的图像。


