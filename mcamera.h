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

//#define USE_SDK



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

    size_t CameraNum();
    int InitCamera(std::string &Info);
    int CloseCamera();
    static unsigned int __stdcall InitCamera_(void *pLvoid)
    {
   //     Sleep(500);
        try{
            std::string CamInfo = "\r\n";
            int isOK = MCamera::GetIns()->InitCamera(CamInfo);
            if(isOK >= 3)
            {
                char InFo[256]= {0};
                sprintf_s(InFo,"相机初始化成功! 个数为%d %s",isOK,CamInfo.c_str());
                global::GetIns()->History.push_back(InFo);
            }
            else
            {
                char InFo[256]= {0};
                sprintf_s(InFo,"相机初始化失败! 个数为%d %s",isOK,CamInfo.c_str());
                global::GetIns()->History.push_back(InFo);
            }

            }catch(HalconCpp::HException ex)
        {

        }
        return 0;

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

        try{
        CV_Assert(nullptr !=bytes && w >0 && h >0);
        cv::Mat m;
        if(ch == 3)
            m = cv::Mat(h, w, CV_8UC3, cv::Scalar(0, 0, 0));
        else
            m = cv::Mat(h, w, CV_8UC1, cv::Scalar(0));
        m.data = bytes;
        mat = m.clone();
        }catch(cv::Exception ex){
            throw std::exception(ex.what());
        }
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


  //  void GrabImageA(HalconCpp::HObject &ho_Image, int WhichCam,int isFlip = 0,int Delay =0, int Manuefactory = 0){

    void GrabImageA3(cv::Mat &ho_Image, int WhichCam,int isFlip = 0,int Delay =0, int Manuefactory = 0);
    void GrabImageA1(cv::Mat &ho_Image, int WhichCam,int isFlip = 0,int Delay =0, int Manuefactory = 0);
    void GrabImageA2(cv::Mat &ho_Image, int WhichCam,int isFlip = 0,int Delay =0, int Manuefactory = 0);

};

#endif // MCAMERA_H
