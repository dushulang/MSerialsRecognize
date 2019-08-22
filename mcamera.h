#ifndef MCAMERA_H
#define MCAMERA_H

#include "HalconCpp.h"
#include "process.h"
#include "global.h"
#include "MSerialsCam.h"
using namespace HalconCpp;


struct image_data
{
    int w = 0;
    int h  = 0;
    int ch = 1;
    uchar * data = nullptr;
};

#define USE_SDK

class MCamera
{
    MCamera(){
       // Init();
    }
    ~MCamera(){

    }
public:
    static MCamera *GetIns(){static MCamera g; return &g;}
    void Init()
    {
        (HANDLE)_beginthreadex(NULL, 0,InitCamera_, this, 0, NULL);
    }

    static unsigned int __stdcall InitCamera_(void *pLvoid)
    {
        Sleep(500);
        try{
            bool isOK = MCamera::GetIns()->InitCamera();
            if(isOK)
            {
                global::GetIns()->History.push_back("相机初始化成功！");
            }
            else
            {
                global::GetIns()->History.push_back("相机初始化失败！");
            }

            }catch(HalconCpp::HException ex)
        {

        }
        return 0;

    }

    HTuple hv_AcqHandle;
    bool InitCamera(){
        using namespace HalconCpp;
        try{

#ifdef USE_SDK
        int camnum = enum_cameras();
        if(camnum < 1)
            return false;
        return true;
#endif
        printf_s("init start\n");
        OpenFramegrabber("GigEVision2", 0, 0, 0, 0, 0, 0, "progressive", -1, "default",
            -1, "false", "default", "default", 0, -1, &hv_AcqHandle);
        printf_s("init camera success\n");

        SetFramegrabberParam(hv_AcqHandle, "AcquisitionMode", "Continuous");
        SetFramegrabberParam(hv_AcqHandle, "TriggerMode", "Off");
        SetFramegrabberParam(hv_AcqHandle, "TriggerSource", "Software");

        GrabImageStart(hv_AcqHandle, -1);


        printf_s("init over\n");
        return true;
        }catch(HalconCpp::HException ex)
        {
            throw ex;
        }
    }

    static void split_color(int width, int height, int ch, uchar* src, uchar** dst)
        {
            if (ch > 2) return;

            *dst = (uchar*)malloc(width*height * sizeof(char));
            uchar *s = src; uchar *d = *dst;
            //openmp
    //#pragma omp parallel
            for (int h = 0; h < height; h++)
            {
                for (int w = 0; w < width; w++)
                {
                    d[w] = s[w * 3 + ch];
                }
                d += width;
                s += 3 * width;
            }
        }

    inline void bytes_to_mat(cv::Mat & mat, uchar * bytes, int w, int h, int ch)
    {
        CV_Assert(NULL!=bytes && w >0 && h >0);
        cv::Mat m;
        if(ch == 3)
            m = cv::Mat(h, w, CV_8UC3, cv::Scalar(0, 0, 0));
        else
            m = cv::Mat(h, w, CV_8UC1, cv::Scalar(0));
        m.data = bytes;
        mat = m.clone();
    }

    static  bool bytes_to_hobj(HalconCpp::HObject & hobj,uchar * bytes, int w, int h, int ch)
    {
        if (NULL == bytes || w == 0 || h == 0)		return false;
        if(3==ch)
        {
            uchar * r = nullptr, *g = nullptr, *b = nullptr;
            split_color(w, h, 0, bytes, &b);
            split_color(w, h, 1, bytes, &g);
            split_color(w, h, 2, bytes, &r);
            HalconCpp::GenImage3(&hobj, "byte", w, h, (Hlong)r, (Hlong)g, (Hlong)b);
            free(r);
            free(g);
            free(b);

        }
        else
        {
            uchar * data = bytes;
            HalconCpp::GenImage1(&hobj, "byte", w, h, (Hlong)data);
        }
        return true;
    }

    void GrabImageA(cv::Mat &ho_Image, int WhichCam, int Delay =0, int Manuefactory = 0){
        try{
        image_data ImgInfo;
        Snap(ImgInfo.w, ImgInfo.h, &ImgInfo.data, ImgInfo.ch, Manuefactory, WhichCam,Delay);
        bytes_to_mat(ho_Image,ImgInfo.data,ImgInfo.w,ImgInfo.h,ImgInfo.ch);
        }catch(std::out_of_range ex){
            throw std::exception(ex.what());
        }catch(cv::Exception ex){
            throw std::exception(ex.what());
        }
    }

    void GrabImageA(HalconCpp::HObject &ho_Image, HTuple MaxDelay = 2000){
        try{
            using namespace HalconCpp;
#ifdef USE_SDK
            image_data ImgInfo;
            Snap(ImgInfo.w, ImgInfo.h, &ImgInfo.data, ImgInfo.ch, 0, 0,0);
            bytes_to_hobj(ho_Image,ImgInfo.data,ImgInfo.w,ImgInfo.h,ImgInfo.ch);
            printf_s("w %d ,h %d, ch %d\n",ImgInfo.w,ImgInfo.h,ImgInfo.ch);
#else
            static std::mutex mtx;
            std::lock_guard<std::mutex> lck(mtx);
        GrabImageAsync(&ho_Image,hv_AcqHandle,MaxDelay);
#endif

        }catch(HalconCpp::HException except)
        {
            throw except;
        }
    }
};

#endif // MCAMERA_H
