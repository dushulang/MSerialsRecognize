#include "mcamera.h"
#include "global.h"


#define USE_SDK

std::vector<HTuple> hv_CamHandles;
HTuple hv_AcqHandle,hv_AcqHandle1,hv_AcqHandle2;
size_t  CamNum = 0;
size_t MCamera::CameraNum(){
    return CamNum;
//    return hv_CamHandles.size();
}


void MCamera::GrabImageA3(cv::Mat &ho_Image, int WhichCam,int isFlip ,int Delay, int Manuefactory){
#ifndef USE_SDK
    try {
        using namespace HalconCpp;
        HObject Hobj;
        if(2!=WhichCam){
            //  GrabImage(&Hobj,hv_CamHandles.at(WhichCam));
            GrabImageAsync(&Hobj,hv_CamHandles.at(WhichCam), -1);
        }
        else{

            GrabImage(&Hobj,hv_CamHandles.at(WhichCam));
        }
        cv::Mat Snaped;
        MSerials::HObject2Mat(Hobj,Snaped);
        if(Snaped.empty()){
            ho_Image = cv::Mat(960,1280,CV_8UC1,cv::Scalar(128,128,128));
            return;
        }
        if(isFlip){
            cv::flip(Snaped,ho_Image,-1);
        }
        else
        {
            ho_Image = Snaped.clone();
        }
    } catch (std::out_of_range ex) {
        char ErrorCode[256] = {0};
        sprintf_s(ErrorCode,"超出相机初始化数量 %zd 选择 %d",hv_CamHandles.size(),WhichCam);
        throw std::exception(ErrorCode);

    }catch(HalconCpp::HException ex){
        char ErrorCode[256] = {0};
        sprintf_s(ErrorCode,"抓图超时 %zd 选择 %d",hv_CamHandles.size(),WhichCam);
        throw std::exception(ErrorCode);

        if(2==WhichCam){
            bool InitOK = false;
            for (int i = 0;i<8;i++) {

                try {
                    CloseFramegrabber(hv_CamHandles.at(2));
                }
                catch(std::out_of_range & e){
                    continue;
                }
                catch (HalconCpp::HException & e) {

                }

                try {
                    OpenFramegrabber("GigEVision2", 0, 0, 0, 0, 0, 0, "progressive", -1, "default",
                                     -1, "false", "default", "3", 0, -1, &hv_CamHandles.at(2));
                    SetFramegrabberParam(hv_CamHandles.at(2), "AcquisitionMode", "Continuous");
                    SetFramegrabberParam(hv_CamHandles.at(2), "TriggerMode", "Off");
                    GrabImageStart(hv_CamHandles.at(2), -1);
                    HObject Hobj;
                    GenEmptyObj(&Hobj);
                    GrabImage(&Hobj,hv_CamHandles.at(2));
                    cv::Mat Snaped;
                    MSerials::HObject2Mat(Hobj,Snaped);
                    if(Snaped.empty()){
                        ho_Image = cv::Mat(960,1280,CV_8UC1,cv::Scalar(128,128,128));
                        return;
                    }
                    if(isFlip){
                        cv::flip(Snaped,ho_Image,-1);
                    }
                    else
                    {
                        ho_Image = Snaped.clone();
                    }


                    //hv_CamHandles.push_back(hv_AcqHandle2);
                    InitOK = true;
                } catch (HalconCpp::HException ex) {

                }
                if(InitOK) break;
            }
            if(!InitOK){
                throw std::exception(ErrorCode);
            }else {
                //  throw std::exception("初始化相机3失败！");
            }

        }else{
            throw std::exception(ErrorCode);
        }
    }
    catch(cv::Exception ex){
        throw std::exception(ex.what());

    }
    catch(std::exception ex){
        char ErrorCode[256] = {0};
        sprintf_s(ErrorCode,"%s 错误 %zd 选择 %d ",ex.what(),hv_CamHandles.size(),WhichCam);
        throw std::exception(ErrorCode);

    }
    return;

#endif
    try{
        image_data ImgInfo;
        Snap(ImgInfo.w, ImgInfo.h, &ImgInfo.data, ImgInfo.ch, Manuefactory, WhichCam,Delay);
        if(!isFlip){
            bytes_to_mat(ho_Image,ImgInfo.data,ImgInfo.w,ImgInfo.h,ImgInfo.ch);
        }else{
            cv::Mat F;
            bytes_to_mat(F,ImgInfo.data,ImgInfo.w,ImgInfo.h,ImgInfo.ch);
            cv::flip(F,ho_Image,-1);
        }
    }catch(std::out_of_range ex){
        throw std::exception(ex.what());
    }catch(cv::Exception ex){
        throw std::exception(ex.what());
    }
}



