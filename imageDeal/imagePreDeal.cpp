/*
 * @filename:imagePreDeal.c
 * @version: 1.0
 * @Description: 图像预处理,二值化的过程
 * @Author: wloves
 * @Date: 2020 2020年5月3日 下午8:14:38
 * @LastEditors: wloves
 * @LastEditTime: 2020-07-31 16:29:33
 */
#include "imagePreDeal.h"
uint8 SampleRow[ROW] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52, 54, 56, 58, 60, 62, 64, 66, 68, 70, 72, 74, 76, 78, 80, 82, 84, 86, 88 };
uint8 mt9v30x_image[120][188];
uint8 imageGray[ROW][COLUMN];
uint8 imageBin[ROW][COLUMN];
#define GrayScale 256
#define pixelSum (COLUMN * ROW)

uint8 threshold_image;
/**
 * @name: imageBinary
 * @details: 灰度图像二值化函数：中值滤波->大津法求阈值->二值化
 * @param {灰度数组（全局变量）}
 * @return: 二值化数组
 */
#include<chrono>
#include<iostream>

void process(binaryMethod method)
{
	//中值滤波
	auto start = std::chrono::steady_clock::now();
	switch (method)
	{
	case OTSU:
		threshold_image = otsuTh();
		for (int i = 0; i < ROW * COLUMN; i++)
			*(imageBin[0] + i) = *(imageGray[0] + i) > threshold_image;
		break;
	case OTSU2D:
		break;
	case SAUVOLA:
		threshold_image = otsuTh();
		for (int i = 0; i < ROW * COLUMN; i++)
			*(imageBin[0] + i) = *(imageGray[0] + i) > threshold_image;
		sauvola();
		break;
	case SOBEL:
		sobel();
		break;
	case BALANCE:
		break;
	case MEDIANFILTER:
		medianFilter();
		break;
	default:
		break;
	}
	auto end = std::chrono::steady_clock::now();
}
void sobel(void)
{
	uint8 i, j;
	int8 x, y;
	uint8 tmp[ROW][COLUMN];
	const int8 sobelx[3][3] = {
		{-1, 0, 1},
		{-2, 0, 2},
		{-1, 0, 1}
	};
	const int8 sobely[3][3] = {
		{-1, -2, -1},
		{0, 0, 0},
		{1, 2, 1}
	};
	memcpy(tmp, imageBin, ROW * COLUMN);
	for (i = 1; i < ROW - 2; i++)
	{
		for (j = 1; j < COLUMN - 2; j++)
		{
			int8 sumx=0;
			for (x=-1;x<2;x++)
			{
				for (y = -1; y < 2; y++)
				{
					sumx += tmp[i + y][j + x] * sobelx[1+y][1+x];
				}
			}
			int8 sumy = 0;
			for (x = -1; x < 2; x++)
			{
				for (y = -1; y < 2; y++)
				{
					sumy += tmp[i + y][j + x] * sobely[1 + y][1 + x];
				}
			}
			imageBin[i][j] = !(sumx|sumy);
			//imageBin[i][j] = -1 * tmp[i - 1][j - 1] - 2 * tmp[i - 1][j] - 1 * tmp[i - 1][j + 1]
			//	+ 1 * tmp[i + 1][j + 1] + 2 * tmp[i + 1][j] + 1 * tmp[i + 1][j + 1];
			//imageBin[i][j] = !imageBin[i][j];
		}
		
	}
}
//************************************
// 函数名称: sauvola
// 函数说明: 局部均值二值化
// 参    数:
//           const int k                            [in]        threshold = mean*(1 + k*((std / 128) - 1))
//           const int windowSize                   [in]        处理区域宽高
// 返 回 值: void
//************************************'
#define whalf 7
#define k_sauvola 0.18
#define len_IntImg (2*whalf+2)
uint16 backIntegralImg[len_IntImg][COLUMN];
uint32 backIintegralImgSqrt[len_IntImg][COLUMN];
uint8 flag = 1;
void sauvola(void)
{
	int8 i;
	uint8 j;
	// create the integral image
	uint16* integralImg = &backIntegralImg[0][0];
	uint32* integralImgSqrt = &backIintegralImgSqrt[0][0];
	uint16 sum = 0;
	uint32 sqrtsum = 0;
	uint16 index;
	uint8 value;
	uint8* grayPtr = &imageGray[0][0];
	uint8* binPtr = &imageBin[0][0];
	//Calculate the mean and standard deviation using the integral image
	uint8 xmin, ymin, xmax, ymax;
	uint16 area, diagsum, idiagsum, diff;
	uint32 sqdiagsum, sqidiagsum, sqdiff;
	double std;
	uint8 up_IntImg = len_IntImg - 1;
	uint8 base = up_IntImg;
	uint8 ymax_IntImg = 0;
	uint8 ymin_sub_IntImg = 0;
	uint8 next_base;
	double a, b;
	for (i = 0; i < len_IntImg; i++)
	{
		sum = 0;
		sqrtsum = 0;
		for (j = 0; j < COLUMN; j++)
		{
			index = i * COLUMN + j;
			value = grayPtr[index];
			sum += value;
			sqrtsum += value * value;
			if (0 == i)
			{
				integralImg[index] = sum;
				integralImgSqrt[index] = sqrtsum;
			}
			else
			{
				integralImg[index] = integralImg[(i - 1) * COLUMN + j] + sum;
				integralImgSqrt[index] = integralImgSqrt[(i - 1) * COLUMN + j] + sqrtsum;
			}
		}
	}
	for (i = 0; i < ROW; i++)
	{
		ymin = (i > whalf) ? i - whalf : 0;
		ymax = (i < ROW - whalf - 1) ? i + whalf : ROW - 1;
		//收集数据 integralImg像素和积分图 integralImgSqrt像素平方和积分图
		if (ymax == up_IntImg + 1)
		{
			next_base = (base + 1) % len_IntImg;
			sum = 0;
			sqrtsum = 0;
			for (j = 0; j < COLUMN; j++)
			{
				index = ymax * COLUMN + j;
				value = grayPtr[index];
				sum += value;
				sqrtsum += value * value;
				integralImg[next_base * COLUMN + j] = integralImg[base * COLUMN + j] + sum;
				integralImgSqrt[next_base * COLUMN + j] = integralImgSqrt[base * COLUMN + j] + sqrtsum;
			}
			base = next_base;
			up_IntImg++;
		}
		ymax_IntImg = (ymax + len_IntImg - up_IntImg + base) % len_IntImg;
		ymin_sub_IntImg = (ymin - 1 + len_IntImg - up_IntImg + base) % len_IntImg;

		for (j = 0; j < COLUMN; j++)
		{
			if (!binPtr[i * COLUMN + j] || i > ROW - 10 || myabs(j - COLUMN / 2) < 10)
				continue;
			xmin = (j > whalf) ? j - whalf : 0;
			xmax = (j < COLUMN - whalf - 1) ? j + whalf : COLUMN - 1;
			area = (xmax - xmin + 1) * (ymax - ymin + 1);
			if (area <= 0)
			{
				binPtr[i * COLUMN + j] = 255;
				continue;
			}
			if (xmin == 0 && ymin == 0)
			{
				diff = integralImg[ymax_IntImg * COLUMN + xmax];
				sqdiff = integralImgSqrt[ymax_IntImg * COLUMN + xmax];
			}
			else if (xmin > 0 && ymin == 0)
			{
				diff = integralImg[ymax_IntImg * COLUMN + xmax] - integralImg[ymax_IntImg * COLUMN + xmin - 1];
				sqdiff = integralImgSqrt[ymax_IntImg * COLUMN + xmax] - integralImgSqrt[ymax_IntImg * COLUMN + xmin - 1];
			}
			else if (xmin == 0 && ymin > 0)
			{
				diff = integralImg[ymax_IntImg * COLUMN + xmax] - integralImg[ymin_sub_IntImg * COLUMN + xmax];
				sqdiff = integralImgSqrt[ymax_IntImg * COLUMN + xmax] - integralImgSqrt[ymin_sub_IntImg * COLUMN + xmax];
			}
			else
			{
				diagsum = integralImg[ymax_IntImg * COLUMN + xmax] + integralImg[ymin_sub_IntImg * COLUMN + xmin - 1];
				idiagsum = integralImg[ymin_sub_IntImg * COLUMN + xmax] + integralImg[ymax_IntImg * COLUMN + xmin - 1];
				diff = diagsum - idiagsum;
				sqdiagsum = integralImgSqrt[ymax_IntImg * COLUMN + xmax] + integralImgSqrt[ymin_sub_IntImg * COLUMN + xmin - 1];
				sqidiagsum = integralImgSqrt[ymin_sub_IntImg * COLUMN + xmax] + integralImgSqrt[ymax_IntImg * COLUMN + xmin - 1];
				sqdiff = sqdiagsum - sqidiagsum;
			}
			//如果把area前面的double去掉，速率会降低很多
			//mean = diff / area;
#if 0
			std = sqrt((sqdiff - (uint64)diff * (uint64)diff / (double)area) / (double)(area - 1));
			threshold_tmp = ((uint16)((diff * (1 + k_sauvola * ((std / 128) - 1))))) / area;
			binPtr[i * COLUMN + j] = (grayPtr[i * COLUMN + j] > threshold_tmp);
#else
			std = ((double)sqdiff - (double)diff * (double)diff / (double)area) / (double)(area - 1);
			a = ((double)((double)grayPtr[i * COLUMN + j] * (double)area / (double)diff) - 1 + k_sauvola);
			b = (k_sauvola * k_sauvola / 128 / 128) * std;
			binPtr[i * COLUMN + j] = !(a < 0 || a * a < b);
#endif
		}
	}
}

