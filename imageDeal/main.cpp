#include <iostream>
#include<stdio.h>
#include<string>
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv.hpp>
#include"mainInclude.h"
#define CVUI_IMPLEMENTATION
#include"cvui-2.7.0/cvui.h"
using namespace std;
using namespace cv;
enum type { color, gray, bin };
void mat2array(Mat img);
Mat array2mat(uint8* arr, uint8 h, uint8 w);
void matShow(Mat img, enum type imgType, string str);
Mat merge(Mat imgBin);
uint8 mt9v30x_image[120][188];
int16 differ;
int main()
{
	cvui::init("button");
	carCtrl.flag_associate = 0;
	Mat frame = Mat(300, 600, CV_8UC3);
	int imgNum = 48;
	imgInfoInit();
	int flag_out = 1;
	imgInfo.flag_round_stage = 0;
	while (waitKey(30) != 27)
	{

		char str[20];
		sprintf_s(str, "D:\\photo\\%d.bmp", imgNum);
		Mat imgGray1 = imread(str, IMREAD_GRAYSCALE);
		//matShow(imgGray1, gray, "gray origin");
		mat2array(imgGray1);

		imageSample();
#if 1
		//直方图均衡化
		int ROW = 120;
		int COLUMN = 188;
		int i, j;
		int hist[256] = { 0 };
		for (i = 0; i < ROW; i++)
			for (j = 0; j < ROW; j++)
			{
				hist[imageGray[i][j]]++;
			}
		int trans[256];
		int sum = 0;
		for (i = 0; i < 256; i++)
		{
			sum += hist[i];
			trans[i] = (256.0 - 1.0) / (ROW * COLUMN) * sum;
		}
		for (i = 0; i < ROW; i++)
			for (j = 0; j < COLUMN; j++)
			{
				imageGray[i][j] = trans[imageGray[i][j]];
			}
#endif
		Mat imgGray2 = array2mat(&imageGray[0][0], _ROW, _COLUMN);
		matShow(imgGray2, gray, "gray sample");

		imageBinary();
		Mat imgBin = array2mat(&imageBin[0][0], _ROW, _COLUMN);
		matShow(imgBin, bin, "bin");

		imageDeal();
		Mat	imgSearch = merge(imgBin);
		matShow(imgSearch, color, "search");

		moveWindow("button", 565, 0);
		//moveWindow("gray origin", 0, 0);
		moveWindow("gray sample", 0, 0);
		moveWindow("bin", 565, 361);
		moveWindow("search", 0, 361);

		frame = cv::Scalar(49, 52, 49);
		//*****************************************************************************************************
		cvui::printf(frame, 0, 150, 1, 0xff0000, "imgNum:%d",imgNum);
		cvui::printf(frame, 0, 200, 1, 0xff0000, "differ: %d ", differ);
		cvui::printf(frame, 0, 250, 1, 0xff0000, "bn:%d", imgInfo.black_num);
		cvui::printf(frame, 400, 150, 1, 0xff0000, "flag: %d ", imgInfo.flag_round_stage);
		if (cvui::button(frame, 50, 50, 100, 100, "previous"))
		{
			while (true)
			{
				flag_out = 1;
				imgNum--;
				sprintf_s(str, "D:\\photo\\%d.bmp", imgNum);
				string name = str;
				struct stat buffer;
				if ((stat(name.c_str(), &buffer) == 0))
					break;
				if (imgNum < 0)
					exit(0);
			}
		}
		if (cvui::button(frame, 200, 50, 100, 100, "next"))
		{
			while (true)
			{
				flag_out = 1;
				imgNum++;
				sprintf_s(str, "D:\\photo\\%d.bmp", imgNum);
				string name = str;
				struct stat buffer;
				if ((stat(name.c_str(), &buffer) == 0))
					break;
				if (imgNum > 1000)
					exit(0);
			}
		}
		if (cvui::button(frame, 400, 50, 100, 100, "end"))
		{
			exit(0);
		}
		cvui::update();
		cv::imshow("button", frame);
	}

}
const Vec3b WHITE = Vec3b(255, 255, 255);
const Vec3b BLACK = Vec3b(0, 0, 0);
const Vec3b RED = Vec3b(0, 0, 255);
const Vec3b GREEN = Vec3b(0, 255, 0);
const Vec3b BLUE = Vec3b(255, 0, 0);
const Vec3b YELLOW = Vec3b(0, 255, 255);
const Vec3b CYAN = Vec3b(255, 255, 0);
const Vec3b DARKYELLOW = Vec3b(0, 128, 128);
Mat merge(Mat imgBin)
{
	Mat imgGray;
	imgBin.copyTo(imgGray);
	imgGray = imgGray * 255;
	Mat imgColor;
	cvtColor(imgGray, imgColor, COLOR_GRAY2BGR);
	//imgGray.convertTo(imgColor, CV_GRAY2BGR);
	for (int i = imgInfo.top; i < _ROW; i++)
	{
		imgColor.at<Vec3b>(i, RLB[i]) = RED;
		imgColor.at<Vec3b>(i, LLB[i]) = RED;
		imgColor.at<Vec3b>(i, MLB[i]) = GREEN;
		if (_ROW - i <= imgInfo.len)
			imgColor.at<Vec3b>(i, imgInfo.len_col) = DARKYELLOW;

	}

	for (int j = 0; j < _COLUMN; j++)
	{
		imgColor.at<Vec3b>(imgInfo.bottom, j) = YELLOW;
		imgColor.at<Vec3b>(imgInfo.top, j) = YELLOW;
	}
	if (imgInfo.Lcliffs.downExist)
		for (int i = -2; i <= 2; i++)
		{
			imgColor.at<Vec3b>(imgInfo.Lcliffs.downy, imgInfo.Lcliffs.downx + i) = CYAN;
			imgColor.at<Vec3b>(imgInfo.Lcliffs.downy + i, imgInfo.Lcliffs.downx) = CYAN;
		}
	if (imgInfo.Lcliffs.topExist)
		for (int i = -2; i <= 2; i++)
		{
			imgColor.at<Vec3b>(imgInfo.Lcliffs.topy, imgInfo.Lcliffs.topx + i) = RED;
			imgColor.at<Vec3b>(imgInfo.Lcliffs.topy + i, imgInfo.Lcliffs.topx) = RED;
		}
	if (imgInfo.Rcliffs.downExist)
		for (int i = -2; i <= 2; i++)
		{
			imgColor.at<Vec3b>(imgInfo.Rcliffs.downy, imgInfo.Rcliffs.downx + i) = CYAN;
			imgColor.at<Vec3b>(imgInfo.Rcliffs.downy + i, imgInfo.Rcliffs.downx) = CYAN;
		}
	if (imgInfo.Rcliffs.topExist)
		for (int i = -2; i <= 2; i++)
		{
			imgColor.at<Vec3b>(imgInfo.Rcliffs.topy, imgInfo.Rcliffs.topx + i) = CYAN;
			imgColor.at<Vec3b>(imgInfo.Rcliffs.topy + i, imgInfo.Rcliffs.topx) = CYAN;
		}
	//if (imgInfo.car_exist&&carCtrl.flag_associate==2)
	//{
	//	for (int i = imgInfo.car_bottom; i > imgInfo.car_top; i--)
	//	{
	//		imgColor.at<Vec3b>(i, RLC[i]) = BLUE;
	//		imgColor.at<Vec3b>(i, LLC[i]) = BLUE;
	//		imgColor.at<Vec3b>(i, MLC[i]) = WHITE;
	//	}
	//	for (int j = 0; j < _COLUMN; j++)
	//	{
	//		imgColor.at<Vec3b>(imgInfo.car_bottom, j) = RED;
	//		imgColor.at<Vec3b>(imgInfo.car_top, j) = RED;
	//	}
	//}
	return imgColor;
}
Mat array2mat(uint8* arr, uint8 h, uint8 w)
{
	return Mat(h, w, CV_8UC1, (uint8*)arr);
}
void mat2array(Mat img)
{
	int h = img.size().height;
	int w = img.size().width;
	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
		{
			mt9v30x_image[i][j] = img.at<uint8>(i, j);
		}
	}
}
void matShow(Mat img, enum type imgType, string str)
{
	int h = img.size().height;
	int w = img.size().width;
	namedWindow(str);
	Size dsize = Size(w * 3, h * 6);
	Mat bigImg = Mat(dsize, img.type());
	cv::resize(img, bigImg, dsize, 0, 0, 0);
	switch (imgType)
	{
	case color:
		break;
	case gray:

		break;
	case bin:
		bigImg = bigImg * 255;
		break;
	default:
		break;
	}
	imshow(str, bigImg);
}
int  myabs(int dat)
{
	if (dat >= 0)  return dat;
	else        return -dat;
}