void MCamera::GrabImageA1(cv::Mat &ho_Image, int WhichCam,int isFlip ,int Delay, int Manuefactory){

#ifndef USE_SDK
    try {
        using namespace HalconCpp;
        HObject Hobj;

        if(2!=WhichCam){
            //  GrabImage(&Hobj,hv_CamHandles.at(WhichCam));
            GrabImageAsync(&Hobj,hv_CamHandles.at(WhichCam), -1);
        }
        else{
            GrabImage(&Hobj,hv_CamHandles.at(WhichCam));
        }

        cv::Mat Snaped;
        MSerials::HObject2Mat(Hobj,Snaped);
        if(Snaped.empty()){
            ho_Image = cv::Mat(960,1280,CV_8UC1,cv::Scalar(128,128,128));
            return;
        }
        if(isFlip){
            cv::flip(Snaped,ho_Image,-1);
        }
        else
        {
            ho_Image = Snaped.clone();
        }
    } catch (std::out_of_range ex) {
        char ErrorCode[256] = {0};
        sprintf_s(ErrorCode,"超出相机初始化数量 %zd 选择 %d",hv_CamHandles.size(),WhichCam);
        throw std::exception(ErrorCode);

    }catch(HalconCpp::HException ex){
        char ErrorCode[256] = {0};
        sprintf_s(ErrorCode,"抓图超时 %zd 选择 %d",hv_CamHandles.size(),WhichCam);
        throw std::exception(ErrorCode);
    }
    catch(cv::Exception ex){
        throw std::exception(ex.what());

    }
    catch(std::exception ex){
        char ErrorCode[256] = {0};
        sprintf_s(ErrorCode,"%s 错误 %zd 选择 %d ",ex.what(),hv_CamHandles.size(),WhichCam);
        throw std::exception(ErrorCode);

    }
    return;

#endif
    try{
        image_data ImgInfo;
        Snap(ImgInfo.w, ImgInfo.h, &ImgInfo.data, ImgInfo.ch, Manuefactory, WhichCam,Delay);
        if(!isFlip){
            bytes_to_mat(ho_Image,ImgInfo.data,ImgInfo.w,ImgInfo.h,ImgInfo.ch);
        }else{
            cv::Mat F;
            bytes_to_mat(F,ImgInfo.data,ImgInfo.w,ImgInfo.h,ImgInfo.ch);
            cv::flip(F,ho_Image,-1);
        }
    }catch(std::out_of_range ex){
        throw std::exception(ex.what());
    }catch(cv::Exception ex){
        throw std::exception(ex.what());
    }
}


void MCamera::GrabImageA2(cv::Mat &ho_Image, int WhichCam,int isFlip ,int Delay, int Manuefactory){

#ifndef USE_SDK
    try {
        using namespace HalconCpp;
        HObject Hobj;

        if(2!=WhichCam){
            //  GrabImage(&Hobj,hv_CamHandles.at(WhichCam));
            GrabImageAsync(&Hobj,hv_CamHandles.at(WhichCam), -1);
        }
        else{
            GrabImage(&Hobj,hv_CamHandles.at(WhichCam));
        }

        cv::Mat Snaped;
        MSerials::HObject2Mat(Hobj,Snaped);
        if(Snaped.empty()){
            ho_Image = cv::Mat(960,1280,CV_8UC1,cv::Scalar(128,128,128));
            return;
        }
        if(isFlip){
            cv::flip(Snaped,ho_Image,-1);
        }
        else
        {
            ho_Image = Snaped.clone();
        }
    } catch (std::out_of_range ex) {
        char ErrorCode[256] = {0};
        sprintf_s(ErrorCode,"超出相机初始化数量 %zd 选择 %d",hv_CamHandles.size(),WhichCam);
        throw std::exception(ErrorCode);

    }catch(HalconCpp::HException ex){
        char ErrorCode[256] = {0};
        sprintf_s(ErrorCode,"抓图超时 %zd 选择 %d",hv_CamHandles.size(),WhichCam);
        throw std::exception(ErrorCode);
    }
    catch(cv::Exception ex){
        throw std::exception(ex.what());

    }
    catch(std::exception ex){
        char ErrorCode[256] = {0};
        sprintf_s(ErrorCode,"%s 错误 %zd 选择 %d ",ex.what(),hv_CamHandles.size(),WhichCam);
        throw std::exception(ErrorCode);

    }
    return;

#endif
    try{
        image_data ImgInfo;
        Snap(ImgInfo.w, ImgInfo.h, &ImgInfo.data, ImgInfo.ch, Manuefactory, WhichCam,Delay);
        if(!isFlip){
            bytes_to_mat(ho_Image,ImgInfo.data,ImgInfo.w,ImgInfo.h,ImgInfo.ch);
        }else{
            cv::Mat F;
            bytes_to_mat(F,ImgInfo.data,ImgInfo.w,ImgInfo.h,ImgInfo.ch);
            cv::flip(F,ho_Image,-1);
        }
    }catch(std::out_of_range ex){
        throw std::exception(ex.what());
    }catch(cv::Exception ex){
        throw std::exception(ex.what());
    }
}

