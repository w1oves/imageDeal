/*
 * @filename:imageDeal.c
 * @version: 1.0
 * @Description: 图像处理、搜线、巡线，四轮
 * @Author: wloves
 * @Date: 2020 2020年5月3日 下午8:14:50
 * @LastEditors: wloves
 * @LastEditTime: 2020-10-07 20:15:06
 */
#include "imageDeal.h"

#define DEFAULT_MID (IMAGE_W / 2)
#define DEFAULT_LEFT 1
#define DEFAULT_RIGHT COLUMN - 2
 /**
  * @name: 丢线判断宏定义
  * @details: 通过简单的等于与不等于判断
  * @param {uint8 行号}
  * @return: 是否丢线
  */
#define isLoseL(i) (LL[i] == DEFAULT_LEFT)
#define isLoseR(i) (RL[i] == DEFAULT_RIGHT)
#define notLoseL(i) (LL[i] != DEFAULT_LEFT)
#define notLoseR(i) (RL[i] != DEFAULT_RIGHT)
  /**
   * @name: limitRl
   * @details: 边线限幅内联函数，保证补线时边线不越界
   * @param {uint8 边线列号}
   * @return: 限幅后的列号
   */
uint8 RL[ROW], ML[ROW], LL[ROW];
uint8 RLB[ROW], MLB[ROW], LLB[ROW];
uint8 whitePoint[ROW];
uint8 width[ROW];
struct imageInformation imgInfo;
uint8 half_width[ROW] = { 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 19, 20, 21, 22, 23, 24, 25, 26, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 42, 44, 46, 48, 50, 52, 54, 56, 58, 61, 63, 65, 67, 69, 71, 73, 75, 77, 79, 81, 83, 85, 87, 89, 90, 91, 91, 91, 91 };
/**
 * @name: imageDeal
 * @details: 图像处理函数；当前版本仅适用于直道、小弯道、环岛
 * @param {二值化数组}
 * @return: differ
 */
void imageDeal(void)
{
	//搜线
	uint8 i;
	imgInfoInit();
	getLen();
	for (i = 0; i < ROW; i++)
	{
		imageBin[i][2] = 1;
		imageBin[i][1] = 0;
		imageBin[i][0] = 0;
		imageBin[i][COLUMN - 3] = 1;
		imageBin[i][COLUMN - 2] = 0;
		imageBin[i][COLUMN - 1] = 0;
	}
	searchLine();
	memcpy(RLB, RL, ROW);
	memcpy(LLB, LL, ROW);
	memcpy(MLB, ML, ROW);
	imgInfo.lastMid = (ML[ROW - 4] + ML[ROW - 3] + ML[ROW - 2] + ML[ROW - 1]) >> 2;
	//弯道处理
	imgInfo.k = leastSquares();
	turnDeal();
	getDiffer();
}

/**
 * @name: getLen
 * @details: 判断最长直道长度
 * @param {void}
 * @return: 最长直道长度及对应列号存入imgInfo数组
 */
void getLen(void)
{
	int8 len;
	uint8 j;
	imgInfo.len = 0;
	for (j = 4; j < COLUMN; j += 10)
	{
		for (len = ROW - 2; len >= 0 && imageBin[len][j]; len -= 2)
			;
		len = ROW - len;
		if (imgInfo.len < len)
		{
			imgInfo.len = len;
		}
	}

	/**
 * @name: imgInfoInit
 * @details: 初始化imgInfo结构体
 * @param {type}
 * @return:
 */
}
void imgInfoInit(void)
{
	imgInfo.bottom = ROW - 1;
	imgInfo.lastMid = DEFAULT_MID;
	imgInfo.top = 0;
}
/**
 * @name: getDiffer
 * @details: 获取当前偏差值
 * @param {void}
 * @return: differ
 */
void getDiffer(void)
{
	//修改以下部分，并配合合适的pid参数，以达到最好效果
	int8 i;
	int16 sum = 0;
	int8 lineNum = 1;
	for (i = imgInfo.top; i < imgInfo.bottom; i++)
	{
		MLB[i] = (LLB[i] + RLB[i]) >> 1;
	}
	//权重设置：近处权重大，远处权重小
	uint8 mid = (imgInfo.bottom + imgInfo.top) / 2;
	lineNum = 0;
	for (i = imgInfo.top; i < mid; i++)
	{
		sum += MLB[i];
		lineNum++;
	}
	for (; i < imgInfo.bottom; i++)
	{
		sum += MLB[i] * 3;
		lineNum += 3;
	}
	sum /= lineNum;
	sum -= DEFAULT_MID;
	imgInfo.differ = sum;
}
/**
 * @name: crossDeal
 * @details: 十字处理函数，待完善
 * @param {type}
 * @return:
 */
