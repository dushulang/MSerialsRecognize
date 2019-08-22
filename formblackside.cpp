#include "formblackside.h"
#include "ui_formblackside.h"
#include <QFileDialog>
#include "excv.h"
#include "mcamera.h"

FormBlackSide::FormBlackSide(QWidget *parent, TupleEx tupleEx) :
    QWidget(parent),
    ui(new Ui::FormBlackSide)
{
    ui->setupUi(this);
    MValue = tupleEx;
}

FormBlackSide::~FormBlackSide()
{
    delete ui;
}

void FormBlackSide::on_pushButton_clicked()
{
    global::GetIns()->History.push_back("读取图片");
    using namespace  HalconCpp;
    using namespace  cv;
    QString fileName = QFileDialog::getOpenFileName(NULL,
        tr("Open Image"), "",
        tr("Image File (*.jpg)||Image File (*.bmp)||All Files (*)"));
    try{
       size_t Idx = static_cast<size_t>(MValue.I());
       global::GetIns()->vecImages.at(Idx) = cv::imread(fileName.toLocal8Bit().data());
       HObject hObj;
       MSerials::Mat2Hobj(global::GetIns()->vecImages.at(Idx),hObj);
       MSerials::h_disp_obj(hObj,MValue.Handle());
    }catch(HalconCpp::HException except)
    {
           HTuple ExceptionMessage;
           except.ToHTuple(&ExceptionMessage);
           SetTposition(MValue.Handle(), 0, 1);
           WriteString(MValue.Handle(), ExceptionMessage);
    }
    catch(std::exception ex){
        SetTposition(MValue.Handle(), 0, 1);
        WriteString(MValue.Handle(), ex.what());
    }
    catch(cv::Exception ex){
        SetTposition(MValue.Handle(), 0, 1);
        WriteString(MValue.Handle(), ex.what());
    }
}

void FormBlackSide::on_pushButton_Grab_clicked()
{
    int Idx = MValue.I();
    try {
        cv::Mat G;
        HalconCpp::HObject HObj;
        MCamera::GetIns()->GrabImageA(G,Idx,0);
        MSerials::Mat2Hobj(G,HObj);
        MSerials::h_disp_obj(HObj,MValue.Handle());
    } catch (std::exception ex) {
        SetTposition(MValue.Handle(), 0, 1);
        WriteString(MValue.Handle(), ex.what());
    }
}
