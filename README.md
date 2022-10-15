# imageDeal
## 简介 
* 本项目使用基于OPENCV和CVui的智能车图像处理视觉化，使用C++语言和C语言混合编程，旨在方便智能车图像的处理与图像处理算法的开发。  
* 图像文件通过自己开发的C#上位机接收:https://github.com/w1oves/upper-computer ，为188*120的灰度图像并保存为bmp文件
* 依赖opencv
* 在imageDeal和imagePreDeal文件中，对二值化数组进行访问和处理。这些文件全部使用C语言编程，其中的程序可以直接拷贝到智能车的单片机项目中运行 
 
## 安装指南
1. clone 本项目到本地  
2. 用Visual Studio 2017以上版本打开（推荐为2019）
3. 下载opencv340 C++版本  
4. 在Visual Studio中配置opencv，可以在搜索引擎中搜索 OpenCV3.4+VisualStudio开发环境配置  
5. 将素材图片（120*188的bmp格式图片)放到文件夹D/photo中并以%d.bmp格式命名（也可以在代码中进行自定义），%d指正整数
6. 运行代码 

### 安装opencv的详细指引
详细参考 https://www.cnblogs.com/shlll/archive/2018/02/06/8424692.html
1. 在opencv官网 https://opencv.org/releases/ 下载opencv3.4.0
2. 下载后安装到任意文件夹，并记下安装路径
3. 在我的电脑->属性->高级系统设置->环境变量中，新建一个系统变量`OPENCV_DIR`，值为opencv路径中build\x64\vc15文件夹。
4. 用visual studio打开imageDeal.sln（如果已经打开，此时必须重启！）
5. 将C/C++->常规->附加包含目录修改为`$(OPENCV_DIR)\..\..\include`，以包含头文件
6. 将链接器->输入->附加依赖项修改为`opencv_world340d.lib`
7. 将链接器->常规->附加库目录修改为`$(OPENCV_DIR)\lib`
8. 运行项目

## 视频
相关演示视频在https://www.bilibili.com/video/BV1Ay4y1m7KC?from=search&seid=5597632271366628433

