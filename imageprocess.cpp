#include "imageprocess.h"

ImageProcess::ImageProcess()
{
    hv_PI = 3.1415926535;
}



void ImageProcess::GetRelativeRectangle2 (HTuple hv_TrackX, HTuple hv_TrackY, HTuple hv_TrackPhi,
    HTuple hv_ReX, HTuple hv_ReY, HTuple hv_RelativePhi, HTuple hv_AbPhi, HTuple *hv_OutX,
    HTuple *hv_OutY, HTuple *hv_OutPhi)
{
    // Local control variables
     HTuple  hv_Dis, hv_Phi, hv_Exception;

     try
     {
       hv_Dis = ((hv_ReX*hv_ReX)+(hv_ReY*hv_ReY)).TupleSqrt();
       hv_Phi = hv_RelativePhi;
       (*hv_OutX) = hv_Dis*(hv_Phi.TupleCos());
       (*hv_OutY) = hv_Dis*(hv_Phi.TupleSin());
       (*hv_OutX) = hv_TrackX+(*hv_OutX);
       (*hv_OutY) = hv_TrackY-(*hv_OutY);
       (*hv_OutPhi) = hv_TrackPhi+hv_AbPhi;
     }
     // catch (Exception)
     catch (HException &HDevExpDefaultException)
     {
       HDevExpDefaultException.ToHTuple(&hv_Exception);
       throw HException(hv_Exception);
     }
}
// Procedures
// External procedures
void ReAdderPoint (HTuple hv_Rows, HTuple hv_Cols, HTuple *hv_ReRow, HTuple *hv_ReCol)
{

  // Local iconic variables

  // Local control variables

  HTuple  hv_VirLen, hv_Angle, hv_VirCol, hv_VirRow;
  HTuple  hv_IsOverLap;
try{
  //第一个点为点 第二个第三个点位直线
  hv_VirLen = 250;
  AngleLx(HTuple(hv_Rows[1]), HTuple(hv_Cols[1]), HTuple(hv_Rows[2]), HTuple(hv_Cols[2]),
      &hv_Angle);
  hv_VirCol = HTuple(hv_Cols[0])+(hv_VirLen*(hv_Angle.TupleSin()));
  hv_VirRow = HTuple(hv_Rows[0])+(hv_VirLen*(hv_Angle.TupleCos()));
  IntersectionLines(hv_VirRow, hv_VirCol, HTuple(hv_Rows[0]), HTuple(hv_Cols[0]),
      HTuple(hv_Rows[1]), HTuple(hv_Cols[1]), HTuple(hv_Rows[2]), HTuple(hv_Cols[2]),
      &(*hv_ReRow), &(*hv_ReCol), &hv_IsOverLap);
  }
  catch (HException &HDevExpDefaultException)
  {
    HTuple hv_except;
    HDevExpDefaultException.ToHTuple(&hv_except);
    throw HException(hv_except);
  }

}



