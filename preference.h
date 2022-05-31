#ifndef PREFERENCE_H
#define PREFERENCE_H

#include <QSettings>
#include <iostream>
#include <string>
#include <QString>
#include <vector>
#include <unordered_map>

#define HOGRATIO        8
#define HOGHEIGHT       256
#define HOGWIDTH        32
#define BLOCKH          64
#define BLOCKW          8
#define CELLH           8
#define CELLW           8

#ifdef BEST
#define HOGHEIGHT       256
#define HOGWIDTH        32
#define BLOCKH          64
#define BLOCKW          8
#define CELLH           3
#define CELLW           3
#endif


#define PROJECTNAME     Preference::GetIns()->sys->GetProjectNameByIndex(Preference::GetIns()->sys->Project_Sel).toLocal8Bit().trimmed()
#define MAX_VOL         8
#define ISDRAW          Preference::GetIns()->prj->isDrawing
#define SAMPLENUM       15
#define CHECKNUM        Preference::GetIns()->prj->SampleNum
#define ROW             Preference::GetIns()->prj->Row
#define COLUMN          Preference::GetIns()->prj->Column
//#define ANGLE           Preference::GetIns()->prj->Angle
#define RECTVBIAS       Preference::GetIns()->prj->Angle
#define REANGLE         Preference::GetIns()->prj->Angle
#define ABANGLE         Preference::GetIns()->prj->AbAngle
#define LEN1            Preference::GetIns()->prj->Len1
#define LEN2            Preference::GetIns()->prj->Len2
#define LINEOFFSET      Preference::GetIns()->prj->Offset
#define LINEOFFSET1     Preference::GetIns()->prj->Offset1
#define SAVEPRJPARA     Preference::GetIns()->prj->WriteSettings()
#define RATIOX          Preference::GetIns()->prj->RatioX
#define RATIOY          Preference::GetIns()->prj->RatioY
#define CABLI           "Cabli"
#define ESTIMATE        Preference::GetIns()->prj->EstimateHighLow
#define CORNERR         Preference::GetIns()->prj->CornerR
#define LINESPOS        4
#define INVERTPOINT     Preference::GetIns()->prj->InvertPoint
#define FLIPIMAGE       Preference::GetIns()->prj->Flip

#define ORIGINX         Preference::GetIns()->prj->OriginX
#define ORIGINY         Preference::GetIns()->prj->OriginY
#define CAMANGLE        Preference::GetIns()->prj->CamAngle
#define READDERSEL      Preference::GetIns()->prj->Readdersel

#define THRESHOLD       Preference::GetIns()->prj->threshold_var
#define CHOPSTICKAREA   Preference::GetIns()->prj->chopstick_area
#define MEANVAR         Preference::GetIns()->prj->mean_var
#define IMAGEFACTOR     Preference::GetIns()->prj->factor
#define ERSIONRADIUS    Preference::GetIns()->prj->erison_radius
#define THRESHOLDBIAS   Preference::GetIns()->prj->threshold_bias

#define DOUBLEFILTERD   Preference::GetIns()->prj->d
#define DOUBLEFILTERSC  Preference::GetIns()->prj->sigmaColor
#define DOUBLEFILTERSS  Preference::GetIns()->prj->sigmaSpace
#define DOUBLEFILTERBS  Preference::GetIns()->prj->blockSize

#define ISSAVEIMAGE     Preference::GetIns()->prj->InvertPoint

#define PARA_SET	(1)
#define PARA_PRJ	(1<<1)
#define PARA_IMAGE	(1<<2)
#define PARA_IO		(1<<3)

#define PARA_ALL	(PARA_SET | PARA_PRJ | PARA_IMAGE |PARA_IO)   //ALL -1

#define STR_SET     "PARA_SET"
#define STR_PRJ     "PARA_PRJ"
#define STR_IMAGE   "IMAGE_SETTING"
#define STR_IO      "PARA_IO"



#define Baund1200       "1200"
#define Baund2400       "2400"
#define Baund4800       "4800"
#define Baund9600       "9600"
#define Baund19200      "19200"
#define Baund38400      "38400"
#define Baund57600      "57600"
#define Baund115200     "115200"
#define DATA7           "7"
#define DATA8           "8"
#define PARNONE         "None"
#define PARODD          "Odd"
#define PAREVEN         "Even"
#define STOP1           "1"
#define STOP1D5         "1.5"
#define STOP2           "2"


