#include "machine.h"
#include "imagedeal.h"
#include "mutex"
#include "global.h"
#include "../PLC_DLL/PLC_DLL/plc_dll.h"
#include "mcamera.h"
#include <QDir>
//定义检测错误为0 上为1 左为2 下为3 右为4 触发定义地址D300和D301
//定义检测结果是否有花纹 ，触发定义302

//检测结果方向返回的20个数据，前是个为第一个相机，后10个位第二个相机
#define CHOPDIR1 310
//D310 D311 D312 D313 D314 D315 D316 D317 D318 D319
#define CHOPDIR2 340
//D340          .....                          D349

//返回结果是否有花纹正面
//D360 D361
#define TRIGGER
//std::mutex mtx_info;
std::mutex mtx_Image;
std::mutex mtx_Information;

#define IMAGE_IN 0
#define IMAGE_OUT 1
#define MESSAGE_IN 0
#define MESSAGE_OUT 1

bool InitCamera = false;

void ImageInOut(){

}


int SnapCounter1 = 0, SnapCounter2 = 0;
//global
//std::mutex mtxImage;
struct ImageInfo{
public :
    ImageInfo(cv::Mat *_Image, int _WhichCamera,int _Triggertime){
      //  std::lock_guard<std::mutex> lck(mtxImage);
        if(_Image) Image = _Image->clone();
        WhichCamera = _WhichCamera;
        TriggerTime = _Triggertime;
    }
    ~ImageInfo(){
     //   std::lock_guard<std::mutex> lck(mtxImage);
        Image.release();
    }
    cv::Mat Image;// = nullptr;
    int WhichCamera = 0;
    int TriggerTime = 0;
};

//PLC不能应急突发数据，所以要慢慢写入，如果多个数据一起写，可能引起bug
//std::mutex _mtx;
struct tagListPLC{
public:

    tagListPLC(int _DA = 302, int _DL = 0x1, std::string _Data = "0000", bool _isRead = false,bool _CanBeExe = true,std::string Address = "D"){
   //     std::lock_guard<std::mutex> lck(_mtx);
        DataAddr = _DA;
        DataLen = _DL;
        Data = _Data;
        isRead = _isRead;
        Addr = Address;
        CanBeExe = _CanBeExe;
    }
    ~tagListPLC(){
      //  std::lock_guard<std::mutex> lck(_mtx);
        Data.clear();
    }
    int DataAddr = 302;
    int DataLen = 0x1;
    std::string Data = "0000",Addr = "D";
    bool isRead;
    bool CanBeExe = true;
};




std::list<tagListPLC>  ListMsg;
std::list<ImageInfo> ListImages;
int TriggerCount2 = 0,TriggerCount1 = 0;
std::string Directions1,Directions2;


void ImageInOut(ImageInfo & Image, int Sel){

    static std::mutex Mtx;
    std::lock_guard<std::mutex> lck(Mtx);
    if(Sel == IMAGE_IN){
        ListImages.push_back(Image);
    }
    else
    {
        if(!ListImages.empty())
        ListImages.pop_front();
    }
}

void MessageInOut(tagListPLC & Info, int Sel){
    static std::mutex Mtx;
    std::lock_guard<std::mutex> lck(Mtx);
    if(Sel == IMAGE_IN){
        ListMsg.push_back(Info);
        //ListImages.push_back(Image);
    }
    else
    {
        if(!ListMsg.empty())
        ListMsg.pop_front();
        //ListImages.pop_front();
    }

}

Machine::Machine()
{
    StartThread();
}


