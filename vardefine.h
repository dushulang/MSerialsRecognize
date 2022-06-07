#ifndef VARDEFINE_H
#define VARDEFINE_H

#include <Windows.h>
#include <mutex>
#include <QString>
#include "preference.h"
#include "DXGI.h"
#include "HalconCpp.h"

#define ONE_CAMERA

#define STR_COM         Preference::GetIns()->prj->COM
#define STR_BAUND       Preference::GetIns()->prj->BAUND
#define STR_PARITY      Preference::GetIns()->prj->PARITY
#define STR_DATA        Preference::GetIns()->prj->DATA
#define STR_STOP        Preference::GetIns()->prj->STOP

//文本开始
#define STX             0x2
//文本结束
#define ETX             0x3
//发送结束
#define EOT             0x4
//查询
#define ENQ             0x5
//肯定响应
#define ACK             0x6
//换行
#define LF              0xA
//清除后，PLC不会发任何数据
#define CL              0xC
//回车
#define CR              0xD
//否定相应
#define NAK             0x15



struct TupleEx{
    private:
     int i = 0;

    public:
     HalconCpp::HTuple DispHandle = 0;
     TupleEx(int idx =0 , HalconCpp::HTuple winHandle = 0 ){
        i = idx;
        DispHandle = winHandle;
     }

    std::string Content;
    int I(){return i;}
    HalconCpp::HTuple &Handle(){
        try{
        if(0 == DispHandle.Type()) throw std::exception("null disphandle");
        }catch (std::exception ex){
            throw std::exception("null disphandle");
        }
        catch(HalconCpp::HException ex)
        {
            throw std::exception("null disphandle");
        }
        return DispHandle;
    }
};



#endif // VARDEFINE_H