void ImageProcess::get_points_with_corner (HObject ho_Image, HTuple hv_Rows, HTuple hv_Cols, HTuple hv_Radius,
    HTuple *hv_PLC_X, HTuple *hv_PLC_Y, HTuple hd_disp)
{

    // Local iconic variables
    HObject  ho_ROI_0, ho_TMP_Region, ho_Filled, ho_erosioned;
    HObject  ho_Rect, ho_willGetCirle, ho_binImage, ho_Edges;
    HObject  ho_ContoursSplit, ho_ObjectSelected, ho_ContCircle;

    // Local control variables
    HTuple  hv_circle_centerx, hv_circle_centery;
    HTuple  hv_PLC_PointsX, hv_PLC_PointsY, hv_isInvert, hv_width;
    HTuple  hv_height, hv_i, hv_Number, hv_ContoursMax, hv_CX;
    HTuple  hv_CY, hv_I, hv_Attrib, hv_Row, hv_Column, hv_TRadius;
    HTuple  hv_StartPhi, hv_EndPhi, hv_PointOrder, hv_j, hv_k;
    HTuple  hv_tmp, hv_TRows, hv_TCols, hv_ReRow, hv_ReCol;
    HTuple  hv_Angle, hv_except;

    hv_circle_centerx = HTuple();
    hv_circle_centery = HTuple();
    hv_PLC_PointsX = HTuple();
    hv_PLC_PointsY = HTuple();
    hv_isInvert = INVERTPOINT;
    try
    {


      GetImageSize(ho_Image, &hv_width, &hv_height);
      GenRegionLine(&ho_ROI_0, HTuple(hv_Rows[0]), HTuple(hv_Cols[0]), HTuple(hv_Rows[1]),
          HTuple(hv_Cols[1]));
      GenRegionLine(&ho_TMP_Region, HTuple(hv_Rows[1]), HTuple(hv_Cols[1]), HTuple(hv_Rows[2]),
          HTuple(hv_Cols[2]));
      Union2(ho_ROI_0, ho_TMP_Region, &ho_ROI_0);
      GenRegionLine(&ho_TMP_Region, HTuple(hv_Rows[2]), HTuple(hv_Cols[2]), HTuple(hv_Rows[3]),
          HTuple(hv_Cols[3]));
      Union2(ho_ROI_0, ho_TMP_Region, &ho_ROI_0);
      GenRegionLine(&ho_TMP_Region, HTuple(hv_Rows[3]), HTuple(hv_Cols[3]), HTuple(hv_Rows[0]),
          HTuple(hv_Cols[0]));
      Union2(ho_ROI_0, ho_TMP_Region, &ho_ROI_0);
      FillUp(ho_ROI_0, &ho_Filled);
      OpeningCircle(ho_Filled, &ho_erosioned, hv_Radius);
      if (HDevWindowStack::IsOpen())
        SetColor(HDevWindowStack::GetActive(),"#24EF24");
      if (HDevWindowStack::IsOpen())
        SetDraw(HDevWindowStack::GetActive(),"margin");
      if (HDevWindowStack::IsOpen())
        DispObj(ho_erosioned, HDevWindowStack::GetActive());
      {
      HTuple end_val19 = (hv_Rows.TupleLength())-1;
      HTuple step_val19 = 1;
      for (hv_i=0; hv_i.Continue(end_val19, step_val19); hv_i += step_val19)
      {
        if (HDevWindowStack::IsOpen())
          SetColor(HDevWindowStack::GetActive(),"cyan");
        GenRectangle1(&ho_Rect, HTuple(hv_Rows[hv_i])-(1.2*hv_Radius), HTuple(hv_Cols[hv_i])-(1.2*hv_Radius),
            HTuple(hv_Rows[hv_i])+(1.2*hv_Radius), HTuple(hv_Cols[hv_i])+(1.2*hv_Radius));
        if (HDevWindowStack::IsOpen())
          SetColor(HDevWindowStack::GetActive(),"yellow");
        ReduceDomain(ho_erosioned, ho_Rect, &ho_willGetCirle);
        RegionToBin(ho_willGetCirle, &ho_binImage, 0, 255, hv_width, hv_height);
        EdgesSubPix(ho_binImage, &ho_Edges, "canny", 1.5, 10, 40);
        SegmentContoursXld(ho_Edges, &ho_ContoursSplit, "lines_circles", 5, 4, 3);
        CountObj(ho_ContoursSplit, &hv_Number);
        hv_ContoursMax = 99999999;
        hv_CX = 0;
        hv_CY = 0;
        {
        HTuple end_val31 = hv_Number;
        HTuple step_val31 = 1;
        for (hv_I=1; hv_I.Continue(end_val31, step_val31); hv_I += step_val31)
        {
          SelectObj(ho_ContoursSplit, &ho_ObjectSelected, hv_I);
          GetContourGlobalAttribXld(ho_ObjectSelected, "cont_approx", &hv_Attrib);
          //Fit a circle to the line segment that are arcs of a circle
          if (0 != (hv_Attrib>0))
          {
            FitCircleContourXld(ho_ObjectSelected, "ahuber", -1, 2, 0, 3, 2, &hv_Row,
                &hv_Column, &hv_TRadius, &hv_StartPhi, &hv_EndPhi, &hv_PointOrder);
            GenCircleContourXld(&ho_ContCircle, hv_Row, hv_Column, hv_TRadius, 0, HTuple(360).TupleRad(),
                "positive", 1.0);
            if (0 != (((hv_TRadius-hv_Radius).TupleAbs())<hv_ContoursMax))
            {
              hv_ContoursMax = (hv_TRadius-hv_Radius).TupleAbs();
              hv_CX = hv_Column;
              hv_CY = hv_Row;
            }
          }

        }
        }


        hv_circle_centerx = hv_circle_centerx.TupleConcat(hv_CX);
        hv_circle_centery = hv_circle_centery.TupleConcat(hv_CY);


      }
      }
      {
      HTuple end_val51 = (hv_Rows.TupleLength())-1;
      HTuple step_val51 = 1;
      for (hv_I=0; hv_I.Continue(end_val51, step_val51); hv_I += step_val51)
      {
        hv_i = hv_I;
        hv_j = hv_i+1;
        if (0 != (hv_j==(hv_Rows.TupleLength())))
        {
          hv_j = 0;
        }

        hv_k = hv_i-1;
        if (0 != (hv_k<0))
        {
          hv_k = (hv_Rows.TupleLength())-1;
        }

        if (0 != (0==hv_isInvert))
        {
          hv_tmp = hv_k;
          hv_k = hv_j;
          hv_j = hv_tmp;
        }

        //切点1
        hv_TRows.Clear();
        hv_TRows.Append(HTuple(hv_circle_centery[hv_i]));
        hv_TRows.Append(HTuple(hv_Rows[hv_i]));
        hv_TRows.Append(HTuple(hv_Rows[hv_j]));
        hv_TCols.Clear();
        hv_TCols.Append(HTuple(hv_circle_centerx[hv_i]));
        hv_TCols.Append(HTuple(hv_Cols[hv_i]));
        hv_TCols.Append(HTuple(hv_Cols[hv_j]));
        if (HDevWindowStack::IsOpen())
          SetColor(HDevWindowStack::GetActive(),"yellow");
        ReAdderPoint(hv_TRows, hv_TCols, &hv_ReRow, &hv_ReCol);
        if (HDevWindowStack::IsOpen())
          SetColor(HDevWindowStack::GetActive(),"#FF0000");
        GenCircleContourXld(&ho_ContCircle, hv_ReRow, hv_ReCol, 10, 0, HTuple(360).TupleRad(),
            "positive", 3.0);
        if (HDevWindowStack::IsOpen())
          DispObj(ho_ContCircle, HDevWindowStack::GetActive());
        hv_PLC_PointsX = hv_PLC_PointsX.TupleConcat(hv_ReCol);
        hv_PLC_PointsY = hv_PLC_PointsY.TupleConcat(hv_ReRow);

        //插补圆弧点
        AngleLx(HTuple(hv_circle_centery[hv_i]), HTuple(hv_circle_centerx[hv_i]), HTuple(hv_Rows[hv_i]),
            HTuple(hv_Cols[hv_i]), &hv_Angle);
        hv_ReRow = HTuple(hv_circle_centery[hv_i])-(hv_Radius*(hv_Angle.TupleSin()));
        hv_ReCol = HTuple(hv_circle_centerx[hv_i])+(hv_Radius*(hv_Angle.TupleCos()));
        if (HDevWindowStack::IsOpen())
          SetColor(HDevWindowStack::GetActive(),"#FFFF00");
        GenCircleContourXld(&ho_ContCircle, hv_ReRow, hv_ReCol, 10, 0, HTuple(360).TupleRad(),
            "positive", 3.0);
        if (HDevWindowStack::IsOpen())
          DispObj(ho_ContCircle, HDevWindowStack::GetActive());
        hv_PLC_PointsX = hv_PLC_PointsX.TupleConcat(hv_ReCol);
        hv_PLC_PointsY = hv_PLC_PointsY.TupleConcat(hv_ReRow);

        //切点2
        hv_TRows.Clear();
        hv_TRows.Append(HTuple(hv_circle_centery[hv_i]));
        hv_TRows.Append(HTuple(hv_Rows[hv_i]));
        hv_TRows.Append(HTuple(hv_Rows[hv_k]));
        hv_TCols.Clear();
        hv_TCols.Append(HTuple(hv_circle_centerx[hv_i]));
        hv_TCols.Append(HTuple(hv_Cols[hv_i]));
        hv_TCols.Append(HTuple(hv_Cols[hv_k]));
        if (HDevWindowStack::IsOpen())
          SetColor(HDevWindowStack::GetActive(),"green");
        ReAdderPoint(hv_TRows, hv_TCols, &hv_ReRow, &hv_ReCol);
        if (HDevWindowStack::IsOpen())
          SetColor(HDevWindowStack::GetActive(),"#0000FF");
        GenCircleContourXld(&ho_ContCircle, hv_ReRow, hv_ReCol, 10, 0, HTuple(360).TupleRad(),
            "positive", 3.0);
        if (HDevWindowStack::IsOpen())
          DispObj(ho_ContCircle, HDevWindowStack::GetActive());
        hv_PLC_PointsX = hv_PLC_PointsX.TupleConcat(hv_ReCol);
        hv_PLC_PointsY = hv_PLC_PointsY.TupleConcat(hv_ReRow);
        //stop ()
      }
      }

      (*hv_PLC_X) = hv_PLC_PointsX;
      (*hv_PLC_Y) = hv_PLC_PointsY;

    }
    // catch (except)
    catch (HException &HDevExpDefaultException)
    {
      HDevExpDefaultException.ToHTuple(&hv_except);
      throw HException(hv_except);
    }



}


