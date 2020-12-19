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
#define isRound (imgInfo.flag_round_stage > 0 && imgInfo.flag_round_stage < 6)
  /**
   * @name: limitRl
   * @details: 边线限幅内联函数，保证补线时边线不越界
   * @param {uint8 边线列号}
   * @return: 限幅后的列号
   */
inline uint8 limitRL(int16 input)
{
	if (input < 0)
		return 0;
	else if (input < COLUMN)
		return (uint8)input;
	else
		return COLUMN;
}
uint8 RL[ROW], ML[ROW], LL[ROW];
uint8 RLB[ROW], MLB[ROW], LLB[ROW];
int avg_mid;
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
	endingDetect();
	searchLine();
	getCliffs();
	memcpy(RLB, RL, ROW);
	memcpy(LLB, LL, ROW);
	memcpy(MLB, ML, ROW);
	imgInfo.roadType = ROAD_straight;
	imgInfo.lastMid = (ML[ROW - 4] + ML[ROW - 3] + ML[ROW - 2] + ML[ROW - 1]) >> 2;
	//弯道处理
	imgInfo.k = leastSquares();
	turnDeal();
	//环岛处理
	//roundDealCliff();
	// if (debugFlag.time_s < 7)
	// 	imgInfo.len = 50;
	//得到最终differ
	//crossDeal();
	getDiffer();

	pid_set(imgInfo.roadType);
	debugFlag.differOrigin = differ;

	//carControl();
}

void endingDetect(void)
{
	uint8 endline = ROW - 30;
	int8 i;
	uint8 j;
	uint8 black_flag = 0;
	uint8 white = 0;
	uint8 black = 0;
	uint8 bn = 0;
	for (i = ROW - 15; i > ROW - 45 && bn <= 5; i--)
	{
		bn = 0;
		endline = i;
		for (j = DEFAULT_RIGHT; j > DEFAULT_LEFT; j--)
		{
			if (black_flag == imageBin[endline][j])
			{
				if (black_flag)
				{
					if (black >= 2 && white >= 2)
						bn++;
					black = 0;
					white = 0;
				}
				black_flag = !imageBin[endline][j];
			}

			if (black_flag)
			{
				black++;
			}
			else
			{
				white++;
			}
		}
	}
	imgInfo.black_num = bn;
	if (imgInfo.black_num > 5)
	{
		drive_servo(DEFAULT_MID);
	}
#if 0
	uint8 endingline;
	endingline = ROW - 20;
	uint8 black_sure = 0;
	for (uint8 i = ROW - 10; i > ROW - 40; i--)
	{
		uint8 black_flag = 1;
		imgInfo.black_num = 0;
		for (uint8 j = DEFAULT_RIGHT - 5; j > DEFAULT_LEFT + 5; j--)
		{
			if (black_flag == 1)
			{
				if (imageBin[endingline][j] == 1)
				{
					black_flag = 0;
				}
				else continue;
			}
			else
			{
				if (imageBin[endingline][j] == 0 && width[endingline - 5] > half_width[endingline - 5] / 2 && width[endingline + 5] > half_width[endingline + 5] / 2)
				{
					black_flag = 1;
					imgInfo.black_num++;
				}
				else continue;
			}
		}
		debugFlag.black_num = imgInfo.black_num;

		if (imgInfo.black_num >= 5)
			black_sure++;
		else black_sure = 0;
	}
	if (black_sure >= 3)
	{
		differ = 50;
		drive_servo(differ);
		while (1)
		{
			drive_motor(0, 0);
		}
	}
#endif
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
			imgInfo.len_col = j;
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
	imgInfo.roadType = ROAD_straight;
	imgInfo.top = 0;
	imgInfo.flag_differ = 0;
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
#if 1
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

#elif 0
	//权重设置：近处权重小，远处权重大
	for (i = imgInfo.top; i < imgInfo.bottom; i++)
	{
		sum += MLB[i];
	}
	lineNum = imgInfo.bottom - imgInfo.top;
	sum /= lineNum;
#else
	//取底下十行
	if (imgInfo.flag_round_stage != 5)
	{
		uint8 start, end;
		uint8 len = imgInfo.bottom - imgInfo.top;
		start = imgInfo.bottom - 8;
		//		start -= (leftSpeed + rightSpeed) / 40;
		//		if (start - 15 < imgInfo.top)
		//			start = imgInfo.top + 15;
		end = start - 15;
		if (end < imgInfo.top)
			end = imgInfo.top + 1;
		if (start < end + 5)
			start = imgInfo.bottom;
		// end = start - len / 2;
		// if (start - end < 14)
		// {
		// 	end = start - 14;
		// }
		// if (end < imgInfo.top)
		// 	end = imgInfo.top;
		lineNum = 0;
		for (i = start; i > end; i--)
		{
			sum += MLB[i];
			lineNum++;
		}
		sum /= lineNum;
	}
	else
	{
		for (i = imgInfo.top; i < imgInfo.bottom; i++)
		{
			sum += MLB[i];
		}
		lineNum = imgInfo.bottom - imgInfo.top;
		sum /= lineNum;
	}

#endif
#if 0
	//对转向偏差进行二次函数处理
	if (sum < 15)
		sum = sum / 2;
	else
		sum = sum * myabs(sum) / 30;
	if (imgInfo.len > 30)
	{
		if (sum > 0)
			sum = sum * sum / 48;
		else
			sum = -sum * sum / 48;
	}

	if (sum > 95)
		sum = 95;
	if (sum < -95)
		sum = -95;
#endif
	sum -= DEFAULT_MID;
	debugFlag.differOrigin = sum;

	if (imgInfo.flag_differ != 0)
	{

		differ = imgInfo.flag_differ;
		imgInfo.flag_differ = 0;
	}
	else
		differ = sum;
}
/**
 * @name: getCliffs
 * @details: 获取前后左右四个拐点
 * @param {void}
 * @return: 结果存入Lcliffs和Rcliffs结构体
 */