#define TRIGGER
//PLC读取
unsigned int __stdcall PLC(void * lp){
    Machine *pMachine = (Machine*)lp;
    std::cout << "start PLC \n" << pMachine->name() <<std::endl;
    //初始化PLC参数等等
    int Trigger1 = 0,Trigger2 = 0;

bool isOK = false,OisOK = false;

//通讯plc地址
//通讯plc端口
#define PLCADDERSS "192.168.0.111"
#define PLCPORT     4999

    int Back305Flag = 0;
    sleep(500);
    global::GetIns()->History.push_back("PLC连接中...");
    for(;;){

#if 0
        for (;;) {

        sleep(90);
        cv::Mat Image;
        cv::Mat Black(500,500,CV_8UC1,cv::Scalar(0,0,0));

        HObject Hobj;

        try {


       MCamera::GetIns()->GrabImageA(Image,0,FLIPIMAGE[0]);

        MSerials::Mat2Hobj(Image,Hobj);
        if(!Image.empty()) cv::imwrite("AACC.jpg",Image);
        MSerials::h_disp_obj(Hobj,global::GetIns()->Cam3Disp);

        } catch (std::exception ex) {
            MSerials::Mat2Hobj(Black,Hobj);
            MSerials::h_disp_obj(Hobj,global::GetIns()->Cam3Disp);

            char ErrorCode[256] = {0};
            sprintf_s(ErrorCode,"超出相机初始化数量 1");
            SetTposition(global::GetIns()->Cam3Disp, 0, 1);
            WriteString(global::GetIns()->Cam3Disp, ex.what());

        }catch(cv::Exception ex){
            MSerials::Mat2Hobj(Black,Hobj);
            MSerials::h_disp_obj(Hobj,global::GetIns()->Cam3Disp);

            char ErrorCode[256] = {0};
            sprintf_s(ErrorCode,"超出相机初始化数量 2");
            SetTposition(global::GetIns()->Cam3Disp, 0, 1);
            WriteString(global::GetIns()->Cam3Disp, ErrorCode);
                    }
        catch(HalconCpp::HException ex){
            MSerials::Mat2Hobj(Black,Hobj);
            MSerials::h_disp_obj(Hobj,global::GetIns()->Cam3Disp);

            HTuple Error;
            ex.ToHTuple(&Error);
            char ErrorCode[256] = {0};
        //    sprintf_s(ErrorCode,"%s 超时 超出相机初始化数量 3 ",Error[0].S().Text());
            SetTposition(global::GetIns()->Cam3Disp, 0, 1);
            WriteString(global::GetIns()->Cam3Disp, Error);


        }
        }
#endif
            if(!isOK)
            {
                Back305Flag = 0;
                try{
                    SocketClear();
                    sleep(1000);
                    isOK = SocketInit("192.168.1.111",4998);
                }catch(std::exception ex){isOK = false;}
                 catch(std::out_of_range ex){isOK = false;}
                if(!isOK && isOK!=OisOK){
                    global::GetIns()->History.push_back("PLC连接失败");
                }
                else if(isOK && isOK!=OisOK)
                {
                    global::GetIns()->History.push_back("PLC连接成功");
                }

            }
            OisOK = isOK;

 if(isOK){

        int TriggerTime1 = 0;
        int TriggerTime2 = 0;
        int TriggerCam3 = 1;
       // bool Shold305Back = false;
        //0表示不需要写回305


     //队列和PLC通讯，否则会产生无法理解的bug
        std::string ToReadPos ;//= PLCReadStrEX(GetHostByIndex(0),"D",300,3);
        {
            //std::lock_guard<std::mutex> lck(mtx_Information);
            tagListPLC Var = tagListPLC(300,0x3,"0000",true);
            MessageInOut(Var,IMAGE_IN);

        }
        //clock_t Old_clk = clock();
        clock_t Clk = clock();
        static clock_t Old_Clock = Clk;
        if((Clk - Old_Clock) > 1000){
             Old_Clock = Clk;
             tagListPLC Var = tagListPLC(306,0x1,"0001",false);
             MessageInOut(Var,IMAGE_IN);
             //std::lock_guard<std::mutex> lck(mtx_Information);
             //listMsg.push_back(tagListPLC(306,0x1,"0001",false));
        }


        for (;!ListMsg.empty();) {
            sleep(1);
            if(ListMsg.front().isRead){
                ToReadPos = PLCReadStrEX(GetHostByIndex(0),ListMsg.front().Addr.c_str(),ListMsg.front().DataAddr,ListMsg.front().DataLen);
            }else{
                //sleep(1);
                //对于305来说 如果，进行一个死循环知道可以读取到0信号，因为305标致着检测完成

                if(ListMsg.front().DataAddr == 305 && "0001" == ListMsg.front().Data){
                    //Shold305Back = true;
                    Back305Flag = 1;
                }
                else {
                    PLCWriteBitEX(GetHostByIndex(0),ListMsg.front().Addr.c_str(),ListMsg.front().DataAddr,ListMsg.front().DataLen,ListMsg.front().Data.c_str());
                }
            //    if(listMsg.front().DataAddr == 305 && "0001" == listMsg.front().Data && 0==TriggerCam3){

                    /*
                    bool isLoop = true;
                    //超时判断，如果断开了PLC，则可能发生失去连接
                    for (;isLoop && ((clock()-Old_clk)<261);) {
                        PLCWriteBitEX(GetHostByIndex(0),"D",302,0x1,"0000");
                        std::string Res302 = PLCReadStrEX(GetHostByIndex(0),"D",302,0x1);
                        if(Res302.length()>0){
                            if(0 == strtol(Res302.c_str(),NULL,16)){
                                sleep(1);
                                isLoop = false;
                                PLCWriteBitEX(GetHostByIndex(0),listMsg.front().Addr.c_str(),listMsg.front().DataAddr,listMsg.front().DataLen,listMsg.front().Data.c_str());
                            }
                        }
                    }//if
                    */
              //  }

            }
            {
                   // std::lock_guard<std::mutex> lck(mtx_Information);
                   // ListMsg.pop_front();
                tagListPLC Var = tagListPLC(306,0x1,"0001",false);
                MessageInOut(Var,IMAGE_OUT);
            //    MessageInOut(IMAGE_OUT);
            }
        }




        if((clock()-Clk)>260){
             global::GetIns()->History.push_back("PLC失去连接");
             isOK = false;
             OisOK = isOK;
             continue;
        }

        //bug 不确定一次能读取多少字节，注意相机1 2 3 的触发
        if(ToReadPos.length()<12) continue;
        //PLC端需要清零操作，
        TriggerTime1 = strtol(ToReadPos.substr(0,4).c_str(), nullptr, 16);
        static int OldTriggerTime1 = TriggerTime1;

        TriggerTime2 = strtol(ToReadPos.substr(4,4).c_str(), nullptr, 16);
        static int OldTriggerTime2 = TriggerTime2;

        TriggerCam3 = strtol(ToReadPos.substr(8,4).c_str(), nullptr, 16);
        //if(Shold305Back) TriggerCam3 = 0;
        static int OldTriggerCam3 = TriggerCam3;

        if(0 !=TriggerCam3 && 1==Back305Flag){
            PLCWriteBitEX(GetHostByIndex(0),"D",302,0x1,"0000");
        }
        else if(0 ==TriggerCam3 && 1==Back305Flag){
            Back305Flag = 0;
            PLCWriteBitEX(GetHostByIndex(0),"D",305,0x1,"0001");
        }

        if(TriggerTime1 < 0 || TriggerTime1 > 99 || TriggerTime2 < 0 || TriggerTime2 > 99){
            continue;
        }


#ifndef TRIGGER

        //即将开始检测的时候就清零
        static bool Clear1 = true,Clear2 = true;
        if(0 != TriggerTime1 && TriggerTime1 != OldTriggerTime1 && Clear1) {Directions1.clear();TriggerCount1 = 0;Trigger1 = 0;
                    SnapCounter1 = 0;
                    Clear1 = false;}
        else if(0 ==TriggerTime1 && TriggerTime1 != OldTriggerTime1 && SnapCounter1>=Trigger1){TriggerCount1 = -1; Clear1 = true;}


        if(0 != TriggerTime2 && TriggerTime2 != OldTriggerTime2 && Clear2) {Directions2.clear();TriggerCount2 = 0;Trigger2 = 0;
                    SnapCounter2 = 0;
                    Clear2 = false;}
        else if(0==TriggerTime2 && TriggerTime2 != OldTriggerTime2 && SnapCounter2>=Trigger2){TriggerCount2 = -1;  Clear2 = true;}
        //因为要并发，所以这个地方进行计数式的触发,万一出现 两次呢
        if(TriggerTime1 >0 && TriggerTime1 != OldTriggerTime1)
        {
            if((TriggerTime1-OldTriggerTime1)>0)
            Trigger1 += (TriggerTime1-OldTriggerTime1);
        }
        if(TriggerTime2 >0 && TriggerTime2 != OldTriggerTime2)
        {
            if((TriggerTime2-OldTriggerTime2)>0)
            Trigger2 += (TriggerTime2-OldTriggerTime2);
        }



        if(Trigger1>0 && !Machine::GetIns()->EvtCam1.State()){
            Machine::GetIns()->EvtCam1.SetEvent();
            Trigger1--;
        }
        if(Trigger2>0 && !Machine::GetIns()->EvtCam2.State()){
            Machine::GetIns()->EvtCam2.SetEvent();
            Trigger2--;
        }

#else

        //即将开始检测的时候就清零
        static bool Clear1 = true,Clear2 = true;
        if(0 != TriggerTime1 && TriggerTime1 != OldTriggerTime1) {Directions1.clear();TriggerCount1 = 0;Trigger1 = 0;
                    SnapCounter1 = 0;
                    Clear1 = false;


        }
        else if(0 ==TriggerTime1 && TriggerTime1 != OldTriggerTime1){TriggerCount1 = -1; Clear1 = true;

        }


        if(0 != TriggerTime2 && TriggerTime2 != OldTriggerTime2) {Directions2.clear();TriggerCount2 = 0;Trigger2 = 0;
                    SnapCounter2 = 0;
                    Clear2 = false;

        }
        else if(0==TriggerTime2 && TriggerTime2 != OldTriggerTime2){TriggerCount2 = -1;  Clear2 = true;

        }


#endif

        //触发相机3
        if(1 == TriggerCam3 && OldTriggerCam3 != TriggerCam3 && !Machine::GetIns()->EvtCam3.State()){Machine::GetIns()->EvtCam3.SetEvent();}

        OldTriggerTime1 = TriggerTime1;
        OldTriggerTime2 = TriggerTime2;
        OldTriggerCam3 = TriggerCam3;


  }//isOK


    }
}


