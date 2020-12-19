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

struct cliffs
{
	uint8 topExist;
	uint8 topx;
	uint8 topy;
	uint8 downExist;
	uint8 downx;
	uint8 downy;
};
struct imageInformation
{
	//图像相关信息
	uint8 bottom;
	uint8 top;
	uint8 roadType;
	uint8 lastMid;
	uint8 flag_round_stage;//0:普通状态,1:第一个入口前，2：第二个入口前，3：第三个入口前，4：出环
	uint8 flag_cross_stage;
	uint8 len_col;
	uint8 maxlen;
	uint8 len;
	int16 flag_differ;
	uint8 straight_sure;
	int16 differ;
	float k;
	float b;
	struct cliffs Rcliffs;
	struct cliffs Lcliffs;
	uint8 black_num;

	uint8 car_bottom;
	uint8 car_top;
	uint8 car_exist;
};

extern struct imageInformation imgInfo;
void imageDeal(void);
float leastSquares(void);
void searchLine(void);
void imgInfoInit(void);
void roundDealCliff(void);
void crossDeal(void);
void turnDeal(void);
void getDiffer(void);
void crossDeal(void);
void getSearchStartMid(void);
void getCliffs(void);
void getLen(void);
uint8 getCorrectMin(uint8* line, uint8* y);
void search_associate(void);
void endingDetect();
#endif /* CODE_IMAGEDEAL_H_ */