void getCliffs(void)
{
	int8 i;
	imgInfo.Lcliffs.downExist = 0;
	imgInfo.Lcliffs.topExist = 0;
	imgInfo.Rcliffs.downExist = 0;
	imgInfo.Rcliffs.topExist = 0;
	uint8 top = imgInfo.top;
	uint8 bottom = ROW - 6;
	top = (top - 5 >= 0) ? (top - 5) : top;
	for (i = bottom; i >= top; i--)
	{
		if (LL[i] < 60)
			if (LL[i] >= LL[i + 2] && LL[i] >= LL[i - 2] * 2 + 10 && LL[i - 2] <= 20 && LL[i - 4] <= 20)
				if (imageBin[i - 1][LL[i]] && imageBin[i][LL[i] + 1])
					if (LL[i] > 2 && LL[i] < COLUMN - 3 && i > 2 && i < ROW - 2)
					{
						imgInfo.Lcliffs.downx = LL[i];
						imgInfo.Lcliffs.downy = i;
						imgInfo.Lcliffs.downExist = 1;
						break;
					}
	}
	for (i = bottom; i >= top; i--)
	{
		if (LL[i] > LL[i + 2] * 2 + 3 && myabs(LL[i] - LL[i - 1]) < 5 && LL[i + 3] < 10 && LL[i] > 30)
			if (imageBin[i + 1][LL[i]] && imageBin[i][LL[i] + 1])
				if (LL[i] > 2 && LL[i] < COLUMN - 3 && i > 2 && i < ROW - 2)
				{
					imgInfo.Lcliffs.topx = LL[i];
					imgInfo.Lcliffs.topy = i;
					imgInfo.Lcliffs.topExist = 1;
					break;
				}
	}
	for (i = bottom; i >= top; i--)
	{
		if (RL[i] <= RL[i + 2] && RL[i] <= DEFAULT_RIGHT - (DEFAULT_RIGHT - RL[i - 2]) * 2 - 10 && RL[i - 2] >= DEFAULT_RIGHT - 20 && RL[i - 4] >= DEFAULT_RIGHT - 20)
			if (imageBin[i - 1][RL[i]] && imageBin[i][RL[i] - 1])
				if (RL[i] > 2 && RL[i] < COLUMN - 3 && i > 2 && i < ROW - 2)
				{
					imgInfo.Rcliffs.downx = RL[i];
					imgInfo.Rcliffs.downy = i;
					imgInfo.Rcliffs.downExist = 1;
					break;
				}
	}
	for (i = bottom; i >= top; i--)
	{
		if (RL[i] < DEFAULT_RIGHT - (DEFAULT_RIGHT - RL[i + 2]) * 2 - 3 && myabs(RL[i] - RL[i - 1]) < 5 && RL[i + 3] > DEFAULT_RIGHT - 10 && RL[i] < DEFAULT_RIGHT - 30)
			if (imageBin[i + 1][RL[i]] && imageBin[i][RL[i] - 1])
				if (RL[i] > 2 && RL[i] < COLUMN - 3 && i > 2 && i < ROW - 2)
				{
					imgInfo.Rcliffs.topx = RL[i];
					imgInfo.Rcliffs.topy = i;
					imgInfo.Rcliffs.topExist = 1;
					break;
				}
	}
}
/**
 * @name: crossDeal
 * @details: 十字处理函数，待完善
 * @param {type}
 * @return:
 */
