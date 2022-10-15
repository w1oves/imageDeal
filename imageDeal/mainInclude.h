#pragma once
#ifndef _mainInclude
#define _mainInclude
#include<stdlib.h>
#include<string.h>

#define IMAGE_H 120
#define IMAGE_W 188

#define ROW 60
#define COLUMN 188

typedef unsigned char       uint8;
typedef unsigned short      uint16;
typedef unsigned long       uint32;
typedef unsigned long long  uint64;
typedef char int8;
typedef short int16;
typedef float               float32;        /* IEEE754-2008 single precision */
typedef double              float64;        /* IEEE754-2008 double precision */
#include"imageDeal.h"
#include"imagePreDeal.h"
extern uint8 mt9v30x_image[120][188];
extern uint8 imageGray[ROW][COLUMN];
extern uint8 imageBin[ROW][COLUMN];
extern uint8 RL[ROW], ML[ROW], LL[ROW];
extern uint8 RLB[ROW], MLB[ROW], LLB[ROW];
extern uint8 RLC[ROW], MLC[ROW], LLC[ROW];
extern int number_image;
/***************道路参数*******************/
//直道、弯道、左转、右转
#define ROAD_straight 0
#define ROAD_trun 10
#define ROAD_turn_left 11
#define ROAD_turn_right 12
//急转、左急转、右急转
#define ROAD_big_turn 20
#define ROAD_big_turn_left 21
#define ROAD_big_turn_right 22
#endif // !_mainInclude