int MCamera::CloseCamera(){
    try {
        SetFramegrabberParam(hv_AcqHandle, "TriggerMode", "Off");
    } catch (HalconCpp::HException ex) {

    }
    try {
        SetFramegrabberParam(hv_AcqHandle1, "TriggerMode", "Off");
    } catch (HalconCpp::HException ex) {

    }
    try {
        SetFramegrabberParam(hv_AcqHandle2, "TriggerMode", "Off");
    } catch (HalconCpp::HException ex) {

    }
    try {
        CloseFramegrabber(hv_AcqHandle);
    } catch (HalconCpp::HException ex) {

    }
    try {
        CloseFramegrabber(hv_AcqHandle1);
    } catch (HalconCpp::HException ex) {

    }
    try {
        CloseFramegrabber(hv_AcqHandle2);
    } catch (HalconCpp::HException ex) {

    }
    return 1;
}



void MCamera::GrabImageSDK(cv::Mat &ho_Image, int WhichCam,int isFlip)
{

    try {
        int w =0, h = 0, ch = 1;
        uchar *data = nullptr;
        Snap(w,h,&data,ch);
        if(nullptr == data){
            throw std::exception("相机采图错误");
        }
        if(isFlip){
            ho_Image = cv::Mat(h,w,CV_8UC1,cv::Scalar(0,0,0));
            memcpy(ho_Image .data,data,w*h);
        }else{
            cv::Mat Image = cv::Mat(h,w,CV_8UC1,cv::Scalar(0,0,0));
            memcpy(Image .data,data,w*h);
            cv::flip(Image,ho_Image,-1);
        }


    } catch (std::exception & e) {
        global::GetIns()->History.push_back(std::string("Error is ") + e.what());
        throw e;
    }

}


#if 1

