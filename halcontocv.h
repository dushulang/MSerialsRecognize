#ifndef HALCONTOCV_H
#define HALCONTOCV_H

#include "halconcpp.h"
#include "opencv.hpp"
#include <stdio.h>
#include <iostream>
//#include "global.h"

namespace MSerials{

void h_disp_obj(HalconCpp::HObject &obj, HalconCpp::HTuple disp_hd);

//halcon 转opencv
void HObject2Mat(HalconCpp::HObject& shobj,cv::Mat &dst);

//opencv 转halcon
void Mat2Hobj(cv::Mat &InputArray, HalconCpp::HObject &Hobj);

};
#endif // HALCONTOCV_H
