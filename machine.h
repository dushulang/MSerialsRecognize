#ifndef MACHINE_H
#define MACHINE_H

#include <windows.h>
#include "../PLC_DLL/PLC_DLL/plc_dll.h"
#include "mserialssystem.h"
#include "opencv.hpp"

//指定在CPU哪个核上运行
// SetProcessAffinityMask

#include <thread>
#include <MMSystem.h>
#include "mserialssystem.h"

class Machine
{
private:
    HANDLE NewWait;
    int gtime_ID = -1;
    DWORD dwThreadId[16];
    //相机1结果，相机2结果，相机3结果
    std::string Cam1Result, Cam2Result,Cam3Result;

public:
    MEvent EvtCam1,EvtCam2,EvtCam12Deal,EvtCam3,EvtPLC;


private:
    Machine();
    ~Machine(){UnregisterWaitEx(NewWait, INVALID_HANDLE_VALUE);timeKillEvent(gtime_ID);}
    void StartThread();
    void OnTimer();
    static void TimeEvent(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2){
        Machine::GetIns()->OnTimer();
    }
public:
    //非大并发情况下，直接在main 里初始化，否则可能产生重复的bug
    static Machine* GetIns(){static Machine l;  return &l;}

    void ImageIn1(cv::Mat& ,int);
    const char *name(){return "Machine";}




    //对PLC检测进行观测


};

#endif // MACHINE_H