int MCamera::InitCamera(std::string &Info){
    using namespace HalconCpp;
    try{


#ifdef USE_SDK
         CamNum = enum_cameras();
        return  CamNum;
        //    if(camnum < 3)
        //        return false;
        //    return true;
#endif
        int c = 0;
        hv_CamHandles.clear();
        for (int i = 0;i<8;i++) {
            bool InitOK = false;
            try {




                OpenFramegrabber("GigEVision2", 0, 0, 0, 0, 0, 0, "progressive", -1, "default",
                                 -1, "false", "default", "1", 0, -1, &hv_AcqHandle);
                SetFramegrabberParam(hv_AcqHandle, "AcquisitionMode", "Continuous");
                SetFramegrabberParam(hv_AcqHandle, "TriggerMode", "On");
                SetFramegrabberParam(hv_AcqHandle, "TriggerSource", "Line0");
                SetFramegrabberParam(hv_AcqHandle, "grab_timeout", -1);
                GrabImageStart(hv_AcqHandle, -1);
                hv_CamHandles.push_back(hv_AcqHandle);
                c++;
                printf_s("初始化成功1\n");
                Info += "初始化成功1\r\n";
                InitOK  = true;
            } catch (HalconCpp::HException ex) {

            }
            if(InitOK) break;
        }

        for (int i = 0;i<8;i++) {
            bool InitOK = false;
            try {
                OpenFramegrabber("GigEVision2", 0, 0, 0, 0, 0, 0, "progressive", -1, "default",
                                 -1, "false", "default", "2", 0, -1, &hv_AcqHandle1);
                SetFramegrabberParam(hv_AcqHandle1, "AcquisitionMode", "Continuous");
                SetFramegrabberParam(hv_AcqHandle1, "TriggerMode", "On");
                SetFramegrabberParam(hv_AcqHandle1, "TriggerSource", "Line0");
                SetFramegrabberParam(hv_AcqHandle1, "grab_timeout", -1);
                GrabImageStart(hv_AcqHandle1, -1);
                hv_CamHandles.push_back(hv_AcqHandle1);
                c++;
                printf_s("初始化成功2\n");
                Info += "初始化成功2\r\n";
                InitOK = true;
            } catch (HalconCpp::HException ex) {

            }
            if(InitOK) break;
        }

        for (int i = 0;i<8;i++) {
            bool InitOK = false;
            try {
                OpenFramegrabber("GigEVision2", 0, 0, 0, 0, 0, 0, "progressive", -1, "default",
                                 -1, "false", "default", "3", 0, -1, &hv_AcqHandle2);
                SetFramegrabberParam(hv_AcqHandle2, "AcquisitionMode", "Continuous");
                SetFramegrabberParam(hv_AcqHandle2, "TriggerMode", "Off");


                // SetFramegrabberParam(hv_AcqHandle2, "grab_timeout", 5000);
                GrabImageStart(hv_AcqHandle2, -1);
                HObject Hobj;
                GenEmptyObj(&Hobj);
                GrabImage(&Hobj,hv_AcqHandle2);
                hv_CamHandles.push_back(hv_AcqHandle2);
                c++;
                printf_s("初始化成功3\n");
                Info += "初始化成功3\r\n";
                InitOK = true;
            } catch (HalconCpp::HException ex) {

            }
            if(InitOK) break;
        }
        printf_s("初始化成功%d\n",c);
        return c;
        if(c<3)return false;
        return true;



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


#else

int MCamera::InitCamera(std::string &Info){
    using namespace HalconCpp;
    try{


#ifdef USE_SDK
        int camnum = enum_cameras();
        if(camnum < 3)
            return false;
        return true;
#endif
        int c = 0;
        hv_CamHandles.clear();
        for (int i = 0;i<8;i++) {
            bool InitOK = false;
            try {




                OpenFramegrabber("GigEVision2", 0, 0, 0, 0, 0, 0, "progressive", -1, "default",
                                 -1, "false", "default", "1", 0, -1, &hv_AcqHandle);
                SetFramegrabberParam(hv_AcqHandle, "AcquisitionMode", "Continuous");
                SetFramegrabberParam(hv_AcqHandle, "TriggerMode", "On");
                SetFramegrabberParam(hv_AcqHandle, "TriggerSource", "Line0");
                SetFramegrabberParam(hv_AcqHandle, "grab_timeout", -1);
                GrabImageStart(hv_AcqHandle, -1);
                hv_CamHandles.push_back(hv_AcqHandle);
                c++;
                printf_s("初始化成功1\n");
                Info += "初始化成功1\r\n";
                InitOK  = true;
            } catch (HalconCpp::HException ex) {

            }
            if(InitOK) break;
        }

        for (int i = 0;i<8;i++) {
            bool InitOK = false;
            try {
                OpenFramegrabber("GigEVision2", 0, 0, 0, 0, 0, 0, "progressive", -1, "default",
                                 -1, "false", "default", "2", 0, -1, &hv_AcqHandle1);
                SetFramegrabberParam(hv_AcqHandle1, "AcquisitionMode", "Continuous");
                SetFramegrabberParam(hv_AcqHandle1, "TriggerMode", "On");
                SetFramegrabberParam(hv_AcqHandle1, "TriggerSource", "Line0");
                SetFramegrabberParam(hv_AcqHandle1, "grab_timeout", -1);
                GrabImageStart(hv_AcqHandle1, -1);
                hv_CamHandles.push_back(hv_AcqHandle1);
                c++;
                printf_s("初始化成功2\n");
                Info += "初始化成功2\r\n";
                InitOK = true;
            } catch (HalconCpp::HException ex) {

            }
            if(InitOK) break;
        }

        for (int i = 0;i<8;i++) {
            bool InitOK = false;
            try {
                OpenFramegrabber("GigEVision2", 0, 0, 0, 0, 0, 0, "progressive", -1, "default",
                                 -1, "false", "default", "3", 0, -1, &hv_AcqHandle2);
                SetFramegrabberParam(hv_AcqHandle2, "AcquisitionMode", "Continuous");
                SetFramegrabberParam(hv_AcqHandle2, "TriggerMode", "Off");


                // SetFramegrabberParam(hv_AcqHandle2, "grab_timeout", 5000);
                GrabImageStart(hv_AcqHandle2, -1);
                HObject Hobj;
                GenEmptyObj(&Hobj);
                GrabImage(&Hobj,hv_AcqHandle2);
                hv_CamHandles.push_back(hv_AcqHandle2);
                c++;
                printf_s("初始化成功3\n");
                Info += "初始化成功3\r\n";
                InitOK = true;
            } catch (HalconCpp::HException ex) {

            }
            if(InitOK) break;
        }
        printf_s("初始化成功%d\n",c);
        return c;
        if(c<3)return false;
        return true;



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

#endif