void imageSample(void)
{
	uint8 i, j;
	uint8 line;
	for (i = 0; i < ROW; i++)
	{
		line = SampleRow[i];
		for (j = 3; j < COLUMN; j++)
		{
			imageGray[i][j] = mt9v30x_image[line][j];
		}
	}
}
/***************中值滤波函数*******************/
/**
 * @name: medianFilter
 * @details: 综合上、左、右、当前坐标共四个点求中值滤波,效果比较好,速度未测算
 * @param {灰度数组（全局变量）}
 * @return: 滤波后的黑白数组（全局变量）
 */
void medianFilter(void)
{
	for (int i = ROW - 2; i > 1; i--)
		for (int j = 1; j < COLUMN - 1; j++)
		{
			if (!imageBin[i][j])
			{
				if (imageBin[i][j - 1] && imageBin[i][j + 1])
					imageBin[i][j] = 1;
				//if (imageBin[i-1][j] && imageBin[i+1][j])
				//	imageBin[i][j] = 1;
			}

		}
}


int otsu2dTh(void)
{
	double dHistogram[256][256];        //建立二维灰度直方图  
	double dTrMatrix = 0.0;             //离散矩阵的迹
	int height = ROW;
	int width = COLUMN;
	int N = height * width;               //总像素数
	int i, j;
	for (i = 0; i < 256; i++)
	{
		for (j = 0; j < 256; j++)
			dHistogram[i][j] = 0.0;      //初始化变量  
	}
	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			unsigned char nData1 = (unsigned char)imageGray[i][j];//当前的灰度值  
			unsigned char nData2 = 0;
			int nData3 = 0;//注意9个值相加可能超过一个字节                                                                                    
			for (int m = i - 1; m <= i + 1; m++)
			{
				for (int n = j - 1; n <= j + 1; n++)
				{
					if ((m >= 0) && (m < height) && (n >= 0) && (n < width))
						nData3 += (unsigned char)imageGray[i][j]; //当前的灰度值                                                                        
				}
			}
			nData2 = (unsigned char)(nData3 / 9);    //对于越界的索引值进行补零,邻域均值  
			dHistogram[nData1][nData2]++;
		}
	}
	for (i = 0; i < 256; i++)
		for (j = 0; j < 256; j++)
			dHistogram[i][j] /= N;  //得到归一化的概率分布  

	double Pai = 0.0;      //目标区均值矢量i分量  
	double Paj = 0.0;      //目标区均值矢量j分量  
	double Pbi = 0.0;      //背景区均值矢量i分量  
	double Pbj = 0.0;      //背景区均值矢量j分量  
	double Pti = 0.0;      //全局均值矢量i分量  
	double Ptj = 0.0;      //全局均值矢量j分量  
	double W0 = 0.0;       //目标区的联合概率密度  
	double W1 = 0.0;       //背景区的联合概率密度  
	double dData1 = 0.0;
	double dData2 = 0.0;
	double dData3 = 0.0;
	double dData4 = 0.0;   //中间变量  
	int nThreshold_s = 0;
	int nThreshold_t = 0;
	int nThreshold;
	double temp = 0.0;     //寻求最大值  
	for (i = 0; i < 256; i++)
	{
		for (j = 0; j < 256; j++)
		{
			Pti += i * dHistogram[i][j];
			Ptj += j * dHistogram[i][j];
		}
	}
	for (i = 0; i < 256; i++)
	{
		for (j = 0; j < 256; j++)
		{
			W0 += dHistogram[i][j];
			dData1 += i * dHistogram[i][j];
			dData2 += j * dHistogram[i][j];

			W1 = 1 - W0;
			dData3 = Pti - dData1;
			dData4 = Ptj - dData2;

			Pai = dData1 / W0;
			Paj = dData2 / W0;
			Pbi = dData3 / W1;
			Pbj = dData4 / W1;   // 得到两个均值向量，用4个分量表示  
			dTrMatrix = ((W0 * Pti - dData1) * (W0 * Pti - dData1) + (W0 * Ptj - dData2) * (W0 * Ptj - dData2)) / (W0 * W1);
			if (dTrMatrix > temp)
			{
				temp = dTrMatrix;
				nThreshold_s = i;
				nThreshold_t = j;
			}
		}
	}
	nThreshold = (nThreshold_t + nThreshold_s) / 2;//返回阈值，有多种形式，可以单独某一个，也可                                                         //是两者的均值 
	return nThreshold;
}
uint8 otsuTh(void)
{
	uint16 histogram[GrayScale] = { 0 };
	uint32 iMulHistogram[GrayScale] = { 0 };
	uint8 i;
	uint8 threshold = 0;
	uint32 sum = 0;
	//统计灰度级中每个像素在整幅图像中的个数
	unsigned char* ptr = &imageGray[0][0];
	for (uint16 i = 0; i < ROW * COLUMN; i++)
		histogram[ptr[i]]++; //将像素值作为计数数组的下标
	//获取总的像素值
	for (i = 0; i < GrayScale - 1; i++)
	{
		iMulHistogram[i] = i * histogram[i];
		sum += iMulHistogram[i];
	}
	uint8 MinValue, MaxValue;
	for (MinValue = 0; MinValue < GrayScale - 1 && histogram[MinValue] == 0; MinValue++)
		; //获取最小灰度的值
	for (MaxValue = GrayScale - 1; MaxValue > MinValue && histogram[MaxValue] == 0; MaxValue--)
		; //获取最大灰度的值
	//遍历灰度级[0,255]
	if (MaxValue == MinValue)
		return (uint8)MaxValue; // 图像中只有一个颜色
	if (MinValue + 1 == MaxValue)
		return (uint8)MinValue; // 图像中只有二个颜色
	//back：背景像素点；front：前景像素点
	//w：比例；u：平均值；
	uint16 w_back = 0, w_front = 0;
	uint32 u_back_sum = 0, u_front_sum = 0;
	float u_back = 0, u_front = 0;
	float deltaTmp = 0, deltaMax = 0;
	for (i = MinValue; i < MaxValue; i++) // i作为阈值
	{
		//更新前景、后景像素点比例
		w_back += histogram[i];
		w_front = pixelSum - w_back;
		//更新前景、后景像素总灰度值
		u_back_sum += iMulHistogram[i];
		u_front_sum = sum - u_back_sum;
		//更新前景、后景像素平均灰度值
		u_back = u_back_sum / (float)w_back;
		u_front = u_front_sum / (float)w_front;
		//计算类间方差
		deltaTmp = w_back * w_front * (u_back - u_front) * (u_back - u_front);
		//求最大类间方差
		if (deltaTmp > deltaMax)
		{
			deltaMax = deltaTmp;
			threshold = (uint8)i;
		}
	}
	return threshold;
}