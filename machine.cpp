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

#define ERRORANGLE -720.0

#define SND_LEN 80
unsigned char SndMsg[80];
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
    Angles = std::vector<float>(AnglesSize,0);
}


#define TRIGGER



#if 1
std::atomic<bool> isOK(false), OisOK(false);// = static_cast<bool>(false);

//PLC读取
unsigned int __stdcall PLC(void * lp){
    Machine *pMachine = (Machine*)lp;
    Q_UNUSED(pMachine);
    unsigned char *data = new unsigned char[1024];

    //    bool isOK = false,OisOK = false;

    //通讯plc地址
    //通讯plc端口

#define PLCADDERSS Preference::GetIns()->prj->IP.toLocal8Bit().data()
#define PLCPORT     Preference::GetIns()->prj->Port

    int Back305Flag = 0;

    for(;;){
        if(!isOK)
        {
            Back305Flag = 0;
            try{
                SocketClear();
                sleep(1000);
                global::GetIns()->History.push_back("PLC连接中...");
                //   isOK = SocketInit("192.168.1.111",4998);
                isOK = SocketInit((const char*)PLCADDERSS,PLCPORT);
            }catch(std::exception ex){
                Q_UNUSED(ex);
                isOK = false;
            }
            catch(std::out_of_range ex){isOK = false;}
            if(!isOK && isOK!=OisOK){
                global::GetIns()->History.push_back("PLC连接失败");
            }
            else if(isOK && isOK!=OisOK)
            {
                global::GetIns()->History.push_back("PLC连接成功");
            }

        }
        OisOK = static_cast<bool>(isOK);

        static short PreTriggerValue = 0;
        //D406为拍照数据
        if(isOK){
            //clear buffer
            //huo
            //这个函数内部已经包好互斥锁，所以没必要加锁
            auto DataLen = ModbusReadHoldingRegisters(1,0x196,0x1,data,GetHostByIndex(0));
            if(DataLen<1){
                isOK = false;
            }else{
                /*
                    规则：
                    从PLC的D406读取数据，如果读到0变1时候，电脑会清空数据为-1.0,表示异常
                    然后拍摄图片，处理，把结果存到第一个数据里，并存下0 90 180 270，表示角度
                    D406从1变为2时候，拍摄图片，处理，结果存到第二个数据里
                    当D406变回0时候，电脑会把处理的数据发送到PLC 的D4000里
                    当然，D406最大数据如果超过20，那么电脑不认这个数据，也不处理
                    建议plc在从0变为1以前，将数据清空为-2.0，表示数据没有接到返回，
                    如果电脑返回数据的话，可以查看D4000是否值为-2.0
            */
                //                short avlue = (data[9] << 8) | data[10];
                Machine::GetIns()->CurTriggerValue = (data[9] << 8) | data[10];
                if(Machine::GetIns()->CurTriggerValue != PreTriggerValue){
                    PreTriggerValue = Machine::GetIns()->CurTriggerValue;
                    //==0时候会返回数据，大于0时候会拍摄数据
                    if((Machine::GetIns()->CurTriggerValue >= 0) && (Machine::GetIns()->CurTriggerValue<=20))
                    {
                        tagImageInfomation imgInfo(Machine::GetIns()->CurTriggerValue);
                        std::lock_guard<std::mutex> lck(Machine::GetIns()->mtxPreImageInfo);
                        Machine::GetIns()->ListPreImageInfo.push_back(imgInfo);
                    }
                }


            }


        }//isOK


    }
}



#else
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

#endif


