#include "machine.h"
#include "imagedeal.h"
#include "mutex"
#include "global.h"
#include "../PLC_DLL/PLC_DLL/plc_dll.h"


//global

std::mutex mtxImage;

Machine::Machine()
{
    StartThread();
}

struct ImageInfo{
public :
    ImageInfo(cv::Mat *_Image, int _WhichCamera,int _Triggertime){
        std::lock_guard<std::mutex> lck(mtxImage);
        if(_Image) Image = _Image->clone();
        WhichCamera = _WhichCamera;
        TriggerTime = _Triggertime;
    }
    ~ImageInfo(){
        std::lock_guard<std::mutex> lck(mtxImage);
        Image.release();
    }
    cv::Mat Image;// = nullptr;
    int WhichCamera = 0;
    int TriggerTime = 0;
};
std::list<ImageInfo> listImages;

//PLC读取
unsigned int __stdcall PLC(void * lp){
    Machine *pMachine = (Machine*)lp;
    std::cout << "start PLC \n" << pMachine->name() <<std::endl;
    //初始化PLC参数等等
    int Trigger1 = 0,Trigger2 = 0;

bool isOK = false,OisOK = false;
//定义，从400里读取数据就可以了 16位的，如果没增加1就拍依次，比如1 拍依次，2拍两次，拍完毕后返回数据
// -1代表有失败的检测，0代表拍摄完毕
#define PLCREADER 0x400

//返回结果 8 位为一个一共返回32个数据， 占用16个16位，含义分别是筷子的方向 0 代表没检测
#define PLCOUT    480
    //

//通讯plc地址
//通讯plc端口
#define PLCADDERSS "192.168.0.111"
#define PLCPORT     4999


    sleep(500);
    global::GetIns()->History.push_back("PLC连接中...");
    for(;;){
        sleep(0);
            if(!isOK)
            {
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

        std::string ToReadPos = PLCReadStrEX(GetHostByIndex(0),"D",PLCREADER,2);
        std::cout << "Read is"  << ToReadPos <<std::endl;
        int StrLen = ToReadPos.length();






        if(Trigger1>0 && !Machine::GetIns()->EvtCam1.State()){
            Machine::GetIns()->EvtCam1.SetEvent();
            Trigger1--;
        }
        if(Trigger2>0 && !Machine::GetIns()->EvtCam2.State()){
            Machine::GetIns()->EvtCam2.SetEvent();
            Trigger2--;
        }

  }//isOK






    }
}


//检测青面抓图
unsigned int __stdcall Cam1Cap(void * lp){
    Machine *pMachine = (Machine*)lp;
    std::cout << "start1cap\n " << pMachine->name() <<std::endl;
    for(;;){
        ::WaitForSingleObject(Machine::GetIns()->EvtCam1.get(),INFINITE);
        cv::Mat Image;
        //snap
        listImages.push_back(ImageInfo(&Image,0,0));
        Machine::GetIns()->EvtCam1.ResetEvent();
    }
}

//检测青面抓图
unsigned int __stdcall Cam2Cap(void * lp){
    Machine *pMachine = (Machine*)lp;
    std::cout << "start2cap\n " << pMachine->name() <<std::endl;
    for(;;){
        ::WaitForSingleObject(Machine::GetIns()->EvtCam2.get(),INFINITE);
        cv::Mat Image;
        //snap
        listImages.push_back(ImageInfo(&Image,1,0));
        Machine::GetIns()->EvtCam2.ResetEvent();
    }
}

//检测青面
unsigned int __stdcall Cam12Deal(void * lp){
    Machine *pMachine = (Machine*)lp;
    std::cout << "start12Deal\n " << pMachine->name() <<std::endl;
    for(;;){
        sleep(1);
        for(;!listImages.empty();){
            cv::Mat *Image = &listImages.front().Image;
            int WhichCamera = listImages.front().WhichCamera;
            int TriggerTime = listImages.front().TriggerTime;
            try{
                int Res = MSerials::CheckUpSide(Image);
            }catch(std::exception ex){
                int Res = 0;
            }

            listImages.pop_front();
        }
    }
}

//检测花纹
unsigned int __stdcall Cam3(void * lp){
    Machine *pMachine = (Machine*)lp;
    std::cout << "start3deal\n " << pMachine->name() <<std::endl;
    for(;;){
        ::WaitForSingleObject(Machine::GetIns()->EvtCam3.get(),INFINITE);

        Machine::GetIns()->EvtCam3.ResetEvent();
    }
}

void Machine::OnTimer(){




}


void Machine::StartThread(){
    //0xC == 001100
    SetThreadAffinityMask((HANDLE)_beginthreadex(NULL, 0, Cam1Cap, this, 0, 0),0x4);
    //110000
    SetThreadAffinityMask((HANDLE)_beginthreadex(NULL, 0, Cam2Cap, this, 0, 0),0x8);

    SetThreadAffinityMask((HANDLE)_beginthreadex(NULL, 0, Cam12Deal, this, 0, 0),0xC);
    //10
    SetThreadAffinityMask((HANDLE)_beginthreadex(NULL, 0, Cam3, this, 0, 0),0x2);

    SetThreadAffinityMask((HANDLE)_beginthreadex(NULL, 0, PLC, this, 0, 0),0x1);

    gtime_ID = timeSetEvent(1, 1, (LPTIMECALLBACK)TimeEvent, 1, TIME_PERIODIC);
    if (gtime_ID == NULL)
    {
        printf("定时器设定失败\n");
        return;
    }
}

