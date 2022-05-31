#include "caffe.h"
#include "opencv.hpp"
#include <fstream>
#include <sstream>
#include <vector>
#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
using namespace cv;
using namespace dnn;


static std::vector<std::string> labels;
static Net net;

std::vector<String> readClasslabels(String labelFile = "DL/synset_words.txt")
{
    std::vector<String> classNames;
    std::ifstream fp(labelFile);
    if (!fp.is_open())	{
        throw std::exception("File with classes labels not found");
        //std::cerr << "File with classes labels not found: " << labelFile << std::endl;		exit(-1);
    }
    std::string name;
    while (!fp.eof())
    {
        std::getline(fp, name);
        if (name.length())
            classNames.push_back(name.substr(name.find(' ') + 1));
    }
    fp.close();
    return classNames;
}



void Caffe::LoadModel(String modelTxt, String modelBin,String labelFile){
    try {
    net = dnn::readNetFromCaffe(modelTxt, modelBin);
    labels = readClasslabels(labelFile);
    } catch (cv::Exception & e) {
        throw std::exception(e.what());
    }
    catch(std::exception & e){
        throw e;
    }
}


void Caffe::Predict(cv::Mat Input,std::string &Res,int &MaxLoc, int ImageW, int ImageH,cv::Scalar scalar){
    Res = "null";
    MaxLoc = -1;
    try{
        cv::Mat In;
        if(CV_8UC1 == Input.type()){
            cv::cvtColor(Input,In,COLOR_GRAY2BGR);
        }
        else {
            In = Input.clone();
        }
    Mat inputBlob = blobFromImage(In, 1, Size(ImageW, ImageH), scalar);//mean: Scalar(104, 117, 123)
    // 输入
    net.setInput(inputBlob, "data");
    // 分类预测
    cv::Mat prob = net.forward("prob");

    Mat probMat = prob.reshape(1, 1); //reshape the blob to 1x1000 matrix // 1000个分类
    Point classNumber;
    double classProb;
    minMaxLoc(probMat, nullptr, &classProb, nullptr, &classNumber); // 可能性最大的一个
    int classIdx = classNumber.x; // 分类索引号
    MaxLoc = classIdx;
    Res = labels.at(static_cast<size_t>(classIdx));
    printf_s("\n current image classification : %s, possible : %.2f \n", labels.at(classIdx).c_str(), classProb);
    }catch(std::out_of_range & e){
        std::cout << e.what() <<std::endl;
    }
    catch(cv::Exception & e){
        throw std::exception(e.what());
    }
    catch(std::exception & e){
        throw e;
    }
}



Caffe::Caffe()
{



}
