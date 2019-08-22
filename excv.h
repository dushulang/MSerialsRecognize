#ifndef EXCV_H
#define EXCV_H
#include "opencv.hpp"
#include <opencv2\imgproc\types_c.h>
#include "HalconCpp.h"
#include <vector>
#include <QDateTime>
//#include "preferences.h"


#define ORGIDX          5


namespace Excv {

#ifndef AN_HOUR
#define AN_HOUR	3600000
#endif

template<typename _T>
static bool WaitValue(_T &T,size_t TimeOut = 2000)
{
    clock_t clk = clock();
    while(T>0)
    {
        if((clock()-clk)>TimeOut)
        {
            return false;
        }
        Sleep(2);
    }
    return true;
}



static inline void Contours_Move(const std::vector<std::vector<cv::Point>> &origin, std::vector<std::vector<cv::Point>> &dst, cv::Point direct)
{

    dst.assign(origin.begin(), origin.end());
    for (size_t i = 0; i < origin.size(); i++)
        for (size_t p = 0; p < origin[i].size(); p++)
        {
            dst[i][p].x = origin[i][p].x + direct.x;
            dst[i][p].y = origin[i][p].y + direct.y;
        }
}


static bool CheckCabliImage(const cv::Mat & InputArray,cv::Mat &color_view, cv::Size board_size = cv::Size(6,9))
{
    using namespace cv;
    std::vector<cv::Point2f> image_points_buf;  /* 缓存每幅图像上检测到的角点 */
    if (0 == findChessboardCorners(InputArray, board_size, image_points_buf)) {
        return false;
    }
    find4QuadCornerSubpix(InputArray, image_points_buf, Size(11, 11)); //对粗提取的角点进行精确

    if(CV_8UC1 == InputArray.type()){
        cv::cvtColor(InputArray,color_view,CV_GRAY2BGR);
    }else {
        color_view = InputArray.clone();
    }


    cv::drawChessboardCorners(color_view, board_size, image_points_buf, true); //用于在图片中标记角点
    return true;
}


static std::vector<cv::Point2f> CheckCabliImageEx(const cv::Mat & InputArray,cv::Mat &color_view, cv::Size board_size = cv::Size(6,9))
{
    using namespace cv;
    std::vector<cv::Point2f> image_points_buf;  /* 缓存每幅图像上检测到的角点 */
    if (0 == findChessboardCorners(InputArray, board_size, image_points_buf)) {
        image_points_buf.clear();
        return image_points_buf;
    }
    find4QuadCornerSubpix(InputArray, image_points_buf, Size(11, 11)); //对粗提取的角点进行精确

    if(CV_8UC1 == InputArray.type()){
        cv::cvtColor(InputArray,color_view,CV_GRAY2BGR);
    }else {
        color_view = InputArray.clone();
    }

    if(image_points_buf.empty())
        return image_points_buf;

    cv::drawChessboardCorners(color_view, board_size, image_points_buf, true); //用于在图片中标记角点
    cv::circle(color_view,image_points_buf.at(ORGIDX),20,cv::Scalar(24,255,24),5,LINE_AA);
    return image_points_buf;
}

//保存图片
static std::string cv_write_image(const cv::Mat & InputArray, const char* path = "", const char *file_name = "", bool AddDateTime = false)
{


    QString DateTime = QDateTime::currentDateTime().toString("yyyy_MM_dd-hh_mm_ss");
    std::string fileName = "";
    if(AddDateTime)
    {
        if(0==strlen(path)){
            fileName = std::string(file_name) + "_" + DateTime.toStdString();
        }
        else
        fileName = std::string(path) + "/" + std::string(file_name) + "_" + DateTime.toStdString();
    }
    else{
        if(0==strlen(path)){
            fileName = std::string(file_name);
        }else
        fileName = std::string(path) + "/" + std::string(file_name);
    }
     cv::imwrite(fileName+".png", InputArray);
    return fileName+".png";

#ifdef __AFX_H__

    CString Path(path), FileName(file_name), Time_zzz(_Timer::GetIns()->Get_Time_zzz()),fileName;
    if (0 == strlen(path) && 0 == strlen(file_name))
        fileName = Time_zzz;
    else if (0 == strlen(file_name))
        fileName = Path + "/" + Time_zzz;
    else
        fileName = Path + "/" + FileName + "_" + Time_zzz;
    cv::imwrite((cv::String)(CStringA)(fileName+L".bmp"), m);
    return (std::string)(CStringA)(fileName + L".bmp");
#endif
    return "\0";
}

static void HobjToMat(const HalconCpp::HObject &HobjOrg, cv::Mat & dst){
    using namespace HalconCpp;
    using namespace  std;
    using namespace cv;
    try{
    HTuple htCh;
    HString cType;
    HObject Hobj;
    cv::Mat Image;
    ConvertImageType(HobjOrg, &Hobj, "byte");
    CountChannels(Hobj, &htCh);
    Hlong wid = 0;	Hlong hgt = 0;
    if (htCh[0].I() == 1)
    {	HImage hImg(Hobj);
        void *ptr = hImg.GetImagePointer1(&cType, &wid, &hgt);//GetImagePointer1(Hobj, &ptr, &cType, &wid, &hgt);
        int W = wid;		int H = hgt;
        dst = cv::Mat(H,W,CV_8UC1,cv::Scalar(0,0,0));
        unsigned char *pdata = static_cast<unsigned char *>(ptr);
        memcpy(dst.data, pdata, W*H);
    }
    else if (htCh[0].I() == 3)
    {		void *Rptr;		void *Gptr;		void *Bptr;
        HImage hImg(Hobj);
        hImg.GetImagePointer3(&Rptr, &Gptr, &Bptr, &cType, &wid, &hgt);
        int W = wid;		int H = hgt;
        dst.release();
        dst.create(H, W, CV_8UC3);
        vector<cv::Mat> VecM(3);
        VecM[0].create(H, W, CV_8UC1);
        VecM[1].create(H, W, CV_8UC1);
        VecM[2].create(H, W, CV_8UC1);
        unsigned char *R = (unsigned char *)Rptr;
        unsigned char *G = (unsigned char *)Gptr;
        unsigned char *B = (unsigned char *)Bptr;
        memcpy(VecM[2].data, R, W*H);
        memcpy(VecM[1].data, G, W*H);
        memcpy(VecM[0].data, B, W*H);
        cv::merge(VecM, dst);
    }
    }catch(HalconCpp::HException ex)
    {
        throw ex;
    }
    catch(cv::Exception ex)
    {
        throw ex;
    }

}

static void MatToHObj(const cv::Mat & image, HalconCpp::HObject &Hobj)
{
    CV_Assert(!image.empty());
    using namespace cv;
    using namespace HalconCpp;

    int hgt=image.rows;
    int wid=image.cols;
    int i;
    //	CV_8UC3
    if(image.type() == CV_8UC3)
    {
        std::vector<cv::Mat> imgchannel;
        split(image,imgchannel);
        Mat imgB=imgchannel[0];
        Mat imgG=imgchannel[1];
        Mat imgR=imgchannel[2];
        uchar* dataR=new uchar[hgt*wid];
        uchar* dataG=new uchar[hgt*wid];
        uchar* dataB=new uchar[hgt*wid];
        for(i=0;i<hgt;i++)
        {
            memcpy(dataR+wid*i,imgR.data+imgR.step*i,wid);
            memcpy(dataG+wid*i,imgG.data+imgG.step*i,wid);
            memcpy(dataB+wid*i,imgB.data+imgB.step*i,wid);
        }
        GenImage3(&Hobj,"byte",wid,hgt,(Hlong)dataR,(Hlong)dataG,(Hlong)dataB);
        delete []dataR;
        delete []dataG;
        delete []dataB;
        dataR=NULL;
        dataG=NULL;
        dataB=NULL;
    }
    //	CV_8UCU1
    else if(image.type() == CV_8UC1)
    {
        uchar* data=new uchar[hgt*wid];
        for(i=0;i<hgt;i++)
            memcpy(data+wid*i,image.data+image.step*i,wid);
        GenImage1(&Hobj,"byte",wid,hgt,(Hlong)data);
        delete[] data;
        data=NULL;
    }
}


}

#endif // EXCV_H