void crossDeal(void)
{
	// static uint8 cross_count=0;
	static uint8 cross_flag = 0;
	if (isRound)
		return;
	if ((imgInfo.Rcliffs.topExist || imgInfo.Rcliffs.downExist) && (imgInfo.Lcliffs.topExist || imgInfo.Lcliffs.downExist))
		if ((imgInfo.Rcliffs.topExist + imgInfo.Rcliffs.downExist) + (imgInfo.Lcliffs.topExist + imgInfo.Lcliffs.downExist) >= 3)
			cross_flag++;
	if (2 == cross_flag)
	{
		if (!imgInfo.Rcliffs.downExist && !imgInfo.Lcliffs.downExist && imgInfo.Rcliffs.topExist && imgInfo.Lcliffs.topExist)
		{
			cross_flag = 0;
		}
		imgInfo.roadType = ROAD_cross;
		imgInfo.bottom = ROW - 1;
		imgInfo.top = ROW / 2;
	}
	// if (cross_flag)
	// {
	// 	imgInfo.roadType = ROAD_cross;
	// 	searchLineCross();
	// }
	// // cross_count=imgInfo.Rcliffs.topExist+imgInfo.Rcliffs.downExist+imgInfo.Lcliffs.downExist+imgInfo.Lcliffs.topExist;
	// if ((imgInfo.Rcliffs.topExist || imgInfo.Rcliffs.downExist) && (imgInfo.Lcliffs.topExist || imgInfo.Lcliffs.downExist))
	// 	cross_flag = 1;
	// else
	// 	cross_flag = 0;
	// if (cross_flag)
	// {
	// }
	// for (i = imgInfo.bottom; i >= imgInfo.top && isLoseR(i); i--)
	// 	;
	// for (; i >= imgInfo.top && notLoseR(i); i--)
	// 	;
	// Rlose.downx = RL[i];
	// Rlose.downy = i;
	// for (; i >= imgInfo.top && isLoseR(i); i--)
	// 	;
	// Rlose.topx = RL[i];
	// Rlose.topy = i;
	// for (i = imgInfo.bottom; i >= imgInfo.top && isLoseL(i); i--)
	// 	;
	// for (; i >= imgInfo.top && notLoseL(i); i--)
	// 	;
	// Llose.downx = LL[i];
	// Llose.downy = i;
	// for (; i >= imgInfo.top && isLoseL(i); i--)
	// 	;
	// Llose.topx = LL[i];
	// Llose.topy = i;
	// Llose.len=Llose.downy - Llose.topy;
	// Rlose.len=Rlose.downy - Rlose.topy ;
	// if ( Llose.len> ROW / 6 && Rlose.len> ROW / 6)
	// {
	// 	imgInfo.roadType=ROAD_big_turn;
	// 	imgInfo.top=(Rlose.downy+Llose.downy)>>1;
	// 	// Llose.midy = Llose.topy + Llose.downy;
	// 	// Rlose.midy = Rlose.topy + Rlose.downy;
	// 	// if (Llose.midy < Rlose.midy)
	// 	// {
	// 	// 	for (i = Llose.downy; i > Llose.topy; i--)
	// 	// 		if (RL[i] - half_width[i] * 2 > 0)
	// 	// 			LLB[i] = RL[i] - half_width[i] * 2;
	// 	// }
	// 	// else
	// 	// {
	// 	// 	for (i = Rlose.downy; i > Rlose.topy; i--)
	// 	// 		if (LL[i] + half_width[i] * 2 < 188)
	// 	// 			RLB[i] = LL[i] + half_width[i] * 2;
	// 	// }
	// }
}
/**
 * @name: turnDeal
 * @details: 弯道处理函数
 * @param {imgInfo.k}
 * @return: 道路类型
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
#define storeN (uint8)(1 + COLUMN / 8)
/**
 * @name: getCorrectMin
 * @details: 暂未使用
 * @param {type}
 * @return:
 */