//检测青面抓图
unsigned int __stdcall Cam1Cap(void * lp){
    Machine *pMachine = (Machine*)lp;
    int iDx = 0;
    std::cout << "start1cap\n " << pMachine->name() <<std::endl;
    for(;!global::GetIns()->isGoingToExit;){

#ifndef TRIGGER
        ::WaitForSingleObject(Machine::GetIns()->EvtCam1.get(),INFINITE);
        cv::Mat Image;
        int Idx = 0;
        MCamera::GetIns()->GrabImageA(Image,Idx,FLIPIMAGE[Idx]);
        listImages.push_back(ImageInfo(&Image,Idx,TriggerCount1++));


        Machine::GetIns()->EvtCam1.ResetEvent();

#else
        if(MCamera::GetIns()->CameraNum() < 3) {
            sleep(100);
            continue;
        }
        cv::Mat Image;
        int Idx = 0;

        try {
        MCamera::GetIns()->GrabImageA1(Image,Idx,FLIPIMAGE[Idx]);
      //  std::lock_guard<std::mutex> lck(mtx_Image);
    //    listImages.push_back(ImageInfo(&Image,Idx,TriggerCount1++));
        ImageInfo imgInfo(&Image,Idx,TriggerCount1++);
        ImageInOut(imgInfo,IMAGE_IN);
        char Msg[256] = {0};
        sprintf_s(Msg,"相机1拍照%d次",iDx++);
      //  global::GetIns()->History.push_back(Msg);
        } catch (std::exception ex) {

        }
#endif
    }
    return 0;
}

