# imageDeal
## 简介 
  本项目使用基于OPENCV和CVui的智能车图像处理视觉化，使用C++语言和C语言混合编程，旨在方便智能车图像的处理与图像处理算法的开发。  
  图像文件通过自己开发的C#上位机接收:https://github.com/w1oves/upper-computer ，为188*120的灰度图像并保存为bmp文件  
  通过OPENCV读取bmp文件为mat数据，并利用mat数据进行显示和存储  
  在imageDeal和imagePreDeal文件中，对由mat结构数据转换而来的二值化数组进行访问和处理。这些文件全部使用C语言编程，其中的程序可以直接拷贝到智能车的单片机项目中运行 
 
## 安装指南：
1.clone 本项目到本地  
2.用Visual Studio 2019/2017打开  
3.下载opencv340 C++版本  
4.在Visual Studio中配置opencv，可以在搜索引擎中搜索 OpenCV3.4+VisualStudio开发环境配置  
5.下载cvui库https://github.com/Dovyski/cvui 并放到项目文件夹中  
6.将素材图片（120*188的bmp格式图片)放到文件夹D/photo中并以%d.bmp格式命名（也可以在代码中自定义）  
7.运行代码  

