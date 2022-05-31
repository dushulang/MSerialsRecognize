#include "mainwindow.h"
#include <QApplication>
#include "machine.h"
#include "preference.h"
#include "mcamera.h"
#include "imagedeal.h"
#include <QDir>
#include <cryptdialog.h>
#include <caffe.h>
//#pragma comment(lib,"F:/Program Files (x86)/Windows Kits/10/Lib/10.0.17763.0/ucrt/x64/ucrt.lib")
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"D:/ProjectFile/third_party/halcon18/halconcpp.lib")
//#pragma comment(lib,"../ChongChuang12/x64/Release/MSerialsCV.lib")
#pragma comment(lib,"D:/ProjectFile/third_party/crypt/vc15/cryptlib.lib")
#pragma comment(lib,"D:/ProjectFile/third_party/opencv4.0.1/x64/vc15/lib/opencv_world401.lib")
//添加PLC通讯
#pragma comment(lib,"D:/ProjectFile/PLC_DLL/x64/Release/PLC_DLL.lib")
//添加相机
#pragma comment(lib,"D:/ProjectFile/ChopStickDetect\\x64\\ReleaseX64\\MSerialsCamera.lib")

//读取配置文件
//初始化相机
//网口不需要初始化，直接连接
void Init(){
    //

    Preference::GetIns()->sys->SetFilePos(QString("Sys.ini"));
    QString ProjectName = PROJECTNAME;
    if(ProjectName.length() < 1){
        ProjectName = "TMP";
    }
    QDir D;D.mkdir(ProjectName.toLocal8Bit());
    D.mkdir(ProjectName.toLocal8Bit() + "/OK");
    D.mkdir(ProjectName.toLocal8Bit() + "/NG");
    Preference::GetIns()->prj->SetFilePos(ProjectName + "/prj.ini");
    try{
    MSerials::load_train_data(ProjectName.toLocal8Bit().data());
    }catch(cv::Exception ex){

    }catch(std::exception ex){

    }

}

//#define USECRYPT

int main(int argc, char *argv[])
{ 
    SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
    Init();
    Caffe caffe = Caffe("Load");
    QApplication a(argc, argv);
#ifdef USECRYPT
    CryptDialog CryptDlg;
    if(false == CryptDlg.Init())
    CryptDlg.exec();
#endif
    MainWindow w;
    w.show();
    return a.exec();
}
