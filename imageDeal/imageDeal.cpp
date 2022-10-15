/*
 * @filename:imageDeal.c
 * @version: 1.0
 * @Description: ͼ�������ߡ�Ѳ�ߣ�����
 * @Author: wloves
 * @Date: 2020 2020��5��3�� ����8:14:50
 * @LastEditors: wloves
 * @LastEditTime: 2020-10-07 20:15:06
 */
#include "imageDeal.h"

#define DEFAULT_MID (IMAGE_W / 2)
#define DEFAULT_LEFT 1
#define DEFAULT_RIGHT COLUMN - 2
 /**
  * @name: �����жϺ궨��
  * @details: ͨ���򵥵ĵ����벻�����ж�
  * @param {uint8 �к�}
  * @return: �Ƿ���
  */
#define isLoseL(i) (LL[i] == DEFAULT_LEFT)
#define isLoseR(i) (RL[i] == DEFAULT_RIGHT)
#define notLoseL(i) (LL[i] != DEFAULT_LEFT)
#define notLoseR(i) (RL[i] != DEFAULT_RIGHT)
  /**
   * @name: limitRl
   * @details: �����޷�������������֤����ʱ���߲�Խ��
   * @param {uint8 �����к�}
   * @return: �޷�����к�
   */
uint8 RL[ROW], ML[ROW], LL[ROW];
uint8 RLB[ROW], MLB[ROW], LLB[ROW];
uint8 whitePoint[ROW];
uint8 width[ROW];
struct imageInformation imgInfo;
uint8 half_width[ROW] = { 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 19, 20, 21, 22, 23, 24, 25, 26, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 42, 44, 46, 48, 50, 52, 54, 56, 58, 61, 63, 65, 67, 69, 71, 73, 75, 77, 79, 81, 83, 85, 87, 89, 90, 91, 91, 91, 91 };
/**
 * @name: imageDeal
 * @details: ͼ����������ǰ�汾��������ֱ����С���������
 * @param {��ֵ������}
 * @return: differ
 */
void imageDeal(void)
{
	//����
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
	//�������
	imgInfo.k = leastSquares();
	turnDeal();
	getDiffer();
}

/**
 * @name: getLen
 * @details: �ж��ֱ������
 * @param {void}
 * @return: �ֱ�����ȼ���Ӧ�кŴ���imgInfo����
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
 * @details: ��ʼ��imgInfo�ṹ��
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
 * @details: ��ȡ��ǰƫ��ֵ
 * @param {void}
 * @return: differ
 */
void getDiffer(void)
{
	//�޸����²��֣�����Ϻ��ʵ�pid�������Դﵽ���Ч��
	int8 i;
	int16 sum = 0;
	int8 lineNum = 1;
	for (i = imgInfo.top; i < imgInfo.bottom; i++)
	{
		MLB[i] = (LLB[i] + RLB[i]) >> 1;
	}
	//Ȩ�����ã�����Ȩ�ش�Զ��Ȩ��С
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
 * @details: ʮ�ִ�������������
 * @param {type}
 * @return:
 */
void turnDeal(void)
{
	//������С���˷���ϵõ���б���ж��Ƿ�Ϊ���
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

/***************���ߺ���*******************/
/**
 * @name: searchLine
 * @details: ��ͨ���߷���������ԭ�򣺴������������������ߣ����ѵ������е�Ϊ�ڵ㣬����������
 * @param {��ֵ������}
 * @return: RL��LL��ML�������飬�ֱ�Ϊ�ҡ�����
 */
void searchLine(void)
{
	int8 i; //i����Ϊint
	uint8 j;
	imgInfo.bottom = ROW - 1;
	imgInfo.top = 0;
	uint8 start;
	//����������һ�е����ұ���
	//����6��Ϊ�ڱ�Ԫ�أ����ã�ʹͼ������ұ߽�ʼ�շ����˳�ѭ����������˼��ɼ��ٶ���ѭ���Ƿ���ϵ��жϡ�
	for (j = imgInfo.lastMid; !imageBin[ROW - 1][j] || imageBin[ROW - 1][j - 1] || imageBin[ROW - 1][j - 2]; j--)
		;
	LL[ROW - 1] = j;
	for (j = imgInfo.lastMid; !imageBin[ROW - 1][j] || imageBin[ROW - 1][j + 1] || imageBin[ROW - 1][j + 2]; j++)
		;
	RL[ROW - 1] = j;
	ML[ROW - 1] = (RL[ROW - 1] + LL[ROW - 1]) >> 1;

	imgInfo.bottom = ROW - 1;
	uint8 sum;
	/*****************�����ұ���***********************/
	for (i = imgInfo.bottom - 1; i >= 0; i--)
	{
		start = ML[i+1];
		if (start < LL[i + 1] + 5 || start > RL[i + 1] - 5)
		{
			start = ML[i + 1];
		}
		//���ߺ���
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
	//Ѱ�Һ��ʵ�top
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
/***************��С���˷���б�ʺ���*******************/
/**
 * @name: leastSquares
 * @details: ��С���˷���ȡ����б��
 * @param {��ֵ������}
 * @return: б��b
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