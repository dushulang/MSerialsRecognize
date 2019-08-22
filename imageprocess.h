#ifndef IMAGEPROCESS_H
#define IMAGEPROCESS_H


#include "global.h"
#include "excv.h"
#include <exception>
#include "preference.h"

using namespace HalconCpp;




class ImageProcess
{
public:

    static ImageProcess* GetIns(){
        static ImageProcess lg;
        return &lg;
    }
  //  PositionInfo ImageProcess::action(HObject ho_Image_MeasureImage,HTuple WindowHandle,int LinesPos,\
   //      double Row[] ,double Col[], double Angle[], double Len1[],double Len2[],\
    //     int ReAdderPointSel = 0,
    //谁、     double Radius = 15);


    // Local iconic variables
    HObject  ho_Image1, ho_ROI_0, ho_Region;
    HObject  ho_Circle;

    // Local control variables
    HTuple  hv_threshold_var, hv_Width, hv_Height;
    HTuple  hv_WindowHandle, hv_i, hv_Num,hv_LinesPos, hv_FindCorner, hv_RowIns,hv_ReAddPoint = 4, hv_Radius = 0;
    HTuple  hv_ColIns, hv_AngleIns, hv_AngleRelativeIns, hv_Len1Ins;
    HTuple  hv_AngleReIns,hv_AngleAbIns;
    HTuple  hv_Len2Ins, hv_RowBegins, hv_ColBegins, hv_RowEnds;
    HTuple  hv_ColEnds, hv_PI, hv_RowIn, hv_ColumnIn, hv_AngleIn;
    HTuple  hv_Len1In, hv_Len2In, hv_OrgX, hv_OrgY, hv_OrgPhi;
    HTuple  hv_Idx, hv_ReYIn, hv_ReXIn, hv_RePhiIn, hv_OutX;
    HTuple  hv_OutY, hv_OutRePhi, hv_OutAbPhi, hv_ReY, hv_ReX;
    HTuple  hv_AngleAbSearch, hv_AngleReSearch, hv_OX, hv_OY;
    HTuple  hv_OPhi, hv_OutCol, hv_OutRow, hv_OutPhi, hv_RowBegin;
    HTuple  hv_ColBegin, hv_RowEnd, hv_ColEnd, hv_LineNum, hv_j;
    HTuple  hv_Row, hv_Col, hv_IsOverLap, hv_str;
    HTuple  hv_LinesOffset;



    void get_points_with_corner (HObject ho_Image, HTuple hv_Rows, HTuple hv_Cols, HTuple hv_Radius,
        HTuple *hv_PLC_X, HTuple *hv_PLC_Y, HTuple hd_disp);

    void RenewPara(double Row[] ,double Col[], double AbAngle[], double ReAngle[], double Len1[],double Len2[],\
                                 int Sample = 15,
                                 int ReaddPoint = 4,
                                 double Radius = 0.0,
                                 double l1offset = 0.0,
                                 double l2offset = 0.0,
                                 double l3offset = 0.0,
                                 double l4offset = 0.0,
                                 int LinesPos = 4
                                 );

    PositionInfo action(HObject ho_MeasureImage,HTuple WindowHandle);
    // Procedure declarations
    // External procedures
    void FindTrackLine (HObject ho_MeasureImage, HTuple hv_RowIn, HTuple hv_ColumnIn,
        HTuple hv_AngleIn, HTuple hv_Len1In, HTuple hv_Len2In, HTuple hv_Num, HTuple hv_ImageWidth,
        HTuple hv_ImageHeight, HTuple *hv_OrgX, HTuple *hv_OrgY, HTuple *hv_OrgPhi);

    void GetRelativeRectangle2 (HTuple hv_TrackX, HTuple hv_TrackY, HTuple hv_TrackPhi,
        HTuple hv_ReX, HTuple hv_ReY, HTuple hv_RelativePhi, HTuple hv_AbPhi, HTuple *hv_OutX,
        HTuple *hv_OutY, HTuple *hv_OutPhi);

    // Short Description: 根据原点的角度和xy坐标的位置得到一个相对坐标的角度和X，Y
    void GenRelativeRectangle2 (HTuple hv_OrgX, HTuple hv_OrgY, HTuple hv_OrgPhi, HTuple hv_ReXIn,
        HTuple hv_ReYIn, HTuple hv_RePhiIn, HTuple *hv_OutX, HTuple *hv_OutY, HTuple *hv_OutRelativePhi,
        HTuple *hv_OutAbPhi);
    void DrawRetangleMult (HObject *ho_ROI, HTuple hv_Num, HTuple hv_RowIn, HTuple hv_ColumnIn,
        HTuple hv_AngleIn, HTuple hv_Len1In, HTuple hv_Len2In, HTuple *hv_Row, HTuple *hv_Column,
        HTuple *hv_Len2);
    void FindSearchLine (HObject ho_MeasureImage, HTuple hv_RowIn, HTuple hv_ColumnIn,
        HTuple hv_AngleIn, HTuple hv_Len1In, HTuple hv_Len2In, HTuple hv_Num, HTuple hv_Width,
        HTuple hv_Height, HTuple *hv_RowBegin, HTuple *hv_ColBegin, HTuple *hv_RowEnd,
        HTuple *hv_ColEnd);


    //void Refresh(int LinesPos = 4, int Smaple = 5);
    ImageProcess();
};

#endif // IMAGEPROCESS_H
