#pragma once
/*
 * @Description: 图像预处理，二值化的过程
 * @version:
 * @Author: wloves
 * @Date: 2020-05-03 20:14:25
 * @LastEditors: wloves
 * @LastEditTime: 2020-07-23 19:28:59
 */
#ifndef CODE_IMAGEPREDEAL_H_
#define CODE_IMAGEPREDEAL_H_
#include"mainInclude.h"
#include<algorithm>
using namespace std;
extern uint8 threshold_image;
extern uint8 mt9v30x_image[120][188];
enum binaryMethod{OTSU,OTSU2D,SAUVOLA,SOBEL,BALANCE,MEDIANFILTER};
uint8 otsuTh(void);
void sauvola();
void process(binaryMethod method);
void medianFilter(void);
int otsu2dTh(void);
void imageSample(void);
void sobel(void);
#endif /* CODE_IMAGEPREDEAL_H_ */
