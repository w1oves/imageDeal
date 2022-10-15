/*
 * @Description: 图像处理、搜线、巡线
 * @version:
 * @Author: wloves
 * @Date: 2020-05-03 20:14:59
 * @LastEditors: wloves
 * @LastEditTime: 2020-09-24 15:10:10
 */
#ifndef CODE_IMAGEDEAL_H_
#define CODE_IMAGEDEAL_H_
#include "mainInclude.h"
struct imageInformation
{
	//图像相关信息
	uint8 bottom;
	uint8 top;
	uint8 roadType;
	uint8 lastMid;
	int16 differ;
	float k;
	float b;
	uint8 black_num;
	uint8 len;

	uint8 car_bottom;
	uint8 car_top;
	uint8 car_exist;
};

extern struct imageInformation imgInfo;
void imageDeal(void);
float leastSquares(void);
void searchLine(void);
void imgInfoInit(void);
void turnDeal(void);
void getDiffer(void);
void getLen(void);
uint8 myabs(int16 input);


#endif /* CODE_IMAGEDEAL_H_ */
