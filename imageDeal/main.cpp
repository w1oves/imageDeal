#include<stdio.h>
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv.hpp>
#include"mainInclude.h"
#include"ui.h"
#include<chrono>
#define CVUI_IMPLEMENTATION
#include"cvui-2.7.0/cvui.h"
using cv::Mat;
/*
依赖：
	本项目开源在github：
	使用opencv2 C++版完成图像显示
	使用cvui库完成gui设置：https://github.com/Dovyski/cvui
	项目仅读取bmp灰度文件，大小限制为188*120，文件需按字符串path的形式存储

用户设置参数：
	frame：GUI框架，可设置大小
	number_image：从哪张图片开始处理
	fps_autoplay：启用自动播放时每秒处理多少帧图像
	path：图片文件存放格式
*/
Mat frame = Mat(300, 400, CV_8UC3);
int number_image = 229;
int fps_autoplay = 6;
const char* path = "D:\\photo\\%d.bmp";
int main()
{
	//UI控制变量
	char str[20];
	bool autoplay = false;
	int direction = 0;
	auto CurrentTime = std::chrono::steady_clock::now();
	//使用到的图片
	Mat imgGray, imgGraySample, imgBin_otsu, imgSearch_otsu, imgBin_sauvola, imgSearch_sauvola, imgBin_sobel, imgSearch_sobel, imgHist;
	//初始化
	imgInfoInit();
	cvui::init("UI_block");
	subplot(2, 2);
	//主程序循环
	while (cv::waitKey(30))
	{
		//读取灰度图片并存入数组
		sprintf_s(str, path, number_image);
		imgGray = cv::imread(str, cv::IMREAD_GRAYSCALE);
		mat2array(imgGray);
		//抽样60层
		imageSample();
		imgGraySample = array2mat(&imageGray[0][0], 60, 188);
		//OTSU二值化及搜线 
		process(OTSU);
		imgBin_otsu = array2mat(&imageBin[0][0], 60, 188);
		imageDeal();
		imgSearch_otsu = paintLine(imgBin_otsu);
		//SOBEL算子及搜线
		process(OTSU);
		process(SOBEL);
		imgBin_sobel = array2mat(&imageBin[0][0], 60, 188);
		imageDeal();
		imgSearch_sobel = paintLine(imgBin_sobel);
		//获取灰度直方图
		imgHist = getHistImg(imgGray, threshold_image);
		//显示
		plot(imgGraySample, "Origin", 0);
		plot(imgHist, "Hist", 1);
		plot(imgBin_otsu, "OTSU", 2, true);
		plot(imgSearch_otsu, "Search result of OTSU", 3);
		//plot(imgBin_sobel, "sobel", 4, true);
		//plot(imgSearch_sobel, "search of sobel", 5);
		//*****************************************************************************************************
		//UI控制部分
		UI_refresh(frame, direction, autoplay);
		if (autoplay)
			if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - CurrentTime).count() > 1000 / fps_autoplay)
				CurrentTime = std::chrono::steady_clock::now();
			else
				continue;
		while (direction)
		{
			number_image += direction;
			if (number_image > 1000) number_image = 1;
			if (number_image < 0) number_image = 410;
			sprintf_s(str, path, number_image);
			struct stat buffer;
			if ((stat(str, &buffer) == 0))
				break;
		}

	}
}