void ImageProcess::RenewPara(double Row[] ,double Col[], double AbAngle[], double ReAngle[], double Len1[],double Len2[],\
                             int Sample ,
                             int ReaddPoint ,
                             double Radius ,
                             double l1offset ,
                             double l2offset ,
                             double l3offset ,
                             double l4offset ,
                             int LinesPos
                             ){

    int LinesPosEx = LinesPos;
    if(LinesPosEx > 7){
        LinesPosEx = 7;
    }
    hv_LinesPos = LinesPosEx;
    hv_Num = Sample;
    hv_ReAddPoint = ReaddPoint;
    hv_Radius = Radius;
    hv_LinesOffset = HTuple();
    hv_RowIns = HTuple();//[hv_Idx] = hv_OutY;
    hv_ColIns = HTuple();//[hv_Idx] = hv_OutX;
    hv_AngleReIns = HTuple();//[hv_Idx] = hv_OutRePhi;
    hv_AngleAbIns= HTuple();//[hv_Idx] = hv_OutAbPhi;
    hv_Len1Ins= HTuple();//[hv_Idx] = hv_Len1In;
    hv_Len2Ins= HTuple();//[hv_Idx] = hv_Len2In;
    hv_PI = 3.1415926535;
    hv_LinesOffset = hv_LinesOffset.TupleConcat(HTuple(l1offset));
    hv_LinesOffset = hv_LinesOffset.TupleConcat(HTuple(l2offset));
    hv_LinesOffset = hv_LinesOffset.TupleConcat(HTuple(l3offset));
    hv_LinesOffset = hv_LinesOffset.TupleConcat(HTuple(l4offset));
    for(int i = 0;i <= LinesPosEx;i++){
        hv_RowIns = hv_RowIns.TupleConcat(HTuple(Row[i]));
        hv_ColIns = hv_ColIns.TupleConcat(HTuple(Col[i]));
        hv_AngleAbIns = hv_AngleAbIns.TupleConcat(HTuple(AbAngle[i]));
        hv_AngleReIns = hv_AngleReIns.TupleConcat(HTuple(ReAngle[i]));
        hv_Len1Ins = hv_Len1Ins.TupleConcat(HTuple(Len1[i]));
        hv_Len2Ins = hv_Len2Ins.TupleConcat(HTuple(Len2[i]));
    }

}