//检测青面抓图
unsigned int __stdcall Cam1Cap(void * lp){
    Machine *pMachine = (Machine*)lp;
    int iDx = 0;
    std::cout << "start1cap\n " << pMachine->name() <<std::endl;
    auto DispHand = global::GetIns()->Cam1Disp;
    global::GetIns()->isGoingToExit = true;
    //     global::GetIns()->History.push_back("开始拍照");
    // for(;!global::GetIns()->isGoingToExit;)
    while(true)
    {

#ifndef TRIGGER
        ::WaitForSingleObject(Machine::GetIns()->EvtCam1.get(),INFINITE);
        cv::Mat Image;
        int Idx = 0;
        MCamera::GetIns()->GrabImageA(Image,Idx,FLIPIMAGE[Idx]);
        listImages.push_back(ImageInfo(&Image,Idx,TriggerCount1++));


        Machine::GetIns()->EvtCam1.ResetEvent();

#else
        //根据获取的数据
        if(MCamera::GetIns()->CameraNum() < 1) {
            sleep(100);
            continue;
        }
        //        global::GetIns()->History.push_back("已经找到相机");
        cv::Mat Image;
        int Idx = 0;

        try {
            if(!Machine::GetIns()->ListPreImageInfo.empty()){
                //debug
                //                global::GetIns()->History.push_back("开始拍照");
                int SerialsIdx = Machine::GetIns()->ListPreImageInfo.front().Idx;
                //注意这个顺序
                {
                    std::lock_guard<std::mutex> lck(Machine::GetIns()->mtxPreImageInfo);
                    Machine::GetIns()->ListPreImageInfo.pop_front() ;
                }


                if(SerialsIdx > 0){
                    MCamera::GetIns()->GrabImageSDK(Image,Idx,FLIPIMAGE[Idx]);
                    {
                        char msg[64];
                        sprintf_s(msg,"拍照第%02d根筷子 ",SerialsIdx);
                        global::GetIns()->History.push_back(msg);
                        //加入检测队列
                        tagImageInfomation imgInfo(SerialsIdx);
                        imgInfo.Image = Image.clone();
                        std::lock_guard<std::mutex> lck(Machine::GetIns()->mtxImageInfo);
                        Machine::GetIns()->ListImageInfomation.push_back(imgInfo);
                    }
                }else{
                    //通知返回数据,函数内部已经加锁，仅仅需要判断链接状态
                    if(isOK){
                        //写20个数据，一共20*4字节 80个数据
                        std::string Result = "结果：";
                        for(int i = 0; i < (SND_LEN/4);i++){
                            //特别注意这个顺序！！！！
                            float Value = Machine::GetIns()->Angles.at(i);
                            if(Value<=ERRORANGLE){
                                Value = 0.0;
                            }
                            int AngleInt = *(int*)&Value;
                            SndMsg[i*4+2] = (AngleInt >> 24)&0xFF;
                            SndMsg[i*4+3] = (AngleInt >> 16)&0xFF;
                            SndMsg[i*4+0] = (AngleInt >> 8)&0xFF;
                            SndMsg[i*4+1] = AngleInt&0xFF;
                            char msg[256];
                            sprintf_s(msg,"%4.1f ",Machine::GetIns()->Angles.at(i));
                            if(i < 10){
                                Result += msg;
                            }
                        }
                        global::GetIns()->History.push_back(Result);
                        //D4000
                        ModbusPresetMultipleRegisters(1,0xFA0,SND_LEN,SndMsg,GetHostByIndex(0));

                        for(int i = 0; i < (SND_LEN/4);i++){
                            //特别注意这个顺序！！！！
                            float Value = Machine::GetIns()->Angles.at(i);
                            if(Value<=ERRORANGLE){
                                SndMsg[i*2+0] = 0;
                                SndMsg[i*2+1] = 2;
                            }else{
                                SndMsg[i*2+0] = 0;
                                SndMsg[i*2+1] = 1;
                            }
                        }
                        //D420  = 0x1A4
                        ModbusPresetMultipleRegisters(1,0x1A4,SND_LEN/2,SndMsg,GetHostByIndex(0));
                        for(auto & r:Machine::GetIns()->Angles){
                            r = ERRORANGLE-1.0;
                        }
                    }

                }



            }else{
                Sleep(1);
            }
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
    std::string result = "";
    auto DispHand = global::GetIns()->Cam1Disp;
    int Idx = 0;
    for(;;){
        bool hasInfo =false;
        int SerialsIdx = 0;
        try {

            if(!Machine::GetIns()->ListImageInfomation.empty()){
                hasInfo =true;
                float Angle = -1.0;
                cv::Mat  Image = Machine::GetIns()->ListImageInfomation.front().Image.clone();
                SerialsIdx = Machine::GetIns()->ListImageInfomation.front().Idx-1;

                //注意这个顺序！
                {
                    std::lock_guard<std::mutex> lck(Machine::GetIns()->mtxImageInfo);
                    Machine::GetIns()->ListImageInfomation.pop_front();
                }

                if(ISSAVEIMAGE){
                    QDateTime time = QDateTime::currentDateTime();
                    QString ImageFile = PROJECTNAME + "/" + time.toString("yyyy-MM-dd-HH-mm-ss-zzz-") + "Cam1" + ".jpg";
                    cv::imwrite(ImageFile.toLocal8Bit().data(),Image);
                }

                int res = MSerials::CheckUpSideOld(Image, global::GetIns()->Cam1Disp,THRESHOLD[Idx], CHOPSTICKAREA[Idx] ,
                                                   0.6,
                                                   DOUBLEFILTERD[Idx],
                                                   DOUBLEFILTERSC[Idx],
                                                   DOUBLEFILTERSS[Idx],
                                                   THRESHOLDBIAS[Idx],
                                                   DOUBLEFILTERBS[Idx],
                                                   &result);

                SetColor(DispHand,"green");
                SetTposition(DispHand, 0, 1);

                switch (res) {
                case UP:
                    WriteString(DispHand, "上");
                    Angle = 0.0;
                    break;
                case LEFT:
                    WriteString(DispHand, "左");
                    Angle = -90.0;
                    break;
                case DOWN:
                    WriteString(DispHand, "下");
                    Angle = 180.0;
                    break;
                case RIGHT:
                    WriteString(DispHand, "右");
                    Angle = 90.0;
                    break;
                default:
                    break;
                }
                if(SerialsIdx >= 0 && (SerialsIdx<Machine::GetIns()->AnglesSize)){
                    Machine::GetIns()->Angles.at(SerialsIdx) = Angle;
                }else{
                    global::GetIns()->History.push_back("获取序列异常！");
                }


            }else{
                Sleep(1);
            }


        } catch (std::exception &e) {
            if(hasInfo){
                if(SerialsIdx >= 0 && (SerialsIdx<Machine::GetIns()->AnglesSize)){
                    Machine::GetIns()->Angles.at(SerialsIdx) = ERRORANGLE-1.0;
                }else{
                    global::GetIns()->History.push_back("获取序列异常！");
                }
            }
            global::GetIns()->History.push_back(e.what());
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
            HalconCpp::WriteString(global::GetIns()->Cam3Disp, ppp);
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
            HalconCpp::WriteString(global::GetIns()->Cam3Disp,ex.what());
        }
        catch(std::exception ex){

            SetTposition(global::GetIns()->Cam3Disp,10,10);
            SetColor(global::GetIns()->Cam3Disp,"red");
            HalconCpp::WriteString(global::GetIns()->Cam3Disp,ex.what());
        }catch(std::out_of_range ex){

            SetTposition(global::GetIns()->Cam3Disp,10,10);
            SetColor(global::GetIns()->Cam3Disp,"red");
            HalconCpp::WriteString(global::GetIns()->Cam3Disp,ex.what());
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

    SetThreadAffinityMask((HANDLE)_beginthreadex(NULL, 0, Cam12Deal, this, 0, 0),0x2);

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

