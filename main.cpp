#include "mainwindow.h"
#include <QApplication>
#include "machine.h"
#include "preference.h"
#include "mcamera.h"
#include "imagedeal.h"
#include <QDir>
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"../third_party\\halcon17\\halconcpp.lib")
//#pragma comment(lib,"../ChongChuang12/x64/Release/MSerialsCV.lib")
#pragma comment(lib,"../third_party/crypt/vc15/cryptlib.lib")
#pragma comment(lib,"../third_party/opencv4.0.1/x64/vc15/lib/opencv_world401.lib")
//添加PLC通讯
#pragma comment(lib,"../PLC_DLL/x64/Release/PLC_DLL.lib")
//添加相机
#pragma comment(lib,"../ChopStickDetect\\x64\\ReleaseX64\\MSerialsCamera.lib")

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

int main(int argc, char *argv[])
{ 
    Init();
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