PositionInfo ImageProcess::action(HObject ho_MeasureImage,HTuple WindowHandle){
try{

    //参数更新
    hv_WindowHandle = WindowHandle;
    Excv::h_disp_obj(ho_MeasureImage, WindowHandle);
    PositionInfo Pos,OriginPos;
    GetImageSize(ho_MeasureImage,&hv_Width,&hv_Height);
    hv_RowBegins = HTuple();//hv_RowBegins.TupleConcat(hv_RowBegin);
    hv_ColBegins = HTuple();//hv_ColBegins.TupleConcat(hv_ColBegin);
    hv_RowEnds = HTuple();//hv_RowEnds.TupleConcat(hv_RowEnd);
    hv_ColEnds = HTuple();//hv_ColEnds.TupleConcat(hv_ColEnd);

    HTuple hv_Idx = 0;
    if (HDevWindowStack::IsOpen())
      SetDraw(HDevWindowStack::GetActive(),"margin");
    if (HDevWindowStack::IsOpen())
      SetColor(HDevWindowStack::GetActive(),"yellow");
    //模拟读取参数
    hv_RowIn = HTuple(hv_RowIns[hv_Idx]);
    hv_ColumnIn = HTuple(hv_ColIns[hv_Idx]);
    hv_AngleIn = HTuple(hv_AngleAbIns[hv_Idx]);
    hv_Len1In = HTuple(hv_Len1Ins[hv_Idx]);
    hv_Len2In = HTuple(hv_Len2Ins[hv_Idx]);
    FindTrackLine(ho_MeasureImage, hv_RowIn, hv_ColumnIn, hv_AngleIn, hv_Len1In, hv_Len2In,
        hv_Num, hv_Width, hv_Height, &hv_OrgX, &hv_OrgY, &hv_OrgPhi);
    DispArrow(hv_WindowHandle, hv_OrgY, hv_OrgX, hv_OrgY-(120*(hv_OrgPhi.TupleSin())),
        hv_OrgX+(120*(hv_OrgPhi.TupleCos())), 2);



    for(int i=1;i<=hv_LinesPos.I();i++){
        //重新计算出需要计算的IN,在运行时候进行
        if (HDevWindowStack::IsOpen())
          SetColor(HDevWindowStack::GetActive(),"cyan");
        hv_Idx = i;
        //读取参数
        hv_ReY = HTuple(hv_RowIns[hv_Idx]);
        hv_ReX = HTuple(hv_ColIns[hv_Idx]);
        hv_AngleAbSearch = HTuple(hv_AngleAbIns[hv_Idx]);
        hv_AngleReSearch = HTuple(hv_AngleReIns[hv_Idx]);
        hv_Len1In = hv_Len1Ins[hv_Idx];
        hv_Len2In = hv_Len2Ins[hv_Idx];
        hv_OX = hv_OrgX;//((const HTuple&)hv_ColIns)[0];
        hv_OY = hv_OrgY;//((const HTuple&)hv_RowIns)[0];
        hv_OPhi = hv_OrgPhi;//((const HTuple&)hv_AngleAbIns)[0];
        GetRelativeRectangle2(hv_OX, hv_OY, hv_OPhi, hv_ReX, hv_ReY, hv_AngleReSearch,
            hv_AngleAbSearch, &hv_OutCol, &hv_OutRow, &hv_OutPhi);
        FindSearchLine(ho_MeasureImage, hv_OutRow, hv_OutCol, hv_OutPhi, hv_Len1In, hv_Len2In,
            hv_Num, hv_Width, hv_Height, &hv_RowBegin, &hv_ColBegin, &hv_RowEnd, &hv_ColEnd);

        if (HDevWindowStack::IsOpen())
          SetColor(HDevWindowStack::GetActive(),"#EE12FF");

        HTuple LineOffset = hv_LinesOffset[i-1];
        if(abs(LineOffset.D()) > 0){
            HTuple LineAngle = 0;
            AngleLx(hv_RowBegin, hv_ColBegin, hv_RowEnd, hv_ColEnd,&LineAngle);
            hv_ColBegin = LineOffset*(LineAngle.TupleSin()) + hv_ColBegin;
            hv_RowBegin = LineOffset*(LineAngle.TupleCos()) + hv_RowBegin;
            hv_ColEnd = LineOffset*(LineAngle.TupleSin()) + hv_ColEnd;
            hv_RowEnd = LineOffset*(LineAngle.TupleCos()) + hv_RowEnd;
        }


        hv_RowBegins = hv_RowBegins.TupleConcat(hv_RowBegin);
        hv_ColBegins = hv_ColBegins.TupleConcat(hv_ColBegin);
        hv_RowEnds = hv_RowEnds.TupleConcat(hv_RowEnd);
        hv_ColEnds = hv_ColEnds.TupleConcat(hv_ColEnd);
        DispLine(hv_WindowHandle, hv_RowBegin, hv_ColBegin, hv_RowEnd, hv_ColEnd);
        //需要进行偏
    }

    HTuple hv_XPoints = HTuple();
    HTuple hv_YPoints = HTuple();
    if (HDevWindowStack::IsOpen())
      SetColor(HDevWindowStack::GetActive(),"green");
    hv_LineNum = hv_RowBegins.TupleLength();
    {
    HTuple end_val238 = hv_LineNum-1;
    HTuple step_val238 = 1;
    for (hv_i=0; hv_i.Continue(end_val238, step_val238); hv_i += step_val238)
    {
      hv_j = (hv_i+1)%hv_LineNum;
      IntersectionLines(HTuple(hv_RowBegins[hv_i]), HTuple(hv_ColBegins[hv_i]), HTuple(hv_RowEnds[hv_i]),
          HTuple(hv_ColEnds[hv_i]), HTuple(hv_RowBegins[hv_j]), HTuple(hv_ColBegins[hv_j]),
          HTuple(hv_RowEnds[hv_j]), HTuple(hv_ColEnds[hv_j]), &hv_Row, &hv_Col, &hv_IsOverLap);
      hv_XPoints = hv_XPoints.TupleConcat(hv_Col);
      hv_YPoints = hv_YPoints.TupleConcat(hv_Row);
    }
    }

    HTuple hv_ReAddX = HTuple();
    HTuple hv_ReAddY = HTuple();
    HTuple hv_ReYPoint;
    HTuple hv_ReXPoint;
    //重定位点
    switch (hv_ReAddPoint.I())
    {
    case 0:
      break;
    case 1:
      hv_ReAddX = HTuple();
      hv_ReAddY = HTuple();

      hv_ReAddX = hv_ReAddX.TupleConcat(HTuple(hv_XPoints[3]));
      hv_ReAddY = hv_ReAddY.TupleConcat(HTuple(hv_YPoints[3]));

      hv_ReAddX = hv_ReAddX.TupleConcat(HTuple(hv_XPoints[0]));
      hv_ReAddY = hv_ReAddY.TupleConcat(HTuple(hv_YPoints[0]));

      hv_ReAddX = hv_ReAddX.TupleConcat(HTuple(hv_XPoints[1]));
      hv_ReAddY = hv_ReAddY.TupleConcat(HTuple(hv_YPoints[1]));

      ReAdderPoint(hv_ReAddY, hv_ReAddX, &hv_ReYPoint, &hv_ReXPoint);
      hv_YPoints[0] = hv_ReYPoint;
      hv_XPoints[0] = hv_ReXPoint;

      hv_ReAddX = HTuple();
      hv_ReAddY = HTuple();

      hv_ReAddX = hv_ReAddX.TupleConcat(HTuple(hv_XPoints[2]));
      hv_ReAddY = hv_ReAddY.TupleConcat(HTuple(hv_YPoints[2]));

      hv_ReAddX = hv_ReAddX.TupleConcat(HTuple(hv_XPoints[0]));
      hv_ReAddY = hv_ReAddY.TupleConcat(HTuple(hv_YPoints[0]));

      hv_ReAddX = hv_ReAddX.TupleConcat(HTuple(hv_XPoints[1]));
      hv_ReAddY = hv_ReAddY.TupleConcat(HTuple(hv_YPoints[1]));

      ReAdderPoint(hv_ReAddY, hv_ReAddX, &hv_ReYPoint, &hv_ReXPoint);
      hv_YPoints[1] = hv_ReYPoint;
      hv_XPoints[1] = hv_ReXPoint;

      break;

    case 2:
      hv_ReAddX = HTuple();
      hv_ReAddY = HTuple();

      hv_ReAddX = hv_ReAddX.TupleConcat(HTuple(hv_XPoints[0]));
      hv_ReAddY = hv_ReAddY.TupleConcat(HTuple(hv_YPoints[0]));

      hv_ReAddX = hv_ReAddX.TupleConcat(HTuple(hv_XPoints[1]));
      hv_ReAddY = hv_ReAddY.TupleConcat(HTuple(hv_YPoints[1]));

      hv_ReAddX = hv_ReAddX.TupleConcat(HTuple(hv_XPoints[2]));
      hv_ReAddY = hv_ReAddY.TupleConcat(HTuple(hv_YPoints[2]));

      ReAdderPoint(hv_ReAddY, hv_ReAddX, &hv_ReYPoint, &hv_ReXPoint);
      hv_YPoints[1] = hv_ReYPoint;
      hv_XPoints[1] = hv_ReXPoint;

      hv_ReAddX = HTuple();
      hv_ReAddY = HTuple();

      hv_ReAddX = hv_ReAddX.TupleConcat(HTuple(hv_XPoints[3]));
      hv_ReAddY = hv_ReAddY.TupleConcat(HTuple(hv_YPoints[3]));

      hv_ReAddX = hv_ReAddX.TupleConcat(HTuple(hv_XPoints[1]));
      hv_ReAddY = hv_ReAddY.TupleConcat(HTuple(hv_YPoints[1]));

      hv_ReAddX = hv_ReAddX.TupleConcat(HTuple(hv_XPoints[2]));
      hv_ReAddY = hv_ReAddY.TupleConcat(HTuple(hv_YPoints[2]));

      ReAdderPoint(hv_ReAddY, hv_ReAddX, &hv_ReYPoint, &hv_ReXPoint);
      hv_YPoints[2] = hv_ReYPoint;
      hv_XPoints[2] = hv_ReXPoint;
      break;
    case 3:

      hv_ReAddX = HTuple();
      hv_ReAddY = HTuple();

      hv_ReAddX = hv_ReAddX.TupleConcat(HTuple(hv_XPoints[1]));
      hv_ReAddY = hv_ReAddY.TupleConcat(HTuple(hv_YPoints[1]));

      hv_ReAddX = hv_ReAddX.TupleConcat(HTuple(hv_XPoints[2]));
      hv_ReAddY = hv_ReAddY.TupleConcat(HTuple(hv_YPoints[2]));

      hv_ReAddX = hv_ReAddX.TupleConcat(HTuple(hv_XPoints[3]));
      hv_ReAddY = hv_ReAddY.TupleConcat(HTuple(hv_YPoints[3]));

      ReAdderPoint(hv_ReAddY, hv_ReAddX, &hv_ReYPoint, &hv_ReXPoint);
      hv_YPoints[2] = hv_ReYPoint;
      hv_XPoints[2] = hv_ReXPoint;

      hv_ReAddX = HTuple();
      hv_ReAddY = HTuple();

      hv_ReAddX = hv_ReAddX.TupleConcat(HTuple(hv_XPoints[0]));
      hv_ReAddY = hv_ReAddY.TupleConcat(HTuple(hv_YPoints[0]));

      hv_ReAddX = hv_ReAddX.TupleConcat(HTuple(hv_XPoints[2]));
      hv_ReAddY = hv_ReAddY.TupleConcat(HTuple(hv_YPoints[2]));

      hv_ReAddX = hv_ReAddX.TupleConcat(HTuple(hv_XPoints[3]));
      hv_ReAddY = hv_ReAddY.TupleConcat(HTuple(hv_YPoints[3]));

      ReAdderPoint(hv_ReAddY, hv_ReAddX, &hv_ReYPoint, &hv_ReXPoint);
      hv_YPoints[3] = hv_ReYPoint;
      hv_XPoints[3] = hv_ReXPoint;
      break;
    case 4:
      hv_ReAddX = HTuple();
      hv_ReAddY = HTuple();

      hv_ReAddX = hv_ReAddX.TupleConcat(HTuple(hv_XPoints[1]));
      hv_ReAddY = hv_ReAddY.TupleConcat(HTuple(hv_YPoints[1]));

      hv_ReAddX = hv_ReAddX.TupleConcat(HTuple(hv_XPoints[0]));
      hv_ReAddY = hv_ReAddY.TupleConcat(HTuple(hv_YPoints[0]));

      hv_ReAddX = hv_ReAddX.TupleConcat(HTuple(hv_XPoints[3]));
      hv_ReAddY = hv_ReAddY.TupleConcat(HTuple(hv_YPoints[3]));

      ReAdderPoint(hv_ReAddY, hv_ReAddX, &hv_ReYPoint, &hv_ReXPoint);
      hv_YPoints[0] = hv_ReYPoint;
      hv_XPoints[0] = hv_ReXPoint;
      hv_ReAddX = HTuple();
      hv_ReAddY = HTuple();
      hv_ReAddX = hv_ReAddX.TupleConcat(HTuple(hv_XPoints[2]));
      hv_ReAddY = hv_ReAddY.TupleConcat(HTuple(hv_YPoints[2]));
      hv_ReAddX = hv_ReAddX.TupleConcat(HTuple(hv_XPoints[0]));
      hv_ReAddY = hv_ReAddY.TupleConcat(HTuple(hv_YPoints[0]));
      hv_ReAddX = hv_ReAddX.TupleConcat(HTuple(hv_XPoints[3]));
      hv_ReAddY = hv_ReAddY.TupleConcat(HTuple(hv_YPoints[3]));
      ReAdderPoint(hv_ReAddY, hv_ReAddX, &hv_ReYPoint, &hv_ReXPoint);
      hv_YPoints[3] = hv_ReYPoint;
      hv_XPoints[3] = hv_ReXPoint;
      break;
    }
    if (HDevWindowStack::IsOpen())
      SetColor(HDevWindowStack::GetActive(),"green");
    hv_LineNum = hv_XPoints.TupleLength();
    {
    HTuple end_val413 = hv_LineNum-1;
    HTuple step_val413 = 1;
    for (hv_i=0; hv_i.Continue(end_val413, step_val413); hv_i += step_val413)
    {
      hv_Row = HTuple(hv_YPoints[hv_i]);
      hv_Col = HTuple(hv_XPoints[hv_i]);

      HTuple hv_j = hv_i+1;
      GenCircleContourXld(&ho_Circle, hv_Row, hv_Col, 20, 0, 6.28, "positive", 2.0);
      SetTposition(hv_WindowHandle, hv_Row, hv_Col);

      Pos.Points.push_back(cv::Point2d(hv_Col[0].D(),hv_Row[0].D()));
      OriginPos.Points.push_back(cv::Point2d(hv_Col[0].D(),hv_Row[0].D()));

      HTuple ColDisp = hv_Col*RATIOX- ORIGINX;
      HTuple RowDisp = hv_Row*RATIOY-ORIGINY;
      hv_str = (((("点"+(hv_j.TupleString(".1")))+" x:")+(ColDisp.TupleString(".5")))+"y:")+(RowDisp.TupleString(".5"));
      WriteString(hv_WindowHandle, hv_str);
      if (HDevWindowStack::IsOpen())
        DispObj(ho_Circle, HDevWindowStack::GetActive());
    }
    }



    int TextGap = 90;
    int TextCol = 1000;


    if(0 != (hv_Radius < HTuple(15.0))){
        PositionInfo TPos;
        TPos.Points.clear();
        while(Pos.Points.size() < (hv_LinesPos.I()-1)){
            Pos.Points.push_back(cv::Point2d(0,0));
        }

        //如果没有半径，则进行数据填充
        for(int k = 0; k < hv_LinesPos.I();k++){
            for(int f = 0;f<3;f++)
            {
                double x = Pos.Points.at(k).x;
                x = x*RATIOX- ORIGINX;
                double y = Pos.Points.at(k).y;
                y = y*RATIOY-ORIGINY;
                TPos.Points.push_back(cv::Point2d(x,y));
            }
        }

        if (HDevWindowStack::IsOpen())
          SetColor(HDevWindowStack::GetActive(),"#F61034");
        Pos.Points = TPos.Points;

        HTuple Angle;
        AngleLx(TPos.Points.at(0).y,TPos.Points.at(0).x,TPos.Points.at(1).y,TPos.Points.at(1).x,&Angle);
        Pos.Angles.at(0) = Angle[0].D();

        int Gap = 0;
        int pp=0;
        char Msg[256] = {0};
        for(int s= 0;s<6;s++){
        sprintf_s(Msg,"点%dX:%4.2f Y:%4.2f",1+pp,Pos.Points.at(pp).x,Pos.Points.at(pp).y);
        if (HDevWindowStack::IsOpen())
            SetTposition(HDevWindowStack::GetActive(), Gap, 0);
            WriteString(HDevWindowStack::GetActive(),Msg);
        pp++;
        sprintf_s(Msg,"点%dX:%4.2f Y:%4.2f",1+pp,Pos.Points.at(pp).x,Pos.Points.at(pp).y);
        if (HDevWindowStack::IsOpen())
            SetTposition(HDevWindowStack::GetActive(), Gap, TextCol);
            WriteString(HDevWindowStack::GetActive(),Msg);

        pp++;
        Gap += TextGap;


    }
    sprintf_s(Msg,"旋转角:%4.2f",Pos.Angles.at(0));
    if (HDevWindowStack::IsOpen())
        SetTposition(HDevWindowStack::GetActive(), Gap, 0);
        WriteString(HDevWindowStack::GetActive(),Msg);
        return Pos;
    }
    else{

      //  WriteString(HDevWindowStack::GetActive(),"proc1");
        HTuple PLC_X,PLC_Y;
        HTuple hv_rows = HTuple();
        HTuple hv_cols = HTuple();
        while(Pos.Points.size() < (hv_LinesPos.I())){
            OriginPos.Points.push_back(cv::Point2d(0,0));
        }



        //WriteString(HDevWindowStack::GetActive(),"proc2");

        for(int i = 0;i <(hv_LinesPos.I());i++){
            hv_rows = hv_rows.TupleConcat(HTuple(OriginPos.Points.at(i).y));
            hv_cols = hv_cols.TupleConcat(HTuple(OriginPos.Points.at(i).x));
        }


        //WriteString(HDevWindowStack::GetActive(),"proc3");
        get_points_with_corner(ho_MeasureImage,hv_rows,hv_cols,hv_Radius,&PLC_X,&PLC_Y,WindowHandle);
        Pos.Points.clear();
        {
        HTuple end_val88 = (PLC_X.TupleLength())-1;
        HTuple step_val88 = 1;
        HTuple hv_i;
        for (hv_i=0; hv_i.Continue(end_val88, step_val88); hv_i += step_val88)
        {
          double x = HTuple(PLC_X[hv_i]);
          x = x*RATIOX- ORIGINX;
          double y = HTuple(PLC_Y[hv_i]);
          y = y*RATIOY-ORIGINY;
          Pos.Points.push_back(cv::Point2d(x,y));
        }
        }

        HTuple Angle;
        AngleLx(hv_rows[0],hv_cols[0],hv_rows[1],hv_cols[1],&Angle);
        Pos.Angles.at(0) = Angle[0].D();
        if (HDevWindowStack::IsOpen())
          SetColor(HDevWindowStack::GetActive(),"#F61034");

        int Gap = 0;
        int pp=0;
        char Msg[256] = {0};
    for(int s= 0;s<6;s++){
        sprintf_s(Msg,"点%dX:%4.2f Y:%4.2f",1+pp,Pos.Points.at(pp).x,Pos.Points.at(pp).y);
        if (HDevWindowStack::IsOpen())
            SetTposition(HDevWindowStack::GetActive(), Gap, 0);
            WriteString(HDevWindowStack::GetActive(),Msg);
        pp++;

        sprintf_s(Msg,"点%dX:%4.2f Y:%4.2f",1+pp,Pos.Points.at(pp).x,Pos.Points.at(pp).y);
        if (HDevWindowStack::IsOpen())
            SetTposition(HDevWindowStack::GetActive(), Gap, TextCol);
            WriteString(HDevWindowStack::GetActive(),Msg);

        pp++;
        Gap += TextGap;
    }

    sprintf_s(Msg,"旋转角:%4.2f",Pos.Angles.at(0));
    if (HDevWindowStack::IsOpen())
        SetTposition(HDevWindowStack::GetActive(), Gap, 0);
        WriteString(HDevWindowStack::GetActive(),Msg);
       return Pos;

    }
    }
    catch (HException &HDevExpDefaultException)
    {
      HTuple hv_except;
      HDevExpDefaultException.ToHTuple(&hv_except);
      throw HException(hv_except);
    }
    catch(std::out_of_range ex)
    {
        HTuple ErrorInfo = "Error out of range to find points";
        throw HException(ErrorInfo);

    }

}



