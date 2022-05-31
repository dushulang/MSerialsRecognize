#ifndef CAFFE_H
#define CAFFE_H
#include "opencv.hpp"
#include <iostream>
using namespace cv;

#define USE_CAFFE

class Caffe
{
public:
    Caffe();
    Caffe(const char * Sel){
        std::cout <<"-----------------"<<std::endl;
        printf_s(Sel);
        try {
            LoadModel();
        } catch (std::exception & e) {
            printf_s(e.what());
        }

    }

    void Print(){std::cout <<"--------Caffe-----------------"<<std::endl;}
    void LoadModel(String modelTxt = "DL/caffe.prototxt", String modelBin = "DL/caffe.caffemodel",String labelFile = "DL/synset_words.txt");
    void Predict(cv::Mat Input,std::string & Res, int & MaxLoc,int ImageW = 320, int ImageH = 200, cv::Scalar scalar = Scalar(98, 98, 98));
};

#endif // CAFFE_H
