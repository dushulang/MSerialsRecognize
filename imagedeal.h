﻿#ifndef IMAGEDEAL_H
#define IMAGEDEAL_H

#include <iostream>
#include "excv.h"
#include <exception>
#include "preference.h"
#include "halcontocv.h"

#define CHOPSTICKNONE   0
#define CHOPSTICKUP     1
#define CHOPSTICKLEFT   2
#define CHOPSTICKDOWN   3
#define CHOPSTICKRIGHT  4
#define TRAINXML        "traindata.xml"

using namespace HalconCpp;
namespace MSerials {
//需要一个变量的存参数，否则
class CommonVar{
    CommonVar(){Init();}
    ~CommonVar(){}
public:
    static CommonVar* GetIns(){static CommonVar c; return &c;}
    cv::Mat m_cameraMatrix;
    cv::Mat m_distCoeffs;
    cv::Mat map1,map2;
    bool isToMap = false,isSetHog = false;

    cv::HOGDescriptor HOGDetect;
    cv::Ptr<cv::ml::SVM> svm;
    std::vector<cv::KeyPoint> keypoints;
    cv::Ptr<cv::ml::KNearest> knn;

    void Init(){
        isSetHog = false;
        if (svm.empty())
        {
            //对svm进行初始化
            svm = cv::ml::SVM::create();
            svm->setType(cv::ml::SVM::C_SVC);
            svm->setKernel(cv::ml::SVM::LINEAR);//注意必须使用线性SVM进行训练，因为HogDescriptor检测函数只支持线性检测！！！
            svm->setTermCriteria(cv::TermCriteria(CV_TERMCRIT_ITER, 1000, FLT_EPSILON));
        }
        CV_Assert(!svm.empty());
    }

};




static void DrawRetangleMult (HObject *ho_ROI, HTuple hv_Num, HTuple hv_RowIn, HTuple hv_ColumnIn,
                              HTuple hv_AngleIn, HTuple hv_Len1In, HTuple hv_Len2In, HTuple *hv_Row, HTuple *hv_Column,
                              HTuple *hv_Len2)
                          {

                            // Local iconic variables
                            HObject  ho_ROITmp;

                            // Local control variables
                            HTuple  hv_Len2Origin, hv_Len2Div, hv_RStart;
                            HTuple  hv_CStart, hv_i, hv_RowTmp, hv_ColTmp;
                            try{
                            hv_Len2Origin = hv_Len2In/hv_Num;
                            hv_Len2Div = (2*hv_Len2In)/hv_Num;

                            //计算除执行的位置
                            GenEmptyRegion(&(*ho_ROI));
                            hv_RStart = (hv_RowIn-(hv_Len2In*(hv_AngleIn.TupleCos())))+((hv_Len2Div/2)*(hv_AngleIn.TupleCos()));
                            hv_CStart = (hv_ColumnIn-(hv_Len2In*(hv_AngleIn.TupleSin())))+((hv_Len2Div/2)*(hv_AngleIn.TupleSin()));
                            (*hv_Row) = HTuple();
                            (*hv_Column) = HTuple();
                            (*hv_Len2) = hv_Len2Div/2;
                            {
                            HTuple end_val9 = hv_Num-1;
                            HTuple step_val9 = 1;
                            for (hv_i=0; hv_i.Continue(end_val9, step_val9); hv_i += step_val9)
                            {
                              hv_RowTmp = hv_RStart+((hv_i*hv_Len2Div)*(hv_AngleIn.TupleCos()));
                              hv_ColTmp = hv_CStart+((hv_i*hv_Len2Div)*(hv_AngleIn.TupleSin()));
                              GenRectangle2(&ho_ROITmp, hv_RowTmp, hv_ColTmp, hv_AngleIn, hv_Len1In, hv_Len2Div/2);
                              if (HDevWindowStack::IsOpen())
                                DispObj(ho_ROITmp, HDevWindowStack::GetActive());
                              (*hv_Row)[hv_i] = hv_RowTmp;
                              (*hv_Column)[hv_i] = hv_ColTmp;
                              Union2((*ho_ROI), ho_ROITmp, &(*ho_ROI));
                            }
                            }
                            }catch(HalconCpp::HException ex)
                            {
                                throw ex;
                            }
                            return;

                          }



static void FindTrackLine (HObject ho_MeasureImage, HTuple hv_RowIn, HTuple hv_ColumnIn,
                           HTuple hv_AngleIn, HTuple hv_Len1In, HTuple hv_Len2In, HTuple hv_Num, HTuple hv_ImageWidth,
                           HTuple hv_ImageHeight, HTuple *hv_OrgX, HTuple *hv_OrgY, HTuple *hv_OrgPhi)
                       {

                         // Local iconic variables
                         HObject  ho_ROI, ho_Cross, ho_Contour, ho_Crosses;

                         // Local control variables
                         HTuple  hv_Row, hv_Column, hv_Len2, hv_RowLen;
                         HTuple  hv_RowLine, hv_ColLine, hv_j, hv_MeasureHandle;
                         HTuple  hv_RowEdgeFirst, hv_ColumnEdgeFirst, hv_AmplitudeFirst;
                         HTuple  hv_IntraDistance, hv_except, hv_RowBegin1, hv_ColBegin1;
                         HTuple  hv_RowEnd1, hv_ColEnd1, hv_Nr1, hv_Nc1, hv_Dist1;

                         try{
                         GenEmptyObj(&ho_ROI);
                         DrawRetangleMult(&ho_ROI, hv_Num, hv_RowIn, hv_ColumnIn, hv_AngleIn, hv_Len1In,
                             hv_Len2In, &hv_Row, &hv_Column, &hv_Len2);
                         hv_RowLen = hv_Row.TupleLength();
                         hv_RowLine = HTuple();
                         hv_ColLine = HTuple();
                         {
                         HTuple end_val5 = hv_RowLen-1;
                         HTuple step_val5 = 1;
                         for (hv_j=0; hv_j.Continue(end_val5, step_val5); hv_j += step_val5)
                         {
                           GenMeasureRectangle2(HTuple(hv_Row[hv_j]), HTuple(hv_Column[hv_j]), hv_AngleIn,
                               hv_Len1In, hv_Len2, hv_ImageWidth, hv_ImageHeight, "bilinear", &hv_MeasureHandle);
                           hv_RowEdgeFirst = HTuple();
                           hv_ColumnEdgeFirst = HTuple();
                           try
                           {
                             MeasurePos(ho_MeasureImage, hv_MeasureHandle, 1.0, 20.0, "positive", "first",
                                 &hv_RowEdgeFirst, &hv_ColumnEdgeFirst, &hv_AmplitudeFirst, &hv_IntraDistance);
                             GenCrossContourXld(&ho_Cross, HTuple(hv_RowEdgeFirst[0]), HTuple(hv_ColumnEdgeFirst[0]),
                                 20, 0.785398);
                             if (HDevWindowStack::IsOpen())
                               DispObj(ho_Cross, HDevWindowStack::GetActive());
                             hv_RowLine = hv_RowLine.TupleConcat(HTuple(hv_RowEdgeFirst[0]));
                             hv_ColLine = hv_ColLine.TupleConcat(HTuple(hv_ColumnEdgeFirst[0]));
                           }
                           // catch (except)
                           catch (HException &HDevExpDefaultException)
                           {
                             HDevExpDefaultException.ToHTuple(&hv_except);
                           }
                         }
                         }
                         if (0 != ((hv_RowLine.TupleLength())<2))
                         {
                         }
                         GenEmptyObj(&ho_Contour);
                         GenEmptyObj(&ho_Crosses);
                         GenCrossContourXld(&ho_Crosses, hv_RowLine, hv_ColLine, 20, 0.785398);
                         GenContourPolygonXld(&ho_Contour, hv_RowLine, hv_ColLine);
                         FitLineContourXld(ho_Contour, "drop", -1, 0, 5, 1.345, &hv_RowBegin1, &hv_ColBegin1,
                             &hv_RowEnd1, &hv_ColEnd1, &hv_Nr1, &hv_Nc1, &hv_Dist1);
                         if (HDevWindowStack::IsOpen())
                           SetColor(HDevWindowStack::GetActive(),"cyan");
                         if (HDevWindowStack::IsOpen())
                           DispObj(ho_Contour, HDevWindowStack::GetActive());
                         if (HDevWindowStack::IsOpen())
                           SetColor(HDevWindowStack::GetActive(),"red");
                         (*hv_OrgX) = (hv_ColEnd1+hv_ColBegin1)/2;
                         (*hv_OrgY) = (hv_RowEnd1+hv_RowBegin1)/2;
                         AngleLx(hv_RowBegin1, hv_ColBegin1, hv_RowEnd1, hv_ColEnd1, &(*hv_OrgPhi));
                        }catch(HalconCpp::HException except){
                            throw except;
                         }
                         return;
                       }

static int CheckUpSide(cv::Mat *InputArray,double Threshold = 64){
    try {
        CV_Assert(nullptr != InputArray && !InputArray->empty());

    } catch (std::exception ex) {
        throw ex;
    }catch(cv::Exception ex){
        throw std::exception(ex.what());
    }
    return CHOPSTICKNONE;
}

//svm检测正反
static std::vector<std::string> check_imgs_path(std::vector<std::string> path)
{
    std::vector<std::string> imgs;
    for(auto & str:path){
        if(!cv::imread(str).empty()){
            imgs.push_back(str);
        }
    }
    return imgs;
}



static bool load_train_data(std::string prj_dir)
{
    std::string path = prj_dir + "/" + TRAINXML;
    try{
    CommonVar::GetIns()->Init();

        if (prj_dir.length() < 1)
        {
            path = TRAINXML;
        }
        CommonVar::GetIns()->svm = cv::Algorithm::load<cv::ml::SVM>(path);
        CV_Assert(!CommonVar::GetIns()->svm.empty());
        return true;
    }
    catch (cv::Exception ex)
    {
        std::cout << "file path is " <<path <<std::endl;
        throw std::exception(("Error In LoadTrainData "+std::string(ex.what())).c_str());

    }

}


static float cv_train_inv(std::vector<std::string> front_imgs, std::vector<std::string> back_imgs, cv::Rect rt, std::string xml_store_path, bool isFlip = false)
{
    //程序可能有bug 看图片是否需要翻转
    using namespace std;
    using namespace cv;
    std::vector<std::string> pos_path = check_imgs_path(front_imgs);
    std::vector<std::string> neg_path = check_imgs_path(back_imgs);
    size_t PosSamNO = pos_path.size(),NegSamNO = neg_path.size();
    CV_Assert(PosSamNO > 0 && NegSamNO >0);
    try {
        HOGDescriptor hog(Size(HOGWIDTH, HOGHEIGHT), Size(16, 16), Size(8, 8), Size(8, 8), 9);//HOG检测器，用来计算HOG描述子的
        int DescriptorDim;
        Mat sampleFeatureMat;
        Mat sampleLabelMat;
        for (int num = 0; num < PosSamNO; num++)
        {
            cv::Mat src,srcimg = imread(pos_path[num]);
            resize(srcimg, src, Size(HOGWIDTH, HOGHEIGHT));
            vector<float> descriptors;//HOG描述子向量
            hog.compute(src, descriptors, Size(8, 8));
            if (0 == num)
            {
                DescriptorDim = descriptors.size();
                sampleFeatureMat = Mat::zeros(PosSamNO + NegSamNO, DescriptorDim, CV_32FC1);
                sampleLabelMat = Mat::zeros(PosSamNO + NegSamNO, 1, CV_32SC1);
            }

            for (int i = 0; i<DescriptorDim; i++)
                sampleFeatureMat.at<float>(num, i) = descriptors[i];//第num个样本的特征向量中的第i个元素
            sampleLabelMat.at<int>(num, 0) = 1;//正样本类别为1，有人
        }
        for (int num = 0; num<NegSamNO; num++)
        {
            Mat src,srcimg = imread(neg_path[num]);//读取图片
            resize(srcimg, src, Size(HOGWIDTH, HOGHEIGHT));
            vector<float> descriptors;//HOG描述子向量
            hog.compute(src, descriptors, Size(8, 8));
            for (int i = 0; i<DescriptorDim; i++)
                sampleFeatureMat.at<float>(num + PosSamNO, i) = descriptors[i];
            sampleLabelMat.at<int>(num + PosSamNO, 0) = -1;
        }
        CommonVar::GetIns()->Init();
        CommonVar::GetIns()->svm->train(sampleFeatureMat, cv::ml::ROW_SAMPLE, sampleLabelMat);
        std::string filePath = xml_store_path + "\\" + TRAINXML;
        if (xml_store_path.length() < 1)
        {
            filePath = TRAINXML;
        }
        CommonVar::GetIns()->svm->save(filePath);
        return 0.0f;
    }
    catch(cv::Exception ex){
        throw std::exception(ex.what());
    }
    catch (...)
    {
        throw std::exception("未知错误");
    }

}


static float HogSvmPredict(const cv::Mat& InputArray)
{
    try {
        using namespace cv;
        cv::Mat src;
        CV_Assert(!InputArray.empty());
        CV_Assert(!CommonVar::GetIns()->svm.empty());

        if(CV_8UC1 == InputArray.type()){
            src = InputArray.clone();
        }
        else {
            cv::cvtColor(InputArray,src,CV_BGR2GRAY);
        }
        int DescriptorDim;
        Mat res,img;
        Mat sampleFeatureMat;
        resize(src, img, Size(HOGWIDTH,  HOGHEIGHT));
        HOGDescriptor hog(Size(HOGWIDTH,  HOGHEIGHT), Size(16, 16), Size(8, 8), Size(8, 8), 9);
        std::vector<float> descriptors;
        hog.compute(img, descriptors, Size(8, 8));
        DescriptorDim = descriptors.size();
        sampleFeatureMat = Mat::zeros(1, DescriptorDim, CV_32FC1);
        for (int i = 0; i < DescriptorDim; i++)
            sampleFeatureMat.at<float>(0, i) = descriptors[i];
        CommonVar::GetIns()->svm->predict(sampleFeatureMat, res);
        return res.at<float>(0, 0);
    }
    catch (cv::Exception ex)
    {
        throw std::exception(ex.what());
    }
}

//train

//输入图像，大概定位的ROI，
//WhichSet  -2表示保存图片， 大于-1 表示要显示效果，用于设定时候
static std::vector<double> CheckLogo(cv::Mat *InputArray, double Row, double Col, double Angle, double Len1, double Len2, std::vector<double> Bias, double CheckLen1, double CheckLen2,int WhichSet = -1){
    using namespace HalconCpp;
    std::vector<double> Result;
    HObject  ho_Image, ho_ROI_0, ho_ImageReduced;
    HObject  ho_ROI_Check, ho_ImageCheck, ho_UnRotationImageToCheck;
    HObject  ho_ImageToCheck;

    // Local control variables
    HTuple  hv_Row, hv_Column, hv_Angle, hv_Len1;
    HTuple  hv_Len2, hv_RowEx, hv_ColumnEx, hv_AngleEx, hv_Len1Ex;
    HTuple  hv_Len2Ex, hv_RowSearch, hv_ColumnSearch, hv_AngleSearch;
    HTuple  hv_Len1Search, hv_Len2Search, hv_Bias, hv_HOGH;
    HTuple  hv_HOGW, hv_Num = 15, hv_i, hv_ImageFiles, hv_Index;
    HTuple  hv_W, hv_H, hv_Width, hv_Height, hv_WindowHandle;
    HTuple  hv_ColumnBase, hv_RowBase, hv_OrgPhi, hv_Distance;
    HTuple  hv_RowStart, hv_ColStart, hv_isOK;

    try {
        CV_Assert(nullptr != InputArray && !InputArray->empty());
        //参数准备
        hv_Row = Row;
        hv_Column = Col;
       // hv_Angle = HTuple(178.457).TupleRad();
        hv_Angle = Angle;//HTuple(Angle).TupleRad();
        hv_Len1 = Len1;
        hv_Len2 = Len2;
        hv_Len1Search = CheckLen1;
        hv_Len2Search = CheckLen2;
        hv_Bias.Clear();
        for(size_t i = 0;i<Bias.size();i++){
            hv_Bias[i] = Bias[i];
        }
         MSerials::Mat2Hobj(*InputArray,ho_Image);
         //参数准备END
         GetImageSize(ho_Image, &hv_Width, &hv_Height);
         if (HDevWindowStack::IsOpen())
           SetDraw(HDevWindowStack::GetActive(),"margin");
         if (HDevWindowStack::IsOpen())
           SetColor(HDevWindowStack::GetActive(),"cyan");
         //先找到背景来确定大致位置
         if (0 != (hv_Angle<0))
         {
           hv_Angle = (2*3.1415926535)+hv_Angle;
         }
         GenRectangle2(&ho_ROI_0, hv_Row, hv_Column, hv_Angle, hv_Len1, hv_Len2);
         ReduceDomain(ho_Image, ho_ROI_0, &ho_ImageReduced);
         if (HDevWindowStack::IsOpen())
           DispObj(ho_ROI_0, HDevWindowStack::GetActive());
         //根据ROI_0找到定位的基准点
         FindTrackLine(ho_Image, hv_Row, hv_Column, hv_Angle, hv_Len1, hv_Len2, hv_Num,
             hv_Width, hv_Height, &hv_ColumnBase, &hv_RowBase, &hv_OrgPhi);

         //相对于大致位置生成20个检测框
         {
         HTuple end_val65 = (hv_Bias.TupleLength())-1;
         HTuple step_val65 = 1;
         for (hv_i=0; hv_i.Continue(end_val65, step_val65); hv_i += step_val65)
         {
           hv_Distance = HTuple(hv_Bias[hv_i]);
           hv_RowStart = hv_RowBase-(HTuple(hv_Bias[hv_i])*(hv_Angle.TupleSin()));
           hv_ColStart = hv_ColumnBase+(HTuple(hv_Bias[hv_i])*(hv_Angle.TupleCos()));
           GenRectangle2(&ho_ROI_Check, hv_RowStart, hv_ColStart, hv_Angle, hv_Len1Search,
               hv_Len2Search);
           ReduceDomain(ho_Image, ho_ROI_Check, &ho_ImageCheck);
           CropDomain(ho_ImageCheck, &ho_UnRotationImageToCheck);
           RotateImage(ho_UnRotationImageToCheck, &ho_ImageToCheck, (-180*hv_Angle)/3.1415926535,
               "constant");
           //opencv 检测,如果正面，isOK置1

           if(-1 == WhichSet){
           cv::Mat ToCheck;
           MSerials::HObject2Mat(ho_ImageToCheck,ToCheck);
           float Res = MSerials::HogSvmPredict(ToCheck);
           char res[256] = {0};
           sprintf_s(res,"%2.1f",Res);
           SetTposition(HDevWindowStack::GetActive(),hv_RowStart,hv_ColStart - CheckLen1);
           if (Res > 0.0)
           {
             Result.push_back(1.0);
             if (HDevWindowStack::IsOpen())
               SetColor(HDevWindowStack::GetActive(),"green");
           }
           else
           {
             Result.push_back(-1.0);
             if (HDevWindowStack::IsOpen())
               SetColor(HDevWindowStack::GetActive(),"red");
           }
            WriteString(HDevWindowStack::GetActive(),res);
           }
           else if(-2 == WhichSet){
               QDateTime time = QDateTime::currentDateTime();
               QString ImageFile = PROJECTNAME + "/" + time.toString("yyyy-MM-dd-HH-mm-ss-zzz-") + QString::number(hv_i[0].I()) + ".jpg";
               HalconCpp::WriteImage(ho_ImageToCheck,"jpg",0,ImageFile.toLocal8Bit().data());
           }
           else if(WhichSet > -1){
               if(hv_i[0].I() == WhichSet)
               {
                    if (HDevWindowStack::IsOpen())
                        SetColor(HDevWindowStack::GetActive(),"blue");
               }
               else {
                   if (HDevWindowStack::IsOpen())
                     SetColor(HDevWindowStack::GetActive(),"green");
               }
           }


           if (HDevWindowStack::IsOpen())
             DispObj(ho_ROI_Check, HDevWindowStack::GetActive());
         }
         // stop(...); only in hdevelop
       }//endtry
    return Result;
    }catch (std::exception ex) {
        throw ex;
    }catch(cv::Exception ex){
        throw std::exception(ex.what());
    }catch(HalconCpp::HException ex){
        throw ex;
        HTuple Error;
        ex.ToHTuple(&Error);
        throw std::exception(Error[0].S());
    }

}


}//EndMserials

#endif // IMAGEDEAL_H