//检测青面抓图
unsigned int __stdcall Cam2Cap(void * lp){
    Machine *pMachine = (Machine*)lp;
    std::cout << "start2cap\n " << pMachine->name() <<std::endl;
    for(;!global::GetIns()->isGoingToExit;){
#ifndef TRIGGER
        ::WaitForSingleObject(Machine::GetIns()->EvtCam1.get(),INFINITE);
        cv::Mat Image;
        int Idx = 1;
        MCamera::GetIns()->GrabImageA(Image,Idx,FLIPIMAGE[Idx]);
        listImages.push_back(ImageInfo(&Image,Idx,TriggerCount1++));
        Machine::GetIns()->EvtCam1.ResetEvent();
#else

        if(MCamera::GetIns()->CameraNum() < 3) {
            sleep(100);
            continue;
        }
        cv::Mat Image;
        int Idx = 1;
        try {
        MCamera::GetIns()->GrabImageA2(Image,Idx,FLIPIMAGE[Idx]);
        ImageInfo imgInfo(&Image,Idx,TriggerCount2++);
        ImageInOut(imgInfo,IMAGE_IN);
        } catch (std::exception ex) {

        }
#endif
    }
    return 0;
}

//检测青面
#define ERRORCHOP "0000000000000000000000000000000000000000"
unsigned int __stdcall Cam12Deal(void * lp){
    Machine *pMachine = (Machine*)lp;
    std::cout << "start12Deal\n " << pMachine->name() <<std::endl;
    std::string DirInfos[MAX_VOL];
    for(;;){
        sleep(1);
        for(;!ListImages.empty();){
            cv::Mat *Image = &ListImages.front().Image;
            int WhichCamera = ListImages.front().WhichCamera;
            int Idx = WhichCamera;
            int TriggerTime = ListImages.front().TriggerTime;
            int Res = 0;
            std::string result = "";
            try{
                switch (WhichCamera) {
                case 0:Res =
                    MSerials::CheckUpSideOld(*Image, global::GetIns()->Cam1Disp,THRESHOLD[Idx], CHOPSTICKAREA[Idx] ,
                                   0.6,
                                   DOUBLEFILTERD[Idx],
                                   DOUBLEFILTERSC[Idx],
                                   DOUBLEFILTERSS[Idx],
                                   THRESHOLDBIAS[Idx],
                                   DOUBLEFILTERBS[Idx],
                                   &result);
                    break;
                case 1:Res =
                    MSerials::CheckUpSideOld(*Image, global::GetIns()->Cam2Disp,THRESHOLD[Idx], CHOPSTICKAREA[Idx] ,
                                   0.6,
                                   DOUBLEFILTERD[Idx],
                                   DOUBLEFILTERSC[Idx],
                                   DOUBLEFILTERSS[Idx],
                                   THRESHOLDBIAS[Idx],
                                   DOUBLEFILTERBS[Idx],
                                   &result);
                    break;


                default:break;
                }
            }catch(std::exception ex){
            }catch(std::out_of_range ex){
            }catch(HalconCpp::HException ex){
            }catch(cv::Exception ex){}
            char R[256] = {0};
            sprintf_s(R,"%04X",Res);
            std::string Dir = "错误";
            switch (Res) {
            case UP:Dir = "上";break;
            case LEFT:Dir = "左";break;
            case DOWN:Dir = "下";break;
            case RIGHT:Dir = "右";break;
            default:break;

            }
            int Counter = 0;
            if(0 == WhichCamera){
                if(0 == SnapCounter1){
                    DirInfos[WhichCamera].clear();
                }
                DirInfos[WhichCamera] += std::string(" ") + std::string(QString::number(Res).toLocal8Bit().data());
                Directions1 += std::string(R);
                SnapCounter1++;
                Counter = SnapCounter1;
                SetTposition(global::GetIns()->Cam1Disp,10,10);
                SetColor(global::GetIns()->Cam1Disp,"green");
                sprintf_s(R,"第%d次拍摄 %s 方向序列：%s",SnapCounter1,Dir.c_str(),DirInfos[WhichCamera].c_str());
                WriteString(global::GetIns()->Cam1Disp,R);
            }else {
                if(0 == SnapCounter2){
                    DirInfos[WhichCamera].clear();
                }
                DirInfos[WhichCamera] += std::string(" ") + std::string(QString::number(Res).toLocal8Bit().data());
                Directions2 += std::string(R);
                SnapCounter2++;
                Counter = SnapCounter2;
                SetTposition(global::GetIns()->Cam2Disp,10,10);
                SetColor(global::GetIns()->Cam2Disp,"green");
                 sprintf_s(R,"第%d次拍摄 %s 方向序列：%s",SnapCounter2,Dir.c_str(),DirInfos[WhichCamera].c_str());
                WriteString(global::GetIns()->Cam2Disp,R);
            }

            if(ISSAVEIMAGE){
                try{

                    QDateTime time = QDateTime::currentDateTime();
                    QString Path;
                    if(0==WhichCamera){
                        Path = PROJECTNAME + "/" + "Cam1";
                    }else {
                        Path = PROJECTNAME + "/" + "Cam2";
                    }
                    QDir qdir;
                    qdir.mkdir(Path);
                    QString ImageFile = Path + "/" + time.toString("yyyy-MM-dd-HH-mm-ss-zzz-") + QString::fromLocal8Bit("相机") + QString::number(1+WhichCamera) + \
                         QString::fromLocal8Bit("方向") +QString::number(Res) + QString::fromLocal8Bit(Dir.c_str()) + QString::number(Counter) + ".jpg";
                    cv::imwrite(ImageFile.toLocal8Bit().data(),*Image);
                }catch(cv::Exception ex){

                }
            }
            {
                int Idx = 1;
                cv::Mat Image = cv::Mat(100,100,CV_8UC1);
                ImageInfo imgInfo(&Image,Idx,TriggerCount2++);
                ImageInOut(imgInfo,IMAGE_OUT);

            //    std::lock_guard<std::mutex> lck(mtx_Image);
             //   listImages.pop_front();
            }
        }
        //令字节数不超过40个
        if(TriggerCount1 == -1){
            std::string BackPLC = Directions1;
            if(Directions1.length()>80){
                BackPLC = Directions1.substr(0,80);
            }
#if 0
                //std::cout << listMsg.front().Data.c_str() <<std::endl;
                global::GetIns()->History.push_back(std::string("返回数据为：")+BackPLC);
#endif

            int len = BackPLC.length()/4;
            if(len > 0){
            //    std::lock_guard<std::mutex> lck(mtx_Information);
            //    listMsg.push_back(tagListPLC(CHOPDIR1,len,BackPLC.c_str(),false));
           //     listMsg.push_back(tagListPLC(303,0x1,"0001",false));
                tagListPLC Var = tagListPLC(CHOPDIR1,len,BackPLC.c_str(),false);
                 tagListPLC Var1 = tagListPLC(303,0x1,"0001",false);
                MessageInOut(Var,IMAGE_IN);
                MessageInOut(Var1,IMAGE_IN);
            }
            else {
        //        std::lock_guard<std::mutex> lck(mtx_Information);
       //         listMsg.push_back(tagListPLC(CHOPDIR1,strlen(ERRORCHOP)/4,ERRORCHOP,false));
      //          listMsg.push_back(tagListPLC(303,0x1,"0001",false));

                tagListPLC Var = tagListPLC(CHOPDIR1,strlen(ERRORCHOP)/4,ERRORCHOP,false);
                 tagListPLC Var1 = tagListPLC(303,0x1,"0001",false);
                MessageInOut(Var,IMAGE_IN);
                MessageInOut(Var1,IMAGE_IN);


            }
            Directions1.clear();
            TriggerCount1 = 0;
        }


        if(TriggerCount2 == -1){
            std::string BackPLC = Directions2;
            if(Directions2.length()>80){
                BackPLC = Directions2.substr(0,80);
            }
            int len = BackPLC.length()/4;
            if(len > 0)
            {

                tagListPLC Var = tagListPLC(CHOPDIR2,len,BackPLC.c_str(),false);
                 tagListPLC Var1 = tagListPLC(304,0x1,"0001",false);
                MessageInOut(Var,IMAGE_IN);
                MessageInOut(Var1,IMAGE_IN);

            }else{


                tagListPLC Var = tagListPLC(CHOPDIR2,strlen(ERRORCHOP)/4,ERRORCHOP,false);
                 tagListPLC Var1 = tagListPLC(304,0x1,"0001",false);
                MessageInOut(Var,IMAGE_IN);
                MessageInOut(Var1,IMAGE_IN);


            }
            Directions2.clear();
            TriggerCount2 = 0;
        }
    }
}

