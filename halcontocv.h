#ifndef HALCONTOCV_H
#define HALCONTOCV_H

#include "halconcpp.h"
#include "opencv.hpp"

namespace MSerials{

static void h_disp_obj(HalconCpp::HObject &obj, HalconCpp::HTuple disp_hd) {
    using namespace HalconCpp;
    try
    {
        HTuple w, h;
        GetImageSize(obj, &w, &h);
        HDevWindowStack::Push(disp_hd);
        HDevWindowStack::SetActive(disp_hd);
        SetPart(disp_hd, NULL, NULL, h, w);
        DispObj(obj, disp_hd);
    }
    catch (HException &except)
    {
        throw std::exception("无图像或者显示窗口");
        HTuple ExceptionMessage;
        except.ToHTuple(&ExceptionMessage);
        SetTposition(disp_hd, 0, 1);
        WriteString(disp_hd, ExceptionMessage);
    }
}

//halcon 转opencv
static void HObject2Mat(HalconCpp::HObject& shobj,cv::Mat &dst)
{
    try {
        using namespace HalconCpp;
        HTuple htCh ;
        HTuple cType;
        HObject Hobj;
        ConvertImageType(shobj, &Hobj, "byte");
        CountChannels(Hobj, &htCh);
        HTuple wid;
        HTuple hgt;
        int W, H;

        if (htCh[0].I() == 1)
        {
            HTuple ptr;
            GetImagePointer1(Hobj, &ptr, &cType, &wid, &hgt);
            W = (Hlong)wid[0].L();
            H = (Hlong)hgt[0].L();
            dst = cv::Mat(H, W, CV_8UC1, cv::Scalar(0, 0, 0));
            uchar* pdata = (uchar*)ptr[0].L();
            memcpy(dst.data, pdata, W * H);
        }
        else if (htCh[0].I() == 3)
        {

            HTuple ptrR, ptrG, ptrB;
            GetImagePointer3(Hobj,&ptrR,&ptrG,&ptrB,&cType,&wid,&hgt);
            W=(Hlong)wid;
            H=(Hlong)hgt;
            dst.create(H,W,CV_8UC3);
            std::vector<cv::Mat> vecM(3);
            vecM[2].create(H,W,CV_8UC1);
            vecM[1].create(H,W,CV_8UC1);
            vecM[0].create(H,W,CV_8UC1);
            uchar* pr=(uchar*)ptrR[0].L();
            uchar* pg=(uchar*)ptrG[0].L();
            uchar* pb=(uchar*)ptrB[0].L();
            memcpy(vecM[2].data,pr,H*W);
            memcpy(vecM[1].data,pg,H*W);
            memcpy(vecM[0].data,pb,H*W);
            merge(vecM,dst);
        }
    }
    catch (cv::Exception ex)
    {
        throw std::exception(ex.what());
    }
    catch (HalconCpp::HException ex)
    {
        HalconCpp::HTuple Ex = "Error";
        ex.ToHTuple(&Ex);
        std::string Error = Ex[0].C();
        throw(Error.c_str());
        //printf_s("error in hobj to mat %s\n",Ex[0].C());
    }catch(std::exception ex){
        throw ex;
    }
}

//opencv 转halcon
static void Mat2Hobj(cv::Mat &InputArray, HalconCpp::HObject &Hobj) {
    using namespace HalconCpp;
    if (3 == InputArray.channels())
    {
        cv::Mat pImageRed, pImageGreen, pImageBlue;
        std::vector<cv::Mat> sbgr(3);
        cv::split(InputArray, sbgr);

        int length = InputArray.rows * InputArray.cols;
        uchar *dataBlue = new uchar[length];
        uchar *dataGreen = new uchar[length];
        uchar *dataRed = new uchar[length];

        int height = InputArray.rows;
        int width = InputArray.cols;
        for (int row = 0; row < height; row++)
        {
            uchar* ptr = InputArray.ptr<uchar>(row);
            for (int col = 0; col < width; col++)
            {
                dataBlue[row * width + col] = ptr[3 * col];
                dataGreen[row * width + col] = ptr[3 * col + 1];
                dataRed[row * width + col] = ptr[3 * col + 2];
            }
        }

        HalconCpp::GenImage3(&Hobj, "byte", width, height, (Hlong)(dataRed), (Hlong)(dataGreen), (Hlong)(dataBlue));
        delete[] dataRed;
        delete[] dataGreen;
        delete[] dataBlue;
    }
    else if (1 == InputArray.channels())
    {
        int height = InputArray.rows;
        int width = InputArray.cols;
        uchar *dataGray = new uchar[width * height];
        memcpy(dataGray, InputArray.data, width * height);
        GenImage1(&Hobj, "byte", width, height, (Hlong)(dataGray));
        delete[] dataGray;
    }
}

};
#endif // HALCONTOCV_H
