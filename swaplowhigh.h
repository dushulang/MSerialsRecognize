#ifndef SWAPLOWHIGH_H
#define SWAPLOWHIGH_H

#include <string>

#include <QString>

namespace MSerials {

    QString  Decode(std::string str){
        return QString::fromLocal8Bit(str.c_str());
    }

    //功能，高低字节交换，如果是奇数，则填充最高位0 位
    static void SwapLowAndHigh(std::string &Input,std::string &Output, bool FillZero = true)
    {
        using namespace std;
        std::string High,Low;
        int Len = static_cast<int>(Input.length());
        if(0 == Len) {Output.clear(); return;}
        if(1 == (Len%2)){
            if(FillZero){
                Output = "0" + Input;
            }else {
                Output = "1" + Input;
            }
            int HalfLen = (1+Len)/2;


            High = Output.substr(0,size_t(HalfLen));
            Low = Output.substr(size_t(HalfLen));
            Output = Low + High;
        }
        else {
            Output = Input;
            int HalfLen = Len/2;
            High = Output.substr(0,size_t(HalfLen));
            Low = Output.substr(size_t(HalfLen));
            Output = Low + High;
        }
    }

};

#endif // SWAPLOWHIGH_H