//检测花纹
unsigned int __stdcall Cam3(void * lp){
    Machine *pMachine = (Machine*)lp;
    std::cout << "start3deal\n " << pMachine->name() <<std::endl;
    for(;;){
        ::WaitForSingleObject(Machine::GetIns()->EvtCam3.get(),INFINITE);
        clock_t clk = clock();
        bool isOK = false;
        std::string DataToPLC,ForShow;
        try{
        HalconCpp::HObject ho_Image;
        size_t CamIdx = 2;
        MCamera::GetIns()->GrabImageA3(global::GetIns()->vecImages.at(CamIdx),CamIdx);
        MSerials::Mat2Hobj(global::GetIns()->vecImages.at(CamIdx),ho_Image);
        MSerials::h_disp_obj(ho_Image, global::GetIns()->Cam3Disp);
        int Idx = 1;
        std::vector<double> Bias;
        for(int i = 0; i<CHECKNUM;i++){
            Bias.push_back(LINEOFFSET[i]);
        }
        std::vector <double> res = MSerials::CheckLogo(global::GetIns()->Cam3Disp,&global::GetIns()->vecImages.at(CamIdx),ROW[Idx],COLUMN[Idx],ABANGLE[Idx],LEN1[Idx],LEN2[Idx],Bias,LEN1[0],LEN2[0],RECTVBIAS[Idx]);
  //      if(abs(ABANGLE[Idx])>(3.1415926535/2)){
  //          for(int index = res.size()-1; index >=0; index--){
  //              if(res.at(index)>0){
   //                 DataToPLC += "0001";
   //                 ForShow+="1";
   //             }
   //             else {
  //                  DataToPLC += "0000";
   //                 ForShow+="0";
  //              }
   //         }
  //      }else {
            for(auto v : res){
                if(v > 0){
                    DataToPLC += "0001";
                    ForShow+="1";
                }
                else {
                    DataToPLC += "0002";
                    ForShow+="0";
                }
            }
 //       }



 ForShow+="  ";


        Idx = 2;
        Bias.clear();
        for(int i = 0; i<CHECKNUM;i++){
             Bias.push_back(LINEOFFSET1[i]);
        }
        std::vector <double> res1 = MSerials::CheckLogo(global::GetIns()->Cam3Disp,&global::GetIns()->vecImages.at(CamIdx),ROW[Idx],COLUMN[Idx],ABANGLE[Idx],LEN1[Idx],LEN2[Idx],Bias,LEN1[0],LEN2[0],RECTVBIAS[Idx]);
 //       if(abs(ABANGLE[Idx])>(3.1415926535/2)){
 //           for(int index = res1.size()-1; index >=0; index--){
 //               if(res1.at(index)>0){
 //                   DataToPLC += "0001";
//                    ForShow+="1";
 //               }
 //               else {
 //                   DataToPLC += "0000";
  //                  ForShow+="0";
 //               }
  //          }
 //       }else {
            for(auto v : res1){
                if(v > 0){
                    DataToPLC += "0001";
                    ForShow+="1";
                }
                else {
                    DataToPLC += "0002";
                    ForShow+="0";
                }
            }
 //       }
            {

                //协定相机3传回数据 从370开始1为识别正确，2为识别错误 0是没有识别一共有2*双筷子数据
                tagListPLC Var = tagListPLC(370,DataToPLC.length()/4,DataToPLC,false);
                MessageInOut(Var,IMAGE_IN);
            }


        //准备返回的数据
        int Value = 0, i = 0;
        char TMP[256] = {0};
        for(auto v : res){ if(v>0) Value++;}
        for(auto v : res1){ if(v>0) Value++;}
        sprintf_s(TMP,"%04X",Value);
        std::string toPLC(TMP);
        SetColor(global::GetIns()->Cam3Disp,"cyan");
        SetTposition(global::GetIns()->Cam3Disp, 0, 1);
        char ppp[256] ={0};
        sprintf_s(ppp,"正面花纹数量%d 计算时间为%d ms 结果%s\n",Value,clock()-clk,ForShow.c_str());
        WriteString(global::GetIns()->Cam3Disp, ppp);
        {
 //           std::lock_guard<std::mutex> lck(mtx_Information);
 //           listMsg.push_back(tagListPLC(360,toPLC.length()/4,toPLC,false));

            tagListPLC Var = tagListPLC(360,toPLC.length()/4,toPLC,false);
            MessageInOut(Var,IMAGE_IN);

        }
        isOK = true;
        }catch(HalconCpp::HException ex){

            HTuple Error;
            ex.ToHTuple((&Error));
            SetTposition(global::GetIns()->Cam3Disp,10,10);
            SetColor(global::GetIns()->Cam3Disp,"red");
            WriteString(global::GetIns()->Cam3Disp,Error);

        }catch(cv::Exception ex){

            SetTposition(global::GetIns()->Cam3Disp,10,10);
            SetColor(global::GetIns()->Cam3Disp,"red");
            WriteString(global::GetIns()->Cam3Disp,ex.what());
        }
        catch(std::exception ex){

            SetTposition(global::GetIns()->Cam3Disp,10,10);
            SetColor(global::GetIns()->Cam3Disp,"red");
            WriteString(global::GetIns()->Cam3Disp,ex.what());
        }catch(std::out_of_range ex){

            SetTposition(global::GetIns()->Cam3Disp,10,10);
            SetColor(global::GetIns()->Cam3Disp,"red");
            WriteString(global::GetIns()->Cam3Disp,ex.what());
        }
        {
   //         std::lock_guard<std::mutex> lck(mtx_Information);
  //          if(!isOK)  listMsg.push_back(tagListPLC(360,1,"0000",false));
  //          listMsg.push_back(tagListPLC(302,0x1,"0000",false));
 //           listMsg.push_back(tagListPLC(305,0x1,"0001",false));

            if(!isOK){
                tagListPLC Var = tagListPLC(360,1,"0000",false);
                MessageInOut(Var,IMAGE_IN);
            }
            tagListPLC Var = tagListPLC(302,0x1,"0000",false);
            MessageInOut(Var,IMAGE_IN);
            tagListPLC Var1 = tagListPLC(302,0x1,"0000",false);
            MessageInOut(Var1,IMAGE_IN);
        }
        Machine::GetIns()->EvtCam3.ResetEvent();
    }
}