class Ini
{
private:
    void Init() {
        //InitData();
        int SEL = PARA_ALL;
        if (0 == strcmp("sys", m_class_name.c_str()))
        {
            SEL = PARA_SET;
        }
        else {
            SEL = PARA_PRJ | PARA_IO ;
        }
        WriteSettings(SEL);
    }

    std::string m_class_name;
    QSettings *settings;
    QString str_set,str_prj,str_img,str_io;

public:
    Ini(std::string class_name = "Preference")
        : settings(nullptr)
        ,m_class_name (class_name)
    {
        str_set = STR_SET; str_set += "/";
        str_prj = STR_PRJ; str_prj += "/";
        str_img = STR_IMAGE; str_img += "/";
        str_io = STR_IO; str_io += "/";
        //InitData();
    }

    ~Ini() { delete settings; }

    const char* Class_Name() { return m_class_name.c_str(); }

    void SetFilePos(QString str) {
        if (nullptr != settings) delete settings;
        settings = new QSettings(str, QSettings::IniFormat);
        ReadSettings(PARA_ALL);
    }


public:
    void WriteSettings(int SEL = PARA_ALL)
    {

        if (SEL & PARA_SET)
        {
            //表示文件存在
            settings->setValue(str_set + "sign", 1);
            settings->setValue(str_prj + "PRJ_SEL", Project_Sel);
            settings->setValue(str_prj + "PRJ", Project_Name);
            settings->setValue(str_prj + "PRJS", Project_NameS);
        }

        if (SEL & PARA_PRJ)
        {

            settings->setValue(str_prj + "ACC", Acc);
            settings->setValue(str_prj + "SPD", Spd);
            settings->setValue(str_prj + "POS", Pos);
            settings->setValue(str_prj + "SAMNUM", SampleNum);
            for(int i = 0;i<MAX_VOL ;i++){
                settings->setValue(str_prj + "ROW" + QString::number(i), Row[i]);
                settings->setValue(str_prj + "COL" + QString::number(i), Column[i]);
                settings->setValue(str_prj + "ANGLE" + QString::number(i), Angle[i]);
                settings->setValue(str_prj + "ABANGLE" + QString::number(i), AbAngle[i]);
                settings->setValue(str_prj + "LEN1" + QString::number(i), Len1[i]);
                settings->setValue(str_prj + "LEN2" + QString::number(i), Len2[i]);
                settings->setValue(str_prj + "FLIP" + QString::number(i), Flip[i]);

                settings->setValue(str_prj + "THRESHOLD" + QString::number(i), threshold_var[i]);
                settings->setValue(str_prj + "CHOPAREA" + QString::number(i), chopstick_area[i]);
                settings->setValue(str_prj + "MEANVAR" + QString::number(i), mean_var[i]);
                settings->setValue(str_prj + "FACTOR"  + QString::number(i), factor[i]);
                settings->setValue(str_prj + "ERIRADIUS" + QString::number(i), erison_radius[i]);
                settings->setValue(str_prj + "THRESHOLDBIAS" + QString::number(i), threshold_bias[i]);
                settings->setValue(str_prj + "DBD" + QString::number(i), d[i]);
                settings->setValue(str_prj + "DBSC" + QString::number(i), sigmaColor[i]);
                settings->setValue(str_prj + "DBSS" + QString::number(i), sigmaSpace[i]);
                settings->setValue(str_prj + "BLOCKSIZE" + QString::number(i), blockSize[i]);
            }

            for(int i = 0;i<4*MAX_VOL ;i++){
                settings->setValue(str_prj + "OFS" + QString::number(i), Offset[i]);
                settings->setValue(str_prj + "OF1S" + QString::number(i), Offset1[i]);
            }


            settings->setValue(str_prj + "RATIOX", RatioX);
            settings->setValue(str_prj + "RATIOY", RatioY);

            settings->setValue(str_prj + "ORGX", OriginX);
            settings->setValue(str_prj + "ORGY", OriginY);
            settings->setValue(str_prj + "ANGLE", CamAngle);
            settings->setValue(str_prj + "READDER", Readdersel);
            settings->setValue(str_prj + "CRR", CornerR);
            settings->setValue(str_prj + "INVP", InvertPoint);

        }

        if (SEL & PARA_IMAGE)
        {

        }


        if (SEL & PARA_IO)
        {

        }
    }