uint8 getCorrectMin(uint8* line, uint8* y)
{
	uint8 i;
	uint8 store[storeN] = { 0 };
	for (i = ROW; i > 0; i--)
	{
		store[line[i] >> 3] = i - 1;
	}
	for (i = 0; i < storeN - 1; i++)
	{
		if (store[i] && store[i + 1])
			break;
	}
	*y = i;
	if (i == storeN - 1)
		return 255;
	else
		return i * 8 + 4;
}
/**
 * @name: roundDealCliff
 * @details: 环岛处理函数
 * @param {边线}
 * @return: 环岛补线
 */
void roundDealCliff(void)
{
	// if(imgInfo.straight_sure)
	// {
	// 	imgInfo.flag_round_stage=0;
	// 	return;
	// }
	int8 i;
	uint8 k;
	uint8 nowStage = imgInfo.flag_round_stage;
	static uint8 count = 0;
	//寻找入口处下面的拐点
	uint8 sum = 0;
	uint8 minR = COLUMN;
	uint8 minIndexR = 0;
	//阶段处理部分（决定在阶段内该如何行动
	switch (imgInfo.flag_round_stage)
	{
	case 0:
		//入环前
		//imgInfo.roadType = ROAD_straight;
		break;
	case 1:

		//准备入环阶段
		//imgInfo.roadType = ROAD_straight;
		//		ips200_clear(RED);
		for (i = 0; i < ROW; i++)
		{
			if (isLoseL(i))
				LLB[i] = limitRL(RL[i] - 2 * half_width[i]);
		}

		break;
	case 2:
		//入环阶段

		//imgInfo.roadType = ROAD_straight;
		if (!imgInfo.Lcliffs.topExist)
		{
			imgInfo.Lcliffs.topx = limitRL(RL[15] - half_width[15] * 2);
			imgInfo.Lcliffs.topy = 10;
		}
		//		k = ((imgInfo.Lcliffs.topx + RL[ROW * 80 / 100]) >> 1);
		//		for (i = 54; i > imgInfo.Lcliffs.topy; i--)
		//		{
		//			RLB[i] = k - imgInfo.Lcliffs.topy + 50;
		//		}
		for (i = ROW - 1; i > ROW / 2; i--)
			if (isLoseL(i))
			{
				RLB[i] = LL[i] + half_width[i] + 30;
			}
		imgInfo.top = imgInfo.Lcliffs.topy;
		if (imgInfo.top > ROW - 10)
			imgInfo.top = ROW - 10;
		imgInfo.bottom = ROW - 1;

		break;
	case 3:
		//环内阶段
		//按照右边跑

		imgInfo.roadType = ROAD_turn_left;
		for (i = ROW - 3; i >= 2; i--)
		{
			if (isLoseL(i))
			{
				if (RL[i] >= 2 * half_width[i])
					LLB[i] = RL[i] - 2 * half_width[i];
				else
					LLB[i] = 0;
			}
			if (isLoseR(i) && i > imgInfo.top)
			{
				RLB[i] = DEFAULT_MID + 10;
			}
			if (i < ROW - 10 && RL[i] < RL[i - 2] && RL[i] < RL[i + 2] && RL[i] < RL[i - 1] && notLoseR(i + 2))
			{
				imgInfo.top = i;
				break;
			}
		}
		break;
	case 4:
		// while(1)
		// {
		// 	carCtrl.mode=CTRL_STOP;
		// }
		//imgInfo.roadType=ROAD_turn_left;
		//pid_set(ROAD_big_turn);
		for (i = ROW - 1; i > imgInfo.top; i--)
		{
			if (isLoseR(i))
				RLB[i] = DEFAULT_MID + 10;
		}
		break;
	case 5:
		//防止再次入环阶段
		//根据左拐点进行补线，防止再次入环

		imgInfo.roadType = ROAD_straight;
		for (i = 5; i < ROW - 5; i++)
		{
			if (LL[i] < 40)
			{
				LLB[i] = limitRL(RL[i] - half_width[i] * 2) + 40;
			}
		}
		break;
	case 6:
	default:
		break;
	}
	static uint8 timer = 0;

	switch (imgInfo.flag_round_stage)
	{
	case 0:
		//如果找到入口处拐点，进入下一阶段
		if (imgInfo.Lcliffs.downExist && imgInfo.Lcliffs.downy >= 0.2 * ROW && !imgInfo.Rcliffs.downExist && !imgInfo.Rcliffs.topExist)
			if (imgInfo.Lcliffs.downy > ROW / 2)
			{
				uint8 down, top;
				uint8 flag = 0;
				if (imgInfo.Lcliffs.downy + 15 > ROW - 1)
					down = ROW - 1;
				else
					down = imgInfo.Lcliffs.downy + 15;
				if (imgInfo.Lcliffs.downy - 15 < 0)
					top = 0;
				else
					top = imgInfo.Lcliffs.downy - 15;
				for (i = down; i > top; i--)
				{
					if (RL[i] > DEFAULT_MID + half_width[i] + 15)
						flag++;
				}
				if (flag < 5)
					timer++;
				// carCtrl.distance = 0;
			}
		if (timer >= 2)
		{
			//			carCtrl.distance=0;
			imgInfo.flag_round_stage++;
		}
		break;
	case 1:
		//如果找到入口处上拐点，进入下一阶段
		if (imgInfo.Lcliffs.topExist && !imgInfo.Rcliffs.topExist && notLoseR(30))
		{
			for (i = imgInfo.Lcliffs.topy + 10; i < ROW; i += 3)
			{
				if (LL[i] > 8)
					break;
			}
			if (i >= ROW - 10 && imgInfo.Lcliffs.topy < 40)
			{
				imgInfo.flag_round_stage++;
			}
		}
		else if (carCtrl.distance > 5000 && !imgInfo.Lcliffs.topExist && !imgInfo.Lcliffs.downExist)
		{
			imgInfo.flag_round_stage = 0;
		}
		break;
	case 2:
		//根据拐点进行补线入环
		for (i = ROW - 2; i > 1 && RL[i - 1] <= RL[i]; i--)
			;
		if (RL[i + 2] < 80 && RL[i + 1] < 80 && RL[imgInfo.top + 3] < 80)
			imgInfo.flag_round_stage++;
		break;
	case 3:
		//按照右边线跑
		if (imgInfo.top > ROW * 2 / 3)
		{
			i = imgInfo.top;
			if (i < ROW - 10 && RL[i] < RL[i - 2] && RL[i] < RL[i + 2] && RL[i] < RL[i - 1] && notLoseR(i + 2))
				imgInfo.flag_round_stage++;
		}

		// for (i = ROW - 1; i >= 0 && LL[i] < COLUMN / 2; i -= 10)
		// 	;
		// if (i <= -1)
		// 	if (imgInfo.len > ROW * 0.7 && notLoseR(10) && notLoseR(20) && notLoseR(30) && RL[50] > RL[30] && RL[30] > RL[10])
		// 	{
		// 		imgInfo.flag_round_stage++;
		// 	}
		break;
	case 4:
		imgInfo.flag_differ = -45;
		for (i = ROW - 6; i > ROW * 0.1; i--)
			if (isLoseR(i))
				break;
		timer++;
		if (timer > 7)
		{
			imgInfo.flag_round_stage++;
			timer = 0;
		}
		break;
	case 5:
		//根据左拐点进行补线，防止再次入环

		for (i = 40; i > 20; i--)
			if (isLoseR(i) || isLoseL(i))
				break;
		if (i == 20)
			imgInfo.flag_round_stage = 0;
		break;
	default:
		break;
	}

	if (carCtrl.distance > 14000 && imgInfo.flag_round_stage != 6)
	{
		//imgInfo.flag_round_stage = 6;
	}
	if (nowStage != imgInfo.flag_round_stage)
		carCtrl.distance = 0;
	switch (imgInfo.flag_round_stage)
	{
	case 0:
		//gpio_set(P33_9,0);
		//drive_motor(0,0);
		break;
	case 1:
		//gpio_set(P33_9,1);
		break;
	case 2:
		//gpio_set(P33_9,0);
		break;
	case 3:
		//gpio_set(P33_9,1);
		break;
	case 4:
		//gpio_set(P33_9,0);
		break;
	case 5:
		//gpio_set(P33_9,1);
		break;
	case 6:
		//gpio_set(P33_9,0);
		break;
	}
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
	pid_set(ROAD_straight);
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

	for (j = imgInfo.lastMid; !imageBin[ROW - 1][j] || imageBin[ROW - 1][j - 1] || imageBin[ROW - 1][j - 2]; j--)
	{
#if 0
		if (j == imgInfo.lastMid && !imageBin[ROW - 1][j - 1] && !imageBin[ROW - 1][j] && !imageBin[ROW - 1][j + 1])
		{
			carCtrl.mode = CTRL_STOP;
			drive_motor(0, 0);
		}
#endif
	}
	imgInfo.bottom = ROW - 1;
	uint8 sum;
	/*****************搜左右边线***********************/
	for (i = imgInfo.bottom - 1; i >= 0; i--)
	{
		start = imgInfo.len_col;
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
	for (i = ROW - 5; i > 1 && imgInfo.top == 0; i--)
	{
		if (RL[i] < DEFAULT_LEFT + 10)
			imgInfo.top = i;
		if (LL[i] > DEFAULT_RIGHT - 10)
			imgInfo.top = i;
		if (RL[i] < DEFAULT_RIGHT - 15 && RL[i] - RL[i - 1] + 3 < RL[i] - RL[i + 1])
			imgInfo.top = i;
		if (LL[i] > DEFAULT_LEFT + 15 && LL[i - 1] - LL[i] + 3 < LL[i + 1] - LL[i])
			imgInfo.top = i;
	}
}
/**
 * @name: searchLineCross
 * @details: 十字搜线方法：搜线原则：从右边线往左搜线，并从左边线往右搜线
 * @param {二值化数组}
 * @return: RL、LL、ML三个数组，分别为右、左、中
 */
void searchLineCross(void)
{
	int8 i; //i必须为int
	uint8 j;
	uint8 flag = 0;
	//先搜最上面一行的左右边线
	//以下6行为哨兵元素，作用：使图像的左右边界始终符合退出循环条件，如此即可减少对于循环是否完毕的判断。
	for (j = DEFAULT_RIGHT; !imageBin[ROW - 1][j] || imageBin[ROW - 1][j - 1] || imageBin[ROW - 1][j - 2]; j--)
		;
	LLB[ROW - 1] = j;
	for (j = DEFAULT_LEFT; !imageBin[ROW - 1][j] || imageBin[ROW - 1][j + 1] || imageBin[ROW - 1][j + 2]; j++)
		;
	RLB[ROW - 1] = j;
	MLB[ROW - 1] = (RLB[ROW - 1] + LLB[ROW - 1]) >> 1;
	/*****************搜左右边线***********************/
	for (i = ROW - 2; i >= 0; i--)
	{
		for (j = RLB[i + 1]; !imageBin[i][j] || imageBin[i][j - 1] || imageBin[i][j - 2]; j--)
			;
		LLB[i] = j;
		for (j = LLB[i + 1]; !imageBin[i][j] || imageBin[i][j + 1] || imageBin[i][j + 2]; j++)
			;
		RLB[i] = j;
		if (i < ROW - 10)
		{
			if (RLB[i] > 1 + RLB[i + 1])
				RLB[i] = RLB[i + 1] - half_width[i + 1] + half_width[i];
			if (LLB[i] < LLB[i + 1] - 1)
				LLB[i] = LLB[i + 1] + half_width[i + 1] - half_width[i];
		}

		MLB[i] = (RLB[i] + LLB[i]) >> 1;
		width[i] = RLB[i] - LLB[i];
	}
}
void setTop(uint8 roadType, uint8 i)
{
	imgInfo.roadType = roadType;
	imgInfo.top = i;
	pid_set(roadType);
}
/**
 * @name: getEffectiveSection
 * @details: 粗略获取左右引导线、中线的有效区间；
 * @param {二值化数组}
 * @return: 结果存入imgInfo结构体
 */
void getEffectiveSection(void)
{
	uint8 i;
	for (i = 0; i < ROW - 1; i++)
		if (LL[i] - 2 || RL[i] - (COLUMN - 3))
			break;
	imgInfo.bottom = i;
	for (i = ROW - 1; i > 0; i--)
		if (LL[i] - 2 || RL[i] - (COLUMN - 3))
			break;
	for (; i > imgInfo.bottom; i--)
		if (RL[i] - LL[i] > 2)
			break;
	imgInfo.top = i;
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

uint8 isStraight(void)
{
	int8 i;
	uint8 notLose = 0;
	for (i = ROW - 5; i >= 5; i--)
	{
		if (notLoseL(i) && notLoseR(i))
			notLose++;
		if (RL[i] - RL[i + 1] > 5 && RL[i - 1] - RL[i + 2] > 5)
			return 0;
		if (LL[i + 1] - LL[i] > 5 && LL[i + 2] - LL[i - 1] > 5)
			return 0;
	}
	if (notLose > 45)
	{
		return 1;
	}
	return 0;
}
//uint8 addLineWithSlope(uint8 *line, uint8 len)
//{
//for (uint8 i = 0; i < len; i++)
//line[i] = line[0] + i * (line[len - 1] - line[0]) / (len - 1);
//}
