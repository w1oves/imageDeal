#pragma once
#ifndef _mainInclude
#define _mainInclude
#include<stdlib.h>
#include<string.h>
#define IMAGE_W 188
#define ROW 60
#define COLUMN 188
#define _ROW 60
#define _COLUMN 188
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
extern int16 differ;
extern int leftSpeed;
extern int rightSpeed;
struct lose
{
    uint8 topx;
    uint8 topy;
    uint8 downx;
    uint8 downy;
    uint8 midy;
    uint8 len;
};
class _debugFlag
{
public:
    uint8 stop;      //0：初始状态；1：强制停车
    uint8 imageType; //0:不发送；1：灰度图像；2：二值化图像；3：搜线图像
    uint8 sendFlag;  //0:不发送，1：发送一次；

    uint8 time_ms; //ms计时
    uint8 time_s;  //s计时
    uint8 Fps;     //每秒处理图像的次数
    uint8 Fps_out;
    uint8 loop_cnt;
    uint8 camera_fps;
    uint8 camera_exp;
    uint16 duty; //

    uint8 show;
    //需要显示的变量
    int16 differOrigin; //当前舵机需偏移量-PID前
    int16 differPid;    //当前舵机偏移量-PID后

    int16 leftspeed;  //左轮速度
    int16 rightspeed; //右轮速度
    int16 leftPwn;
    int16 rightPwm;

    float k;

    uint8 LEnterDx;
    uint8 LEnterDy;
    uint8 LEnterUx;
    uint8 LEnterUy;
    struct lose Rlose;
    struct lose Llose;
};
extern _debugFlag debugFlag;

class _carControl
{
public:
    uint8 start_park;//0：初始状态;1：启动车辆；2:入库
    uint8 mode;
    int16 distance;
    int8 flag_associate;
};
extern _carControl carCtrl;
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
//十字
#define ROAD_cross 30
//斜坡
#define ROAD_ramp 40
//环岛
#define ROAD_round 50
#define ROAD_round2 51
#define ROAD_round3 52
#define ROAD_round4 53

//引脚定义
#define SLAVE_UART UART_3
#define SLAVE_UART_TX UART3_TX_P20_3
#define SLAVE_UART_RX UART3_RX_P15_7

#define LED1 P20_8
#define OUT1 P02_5
#define OUT2 P02_7

#define S_MOTOR_PIN ATOM1_CH1_P33_9 //定义舵机引脚




#define MOTOR1_A ATOM0_CH6_P02_6 //定义2电机正转PWM引脚
#define MOTOR1_B ATOM0_CH4_P02_4 //定义2电机反转PWM引脚
#define MOTOR2_A ATOM0_CH0_P21_2 //定义1电机反转PWM引脚
#define MOTOR2_B ATOM0_CH2_P21_4//定义1电机正转PWM引脚

//定义按键引脚
#define KEY1 P22_0
#define KEY2 P22_1
#define KEY3 P22_2
#define KEY4 P22_3
//定义拨码开关引脚
#define SW1 ADC0_CH6_A6
#define SW2 ADC0_CH7_A7
#define SW3 ADC0_CH8_A8
#define SW4 ADC0_CH10_A10
#define pid_set(x) ;
#define pid_servo(x) ;
#define drive_servo(x) ;

int  myabs(int dat);
#define P33_9 0
#define gpio_set(x,y) ;
#endif // !_mainInclude