    void ReadSettings(int SEL = PARA_ALL)
    {
        if (SEL & PARA_SET)
        {
            //表示文件存在
            int Sign = settings->value(str_set+"sign", 0).toInt();
            Project_Sel = settings->value(str_prj + "PRJ_SEL",1).toInt();
            Project_Name = settings->value(str_prj + "PRJ","A3").toString();
            Project_NameS = settings->value(str_prj + "PRJS","A3").toString();
            if (1 != Sign) {
                Init();
                return;
            }
        }


        if (SEL & PARA_PRJ)
        {

            Acc = settings->value(str_prj + "ACC","2000").toString();
            Spd = settings->value(str_prj + "SPD","2000").toString();
            Pos = settings->value(str_prj + "POS","2000").toString();
            SampleNum = settings->value(str_prj + "SAMNUM",12).toInt();

            for(int i = 0;i<MAX_VOL ;i++){
                Row[i] = settings->value(str_prj + "ROW" + QString::number(i),0.0).toDouble();
                Column[i] = settings->value(str_prj + "COL" + QString::number(i),0.0).toDouble();
                Angle[i] = settings->value(str_prj + "ANGLE" + QString::number(i),0.0).toDouble();
                AbAngle[i] = settings->value(str_prj + "ABANGLE" + QString::number(i),0.0).toDouble();
                Len1[i] = settings->value(str_prj + "LEN1" + QString::number(i),8.0).toDouble();
                Len2[i] = settings->value(str_prj + "LEN2" + QString::number(i),8.0).toDouble();
                Flip[i] = settings->value(str_prj + "FLIP" + QString::number(i),0).toInt();


                threshold_var[i] = settings->value(str_prj + "THRESHOLD" + QString::number(i),85.0).toDouble();
                chopstick_area[i] = settings->value(str_prj + "CHOPAREA" + QString::number(i),50000.0).toDouble();
                mean_var[i] = settings->value(str_prj + "MEANVAR" + QString::number(i),5.0).toDouble();
                factor[i] = settings->value(str_prj + "FACTOR" + QString::number(i),0.7).toDouble();
                erison_radius[i] = settings->value(str_prj + "ERIRADIUS" + QString::number(i),12.0).toDouble();
                threshold_bias[i] = settings->value(str_prj + "THRESHOLDBIAS" + QString::number(i),-1.0).toDouble();

                d[i] = settings->value(str_prj + "DBD" + QString::number(i),9).toDouble();
                sigmaColor[i] = settings->value(str_prj + "DBSC" + QString::number(i),20).toDouble();
                sigmaSpace[i] = settings->value(str_prj + "DBSS"  + QString::number(i),5).toDouble();
                blockSize[i] = settings->value(str_prj + "BLOCKSIZE" + QString::number(i),75).toDouble();

            }
            for(int i = 0;i<4*MAX_VOL ;i++){
                Offset[i] = settings->value(str_prj + "OFS" + QString::number(i),2.0).toDouble();
                Offset1[i] = settings->value(str_prj + "OF1S" + QString::number(i),2.0).toDouble();
            }

            RatioX = settings->value(str_prj + "RATIOX",0.3).toDouble();
            RatioY = settings->value(str_prj + "RATIOY",0.3).toDouble();
            OriginX = settings->value(str_prj + "ORGX",0.0).toDouble();
            OriginY = settings->value(str_prj + "ORGY",0.0).toDouble();
            CamAngle = settings->value(str_prj + "ANGLE",0.0).toDouble();
            Readdersel = settings->value(str_prj + "READDER",0.0).toInt();
            CornerR= settings->value(str_prj + "CRR",0.0).toDouble();
            InvertPoint = settings->value(str_prj + "INVP",0.0).toDouble();
        }


        if (SEL & PARA_IMAGE)
        {

        }


        if (SEL & PARA_IO)
        {

        }
    }