void Machine::OnTimer(){




}

void Machine::ImageIn1(cv::Mat &InputArray, int idx){
    std::lock_guard<std::mutex> lck(mtx_Information);
  //  listImages.push_back(ImageInfo(&InputArray,idx,TriggerCount2++));

}


void Machine::StartThread(){
    //0xC == 001100
    HANDLE Cam1 = (HANDLE)_beginthreadex(NULL, 0, Cam1Cap, this, 0, 0);
    SetThreadAffinityMask(Cam1,0x8);
    SetThreadPriority(Cam1, THREAD_PRIORITY_TIME_CRITICAL);

    HANDLE Cam2 = (HANDLE)_beginthreadex(NULL, 0, Cam2Cap, this, 0, 0);
    SetThreadAffinityMask(Cam2,0x4);
    SetThreadPriority(Cam2, THREAD_PRIORITY_TIME_CRITICAL);

    SetThreadAffinityMask((HANDLE)_beginthreadex(NULL, 0, Cam12Deal, this, 0, 0),0x2);
    //10
    SetThreadAffinityMask((HANDLE)_beginthreadex(NULL, 0, Cam3, this, 0, 0),0x2);

    HANDLE PLCThread = (HANDLE)_beginthreadex(NULL, 0, PLC, this, 0, 0);
    SetThreadAffinityMask(PLCThread ,0x1);
    SetThreadPriority(PLCThread, THREAD_PRIORITY_TIME_CRITICAL);

    gtime_ID = timeSetEvent(1, 1, (LPTIMECALLBACK)TimeEvent, 1, TIME_PERIODIC);
    if (gtime_ID == NULL)
    {
        printf("定时器设定失败\n");
        return;
    }
}