// Procedures
// External procedures
void ImageProcess::FindTrackLine (HObject ho_MeasureImage, HTuple hv_RowIn, HTuple hv_ColumnIn,
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

  return;
}


// Chapter: 2D Metrology
// Short Description: 根据原点的角度和xy坐标的位置得到一个相对坐标的角度和X，Y
void ImageProcess::GenRelativeRectangle2 (HTuple hv_OrgX, HTuple hv_OrgY, HTuple hv_OrgPhi, HTuple hv_ReXIn,
    HTuple hv_ReYIn, HTuple hv_RePhiIn, HTuple *hv_OutX, HTuple *hv_OutY, HTuple *hv_OutRelativePhi,
    HTuple *hv_OutAbPhi)
{
    HTuple  hv_PI, hv_Ag, hv_Exception;

    try
    {
      //OutX := sqrt(((ReXIn - OrgX)*(ReXIn - OrgX)) + (ReYIn - OrgY)*(ReYIn - OrgY))
      (*hv_OutX) = hv_ReXIn-hv_OrgX;
      (*hv_OutY) = hv_ReYIn-hv_OrgY;
      hv_PI = 3.1415926535;
      hv_Ag = hv_RePhiIn;
      if (0 != (hv_RePhiIn<0))
      {
        hv_Ag = (2*hv_PI)+hv_RePhiIn;
      }
      (*hv_OutAbPhi) = hv_Ag-hv_OrgPhi;
      AngleLx(hv_OrgY, hv_OrgX, hv_ReYIn, hv_ReXIn, &(*hv_OutRelativePhi));
      if (0 != ((*hv_OutRelativePhi)<0))
      {
        hv_Ag = (2*hv_PI)+(*hv_OutRelativePhi);
      }
    }
    // catch (Exception)
    catch (HException &HDevExpDefaultException)
    {
      HDevExpDefaultException.ToHTuple(&hv_Exception);
      throw HException(hv_Exception);
    }}