    std::vector<QString> ProjectNames(){
        std::vector<QString> vecQ;
        auto Vec = Project_NameS.split(",");
        for(auto v:Vec){vecQ.push_back(v);}
        return vecQ;
    }

    QString GetProjectNameByIndex(int index){
        try{
            std::vector<QString> vecQ;
            auto Vec = Project_NameS.split(",");
            for(auto v:Vec){vecQ.push_back(v);}
            return vecQ.at(index - 1);
        }catch(std::out_of_range ex){
            return "TMP";
        }

    }

    void UpateProjectNames(int index, QString PrjName){
        std::unordered_map<int,QString> UMap;
        std::vector<QString> vecQ;
        auto Vec = Project_NameS.split(",");
        int i = 0, vmax = -1, umax = index;
        for(auto v:Vec){
            vecQ.push_back(v);
            UMap[i] = v;
            i++;
        }


        UMap[index - 1] = PrjName;

        vmax = i>index?i:index;
        QString PrjNames;
        for(int j = 0;j<=vmax;j++){
            if(UMap[j].length() > 0){
                PrjNames = PrjNames + UMap[j] + ",";
            }
            else {
                PrjNames = PrjNames + " ,";
            }
        }
        Project_NameS = PrjNames;
    }

    int Project_Sel = 1;
    QString Project_Name = "A3";
    QString Project_NameS = "A3";
    QString Acc = "2000";
    QString Spd = "2000";
    QString Pos = "0";
    int SampleNum = 15;
    double Row[MAX_VOL] = {0,0,0,0,0,0,0,0};
    double Column[MAX_VOL] = {0,0,0,0,0,0,0,0};
    double Angle[MAX_VOL] = {0,0,0,0,0,0,0,0};
    double AbAngle[MAX_VOL] = {0,0,0,0,0,0,0,0};
    double Len1[MAX_VOL] = {16,0,0,0,0,0,0,0};
    double Len2[MAX_VOL] = {256,0,0,0,0,0,0,0};
    double Offset[4*MAX_VOL]= {0,0,0,0,0,0,0,0};
    double Offset1[4*MAX_VOL]= {0,0,0,0,0,0,0,0};
    int Flip[MAX_VOL] = {1,1,0,0,0,0,0,0};
    double RatioX = -1.0;
    double RatioY = -1.0;
    double OriginX = 0.0;
    double OriginY = 0.0;
    double CamAngle = 0.0;
    int Readdersel = 0;
    int isDrawing = 0;
    int InvertPoint = 0;
    //通过计算重定位点到定位点的距离来评估高度差
    double EstimateHighLow = 0.0;
    double CornerR = 0;

    double threshold_var[MAX_VOL] = {85.0,85.0,0,0,0,0,0,0};
    double chopstick_area[MAX_VOL] = {50000,50000,0,0,0,0,0,0};
    double mean_var[MAX_VOL] = {5,5,0,0,0,0,0,0};
    double factor[MAX_VOL] = {0.7,0.7,0,0,0,0,0,0};
    double erison_radius[MAX_VOL] = {12,12,0,0,0,0,0,0};
    double threshold_bias[MAX_VOL] = {-1,-1,0,0,0,0,0,0};
    double d[MAX_VOL] = {9,9,0,0,0,0,0,0};
    double sigmaColor[MAX_VOL] = {20,20,0,0,0,0,0,0};
    double sigmaSpace[MAX_VOL] = {5,5,0,0,0,0,0,0};
    double blockSize[MAX_VOL] = {75,75,0,0,0,0,0,0};


};

/*
请配合 Ini类文件使用
使用方法，在初始化地方如下例子即可
//系统配置文件
Preference::GetIns()->sys->SetFilePos(QString("sys.ini"));
//项目配置文件
QString Path = Preference::GetIns()->sys->Project_Name + "/prj.ini";
Preference::GetIns()->prj->SetFilePos(Path);
*/

class Preference
{
public:
    Preference() {
        sys = new Ini("sys");
        prj = new Ini("prj");
    }
    ~Preference() { delete sys; delete prj; }
    static Preference *GetIns() { static Preference ini; return &ini; }
    Ini *sys;
    Ini *prj;
};




#endif // PREFERENCE_H
