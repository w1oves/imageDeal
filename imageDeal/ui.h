#pragma once
#include <iostream>
#include<stdio.h>
#include<string>
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv.hpp>
#include"cvui-2.7.0/cvui.h"
#include"mainInclude.h"
#define GRID_W (188*3)
#define GRID_H (120*3)
using cv::Mat;
void mat2array(Mat img);
Mat array2mat(uint8* arr, uint8 h, uint8 w);
Mat paintLine(Mat imgBin);
void subplot(int y, int x);
enum class showtype
{
	not_change,keepRatio,whole_fill
};
void plot(Mat image, std::string name, int number, showtype type, bool isBin);
void plot(Mat image, std::string name, int number);
void plot(Mat image, std::string name, int number,bool isBin);
Mat getHistImg(Mat img, int th);
Mat getTextImg(Mat img);

void UI_refresh(Mat& frame, int& direction, bool& autoplay);
