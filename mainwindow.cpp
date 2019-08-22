#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "global.h"
#include <QMenu>
#include <qDebug>
#include "swaplowhigh.h"
#include "machine.h"
#include <QTimer>
#include "formblackside.h"
#include "formrecognize.h"
#include "mcamera.h"
#include <QLCDNumber>
#include <QTime>

using namespace MSerials;




MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //防止程序多开
    HANDLE m_hMutex  =  ::CreateMutexW(NULL, FALSE,  L"PUNCH_____2019.6.8" );
    //  检查错误代码
    if  (GetLastError()  ==  ERROR_ALREADY_EXISTS)  {
     QMessageBox::warning(this,QString::fromLocal8Bit("Error"),QString::fromLocal8Bit("请勿多开程序!如果频繁出现，请重启电脑"));
     CloseHandle(m_hMutex);
     m_hMutex  =  NULL;
     exit(0);
    }




    static auto Min = [=](){showMinimized();};
    ui->menuBar->addAction(Decode("X"),this,exit);
    ui->menuBar->addAction(Decode("-"),this,Min);

    QMenu *file_menu=new QMenu(Decode("帮助"));
    file_menu->addAction(Decode("打开"));//可用返回的QAction 连接槽函数
    file_menu->addAction(Decode("退出"),this,exit,QKeySequence("Ctrl+E"));//失败
    file_menu->addSeparator();
    file_menu->addSection("Section");//貌似无效果
    ui->menuBar->addMenu(file_menu);

    showFullScreen();

    #define IH   964
    #define IW  1292
    int nWndWidth = ui->widgetCam1->width();
    int nWndHeight= ui->widgetCam1->height();    
    HalconCpp::SetCheck("~father");
    HalconCpp::OpenWindow(0, 0, nWndWidth, nWndHeight, (Hlong)ui->widgetCam1->winId(), "visible", "", &global::GetIns()->Cam1Disp);
    HalconCpp::SetCheck("father");

    nWndWidth = ui->widgetCam2->width();
    nWndHeight= ui->widgetCam2->height();
    HalconCpp::SetCheck("~father");
    HalconCpp::OpenWindow(0, 0, nWndWidth, nWndHeight, (Hlong)ui->widgetCam2->winId(), "visible", "", &global::GetIns()->Cam2Disp);
    HalconCpp::SetCheck("father");

    nWndWidth = ui->widgetCam3->width();
    nWndHeight= ui->widgetCam3->height();
    HalconCpp::SetCheck("~father");
    HalconCpp::OpenWindow(0, 0, nWndWidth, nWndHeight, (Hlong)ui->widgetCam3->winId(), "visible", "", &global::GetIns()->Cam3Disp);
    HalconCpp::SetCheck("father");

    ui->tabWidget->clear();
    ui->tabWidget->insertTab(0,new FormBlackSide(0,TupleEx(0,global::GetIns()->Cam1Disp)),MSerials::Decode("相机1"));
    ui->tabWidget->insertTab(1,new FormBlackSide(0,TupleEx(0,global::GetIns()->Cam2Disp)),MSerials::Decode("相机2"));
    ui->tabWidget->insertTab(2,new FormRecognize(0,TupleEx(0,global::GetIns()->Cam3Disp)),MSerials::Decode("相机3"));


    QLCDNumber *液晶时间显示 = ui->lcdNumber;// new QLCDNumber(ui->widget_LCD);
    液晶时间显示->setDigitCount(10);
    液晶时间显示->setMode(QLCDNumber::Dec);
    液晶时间显示->setSegmentStyle(QLCDNumber::Filled);
    液晶时间显示->setStyleSheet("QLCDNumber{background:rgba(64,30,66);color:white}");

    QTimer *timer_io = new QTimer();
    //设置定时器每个多少毫秒发送一个timeout()信号
    timer_io->setInterval(200);
    connect(timer_io, &QTimer::timeout, [=]() {
        //获取系统当前时间
       // QTime time = QTime::currentTime();
         QDateTime time = QDateTime::currentDateTime();
        //设置晶体管控件QLCDNumber上显示的内容
        液晶时间显示->display(time.toString("HH:mm:ss"));
        size_t InfoSize = global::GetIns()->History.size();
        static size_t OldInfoSize = InfoSize;
        if(OldInfoSize != InfoSize){
            std::string Show;
            for(auto &str:global::GetIns()->History){
                Show += (str + "\r\n");
            }
            ui->textEdit_History->setText(MSerials::Decode(Show.c_str()));
        }
        {
            static std::mutex mtx;
            std::lock_guard<std::mutex> lck(mtx);
            while(global::GetIns()->History.size() > 20) global::GetIns()->History.pop_front();
        }
        OldInfoSize = InfoSize;
    });
    timer_io->start();

    ui->widgetlLabel->setStyleSheet("QWidget{background:rgba(64,30,66);color:white}");
    ui->label->setStyleSheet("QLabel{background:rgba(64,30,66);color:white;fontsize:64}");
    ui->label->setText(MSerials::Decode("聚新自动化"));
    global::GetIns()->History.push_back("相机命名为1 2...否则打不开相机");
    MCamera::GetIns()->Init();
    Machine::GetIns()->name();
}

void MainWindow::RefreshTable(std::vector<std::pair<int,int>> Data){

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::OnInitDialog(){


}