void ImageProcess::DrawRetangleMult (HObject *ho_ROI, HTuple hv_Num, HTuple hv_RowIn, HTuple hv_ColumnIn,
    HTuple hv_AngleIn, HTuple hv_Len1In, HTuple hv_Len2In, HTuple *hv_Row, HTuple *hv_Column,
    HTuple *hv_Len2)
{

  // Local iconic variables
  HObject  ho_ROITmp;

  // Local control variables
  HTuple  hv_Len2Origin, hv_Len2Div, hv_RStart;
  HTuple  hv_CStart, hv_i, hv_RowTmp, hv_ColTmp;

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
  return;

}

void ImageProcess::FindSearchLine (HObject ho_MeasureImage, HTuple hv_RowIn, HTuple hv_ColumnIn,
    HTuple hv_AngleIn, HTuple hv_Len1In, HTuple hv_Len2In, HTuple hv_Num, HTuple hv_Width,
    HTuple hv_Height, HTuple *hv_RowBegin, HTuple *hv_ColBegin, HTuple *hv_RowEnd,
    HTuple *hv_ColEnd)
{

  // Local iconic variables
  HObject  ho_ROI, ho_Cross, ho_Contour, ho_Crosses;

  // Local control variables
  HTuple  hv_Row, hv_Column, hv_Len2, hv_RowLen;
  HTuple  hv_RowLine, hv_ColLine, hv_j, hv_MeasureHandle;
  HTuple  hv_RowEdgeFirst, hv_ColumnEdgeFirst, hv_AmplitudeFirst;
  HTuple  hv_IntraDistance, hv_except, hv_Nr1, hv_Nc1, hv_Dist1;
  HTuple  hv_Exception;

  try
  {
    GenEmptyObj(&ho_ROI);
    DrawRetangleMult(&ho_ROI, hv_Num, hv_RowIn, hv_ColumnIn, hv_AngleIn, hv_Len1In,
        hv_Len2In, &hv_Row, &hv_Column, &hv_Len2);
    if (HDevWindowStack::IsOpen())
      DispObj(ho_ROI, HDevWindowStack::GetActive());
    hv_RowLen = hv_Row.TupleLength();
    hv_RowLine = HTuple();
    hv_ColLine = HTuple();
    {
    HTuple end_val7 = hv_RowLen-1;
    HTuple step_val7 = 1;
    for (hv_j=0; hv_j.Continue(end_val7, step_val7); hv_j += step_val7)
    {
      GenMeasureRectangle2(HTuple(hv_Row[hv_j]), HTuple(hv_Column[hv_j]), hv_AngleIn,
          hv_Len1In, hv_Len2, hv_Width, hv_Height, "bilinear", &hv_MeasureHandle);
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
    FitLineContourXld(ho_Contour, "drop", -1, 0, 5, 1.345, &(*hv_RowBegin), &(*hv_ColBegin),
        &(*hv_RowEnd), &(*hv_ColEnd), &hv_Nr1, &hv_Nc1, &hv_Dist1);
    if (HDevWindowStack::IsOpen())
      SetColor(HDevWindowStack::GetActive(),"cyan");
    if (HDevWindowStack::IsOpen())
      DispObj(ho_Contour, HDevWindowStack::GetActive());
  }
  // catch (Exception)
  catch (HException &HDevExpDefaultException)
  {
    HDevExpDefaultException.ToHTuple(&hv_Exception);
    throw HException(hv_Exception);
  }
  return;
}