void turnDeal(void)
{
	//根据最小二乘法拟合得到的斜率判断是否为弯道
	int16 i;
	imgInfo.roadType = ROAD_straight;
	if (imgInfo.len < 45)
		imgInfo.roadType = ROAD_trun;
	else if (imgInfo.len < 35)
		imgInfo.roadType = ROAD_big_turn;
	if (imgInfo.k > 1)
		imgInfo.roadType = ROAD_turn_left;
	else if (imgInfo.k < -1)
		imgInfo.roadType = ROAD_turn_right;
	if (imgInfo.k > 1.9)
		imgInfo.roadType = ROAD_big_turn_left;
	else if (imgInfo.k < -1.9)
		imgInfo.roadType = ROAD_big_turn_right;
}

/***************搜线函数*******************/
/**
 * @name: searchLine
 * @details: 普通搜线方法；搜线原则：从中线往左右两边搜线，若搜到改行中点为黑点，则重新搜线
 * @param {二值化数组}
 * @return: RL、LL、ML三个数组，分别为右、左、中
 */
void searchLine(void)
{
	int8 i; //i必须为int
	uint8 j;
	imgInfo.bottom = ROW - 1;
	imgInfo.top = 0;
	uint8 start;
	//先搜最下面一行的左右边线
	//以下6行为哨兵元素，作用：使图像的左右边界始终符合退出循环条件，如此即可减少对于循环是否完毕的判断。
	for (j = imgInfo.lastMid; !imageBin[ROW - 1][j] || imageBin[ROW - 1][j - 1] || imageBin[ROW - 1][j - 2]; j--)
		;
	LL[ROW - 1] = j;
	for (j = imgInfo.lastMid; !imageBin[ROW - 1][j] || imageBin[ROW - 1][j + 1] || imageBin[ROW - 1][j + 2]; j++)
		;
	RL[ROW - 1] = j;
	ML[ROW - 1] = (RL[ROW - 1] + LL[ROW - 1]) >> 1;

	imgInfo.bottom = ROW - 1;
	uint8 sum;
	/*****************搜左右边线***********************/
	for (i = imgInfo.bottom - 1; i >= 0; i--)
	{
		start = ML[i+1];
		if (start < LL[i + 1] + 5 || start > RL[i + 1] - 5)
		{
			start = ML[i + 1];
		}
		//搜线核心
		for (j = start; !imageBin[i][j] || imageBin[i][j - 1] || imageBin[i][j - 2]; j--)
			;
		LL[i] = j;
		for (j = start; !imageBin[i][j] || imageBin[i][j + 1] || imageBin[i][j + 2]; j++)
			;
		RL[i] = j;
		ML[i] = (RL[i] + LL[i]) >> 1;

		for (sum = 0, j = LL[i]; j < RL[i]; j++)
		{
			sum += imageBin[i][j];
		}
		whitePoint[i] = sum;
		width[i] = RL[i] - LL[i];
	}
	//寻找合适的top
	for (i = ROW - 5; i > 2 && imgInfo.top == 0; i--)
	{
		if (whitePoint[i] < 10 || width[i] < 8)
			imgInfo.top = i;
		if (RL[i] < DEFAULT_LEFT + 10)
			imgInfo.top = i;
		if (LL[i] > DEFAULT_RIGHT - 10)
			imgInfo.top = i;
		if (RL[i] < DEFAULT_RIGHT - 15 && RL[i] - RL[i - 1] + 4 < RL[i] - RL[i + 1] && RL[i] - RL[i-1] + 4 < RL[i+1] - RL[i+2])
			imgInfo.top = i;
		if (LL[i] > DEFAULT_LEFT + 15 && LL[i - 1] - LL[i] + 4 < LL[i + 1] - LL[i] && LL[i-1] - LL[i] + 4 < LL[i+2 ] - LL[i+1])
			imgInfo.top = i;
	}
}
/***************最小二乘法求斜率函数*******************/
/**
 * @name: leastSquares
 * @details: 最小二乘法求取中线斜率
 * @param {二值化数组}
 * @return: 斜率b
 */
float leastSquares(void)
{
	float k, b;
	int16 sum1 = 0;
	uint16 sum2 = 0;
	int16 xavg = 0;
	int16 yavg = 0;
	int8 i;
	xavg = (imgInfo.bottom + imgInfo.top) / 2;
	for (i = imgInfo.bottom; i > imgInfo.top; i--)
		yavg += ML[i];
	yavg /= (imgInfo.bottom - imgInfo.top + 1);
	for (i = imgInfo.bottom; i >= imgInfo.top; i--)
	{
		sum1 += (i - xavg) * (ML[i] - yavg);
		sum2 += (i - xavg) * (i - xavg);
	}
	if (sum2 != 0)
		k = sum1 / (float)sum2;
	else
		k = 255;
	return k;
}
uint8 myabs(int16 input)
{
	return(uint8)(input >= 0 ? input : -input);
}