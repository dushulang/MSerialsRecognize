#include "formblackside.h"
#include "ui_formblackside.h"
#include <QFileDialog>
#include "excv.h"
#include "mcamera.h"
#include "preference.h"
#include "machine.h"
#include "imagedeal.h"
#include <QtNetwork/QHostAddress>
//QHostAddress test;

FormBlackSide::FormBlackSide(QWidget *parent, TupleEx tupleEx) :
    QWidget(parent),
    ui(new Ui::FormBlackSide)
{
    ui->setupUi(this);
    MValue = tupleEx;


    if(FLIPIMAGE[MValue.I()]){
        ui->pushButton_FlipImage->setText(QString::fromLocal8Bit("图片翻转180度"));
    }
    else {
        ui->pushButton_FlipImage->setText(QString::fromLocal8Bit("图片翻转0度"));
    }

    ui->lineEdit_IP->setText(Preference::GetIns()->prj->IP);
    ui->lineEdit_Port->setText(QString::number(Preference::GetIns()->prj->Port ));

    int Idx = MValue.I();
    ui->lineEdit_ThresVar->setText(QString::number(THRESHOLD[Idx]));

    ui->lineEdit_ChopStickArea->setText(QString::number(CHOPSTICKAREA[Idx]));

    ui->lineEdit_MeanVar->setText(QString::number(MEANVAR[Idx]));

    ui->lineEdit_Factior->setText(QString::number(IMAGEFACTOR[Idx]));

    ui->lineEdit_ErsionRadius->setText(QString::number(ERSIONRADIUS[Idx]));

    ui->lineEdit_ThresBias->setText(QString::number(THRESHOLDBIAS[Idx]));



    ui->lineEdit_d->setText(QString::number(DOUBLEFILTERD[Idx]));
    ui->lineEdit_SC->setText(QString::number(DOUBLEFILTERSC[Idx]));
    ui->lineEdit_SS->setText(QString::number(DOUBLEFILTERSS[Idx]));
    ui->lineEdit_BlockSize->setText(QString::number(DOUBLEFILTERBS[Idx]));

    if(1 == MValue.I()){
        ui->pushButton_SAVEIMAGE->setVisible(false);
    }
    if(ISSAVEIMAGE){
        ui->pushButton_SAVEIMAGE->setStyleSheet("background-color:rgb(0,255,0)");

    }
    else
    {
        ui->pushButton_SAVEIMAGE->setStyleSheet("background-color:rgb(255,0,0)");

    }

    ui->groupBox->setVisible(false);
    ui->pushButton_2->setVisible(false);
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
        printf_s("Index is %d",Idx);
        global::GetIns()->vecImages.at(Idx) = cv::imread(fileName.toLocal8Bit().data());
        // cv::flip(global::GetIns()->vecImages.at(Idx),global::GetIns()->vecImages.at(Idx),-1);
        if(global::GetIns()->vecImages.at(Idx).empty()) return;
        HObject hObj;
        MSerials::Mat2Hobj(global::GetIns()->vecImages.at(Idx),hObj);
        MSerials::h_disp_obj(hObj,MValue.Handle());

#ifdef TEST
        Machine::GetIns()->ImageIn(global::GetIns()->vecImages.at(Idx),Idx);
#endif
    }catch(HalconCpp::HException except)
    {
        HTuple ExceptionMessage;
        except.ToHTuple(&ExceptionMessage);
        SetTposition(MValue.DispHandle, 0, 1);
        WriteString(MValue.DispHandle, ExceptionMessage);
    }
    catch(std::exception ex){
        SetTposition(MValue.DispHandle, 0, 1);
        HalconCpp::WriteString(MValue.DispHandle, ex.what());
    }
    catch(cv::Exception ex){
        SetTposition(MValue.DispHandle, 0, 1);
        WriteString(MValue.DispHandle, ex.what());
    }
    catch(std::out_of_range ex){
        SetTposition(MValue.DispHandle, 0, 1);
        WriteString(MValue.DispHandle, ex.what());
    }


}

void FormBlackSide::on_pushButton_Grab_clicked()
{

    int Idx = MValue.I();
    try {
        cv::Mat G;
        MCamera::GetIns()->GrabImageSDK(G,Idx,FLIPIMAGE[Idx]);
        HalconCpp::HObject HObj;
        //        MCamera::GetIns()->GrabImageA3(G,Idx,FLIPIMAGE[Idx]);
        global::GetIns()->vecImages.at(Idx) = G.clone();
        MSerials::Mat2Hobj(G,HObj);
        MSerials::h_disp_obj(HObj,MValue.Handle());
        char msg[256];
        sprintf_s(msg,"图像高度 %d 图像宽度 %d",G.cols,G.rows);
        global::GetIns()->History.push_back(msg);
    } catch (std::exception ex) {
        SetTposition(MValue.Handle(), 0, 1);
        WriteString(MValue.Handle(), ex.what());
    }catch(std::out_of_range ex){

        SetTposition(MValue.Handle(), 0, 1);
        WriteString(MValue.Handle(), ex.what());
    }catch(cv::Exception ex){

        SetTposition(MValue.Handle(), 0, 1);
        WriteString(MValue.Handle(), ex.what());
    }
    catch(HalconCpp::HException ex){
        HTuple Error;
        ex.ToHTuple(&Error);
        SetTposition(MValue.Handle(), 0, 1);
        WriteString(MValue.Handle(), Error[0].S().Text());
    }
}

void FormBlackSide::on_pushButton_FlipImage_clicked()
{
    if(!FLIPIMAGE[MValue.I()]){
        FLIPIMAGE[MValue.I()] = 1;
        ui->pushButton_FlipImage->setText(QString::fromLocal8Bit("图片翻转180度"));
    }
    else {
        FLIPIMAGE[MValue.I()] = 0;
        ui->pushButton_FlipImage->setText(QString::fromLocal8Bit("图片翻转0度"));
    }
}

void FormBlackSide::on_pushButton_SaveImage_clicked()
{
    try {
        size_t Idx = MValue.I();
        HalconCpp::HObject ho_Image;
        MSerials::Mat2Hobj(global::GetIns()->vecImages.at(Idx),ho_Image);
        QDateTime time = QDateTime::currentDateTime();
        QString ImageFile = PROJECTNAME + "/" + time.toString("yyyy-MM-dd-HH-mm-ss-zzz-") + "Cam" + QString::number(1+MValue.I()) + ".jpg";
        HalconCpp::WriteImage(ho_Image,"jpg",0,ImageFile.toLocal8Bit().data());
    }catch(cv::Exception ex){
        SetTposition(MValue.Handle(), 0, 1);
        WriteString(MValue.Handle(), ex.what());
    } catch (std::exception ex) {
        SetTposition(MValue.Handle(), 0, 1);
        WriteString(MValue.Handle(), ex.what());
    }catch(std::out_of_range ex){
        SetTposition(MValue.Handle(), 0, 1);
        WriteString(MValue.Handle(), ex.what());
    }
    catch(HalconCpp::HException except)
    {
        HTuple ExceptionMessage;
        except.ToHTuple(&ExceptionMessage);
        SetTposition(MValue.Handle(), 0, 1);
        WriteString(MValue.Handle(), "保存图片失败");
    }
}

void FormBlackSide::on_pushButton_SavePara_clicked()
{
    QHostAddress Host;
    auto IpAdress = ui->lineEdit_IP->text();
    if(!Host.setAddress(IpAdress) ){
        QMessageBox::information(nullptr,"",QString::fromLocal8Bit("IP设置错误，例如 192.168.0.1"));
        return;
    }

    Preference::GetIns()->prj->IP = IpAdress;
    auto PortNum = ui->lineEdit_Port->text().toInt();
    if(PortNum < 1 || PortNum > 65535){
        QMessageBox::information(nullptr,"",QString::fromLocal8Bit("端口设置错误，应该小于65535"));
        return;
    }
    Preference::GetIns()->prj->Port = PortNum;

    //   Preference::GetIns()->IP = IpAdress;


    int Idx = MValue.I();
    double var = 0.0;
    var = ui->lineEdit_ThresVar->text().toDouble();
    if(var <1) return;
    THRESHOLD [Idx] = var;
    var = ui->lineEdit_ChopStickArea->text().toDouble();
    if(var <1) return;
    CHOPSTICKAREA[Idx] = var;
    var = ui->lineEdit_MeanVar->text().toDouble();
    if(var <1) return;
    MEANVAR[Idx] = var;
    var = ui->lineEdit_Factior->text().toDouble();
    if(var <0) return;
    IMAGEFACTOR[Idx] = var;
    var = ui->lineEdit_ErsionRadius->text().toDouble();
    if(var <1) return;
    ERSIONRADIUS[Idx] = var;
    var = ui->lineEdit_ThresBias->text().toDouble();
    THRESHOLDBIAS[Idx] = var;

    var = ui->lineEdit_d->text().toDouble();
    if(var <1) return;
    DOUBLEFILTERD[Idx] = var;
    var = ui->lineEdit_SC->text().toDouble();
    if(var <1) return;
    DOUBLEFILTERSC[Idx] = var;
    var = ui->lineEdit_SS->text().toDouble();
    if(var <1) return;
    DOUBLEFILTERSS[Idx] = var;
    var = ui->lineEdit_BlockSize->text().toDouble();
    if(var <1) return;
    DOUBLEFILTERBS[Idx] = var;

    SAVEPRJPARA;
}

void FormBlackSide::on_pushButton_TestImage_clicked()
{
    int Idx = MValue.I();
    try{
        std::string result = "";
        int res = MSerials::CheckUpSideOld(global::GetIns()->vecImages.at(Idx), MValue.Handle(),THRESHOLD[Idx], CHOPSTICKAREA[Idx] ,
                                           0.6,
                                           DOUBLEFILTERD[Idx],
                                           DOUBLEFILTERSC[Idx],
                                           DOUBLEFILTERSS[Idx],
                                           THRESHOLDBIAS[Idx],
                                           DOUBLEFILTERBS[Idx],
                                           &result);
        SetColor(MValue.Handle(),"green");
        SetTposition(MValue.Handle(), 0, 1);

        switch (res) {
        case UP:WriteString(MValue.Handle(), "上");break;
        case LEFT:WriteString(MValue.Handle(), "左");break;
        case DOWN:WriteString(MValue.Handle(), "下");break;
        case RIGHT:WriteString(MValue.Handle(), "右");break;
        default:break;

        }

#if  0

        MSerials::CheckUpSide(&global::GetIns()->vecImages.at(Idx),MValue.Handle(),
                              THRESHOLD [Idx],
                              CHOPSTICKAREA[Idx],
                              MEANVAR[Idx],
                              IMAGEFACTOR[Idx],
                              ERSIONRADIUS[Idx],
                              THRESHOLDBIAS[Idx]);
#endif
    }catch(cv::Exception ex){
        SetColor(MValue.Handle(),"red");
        SetTposition(MValue.Handle(), 0, 1);
        WriteString(MValue.Handle(), ex.what());
    }
    catch(HalconCpp::HException except)
    {
        HTuple ExceptionMessage;
        except.ToHTuple(&ExceptionMessage);
        SetColor(MValue.Handle(),"red");
        SetTposition(MValue.Handle(), 0, 1);
        WriteString(MValue.Handle(), ExceptionMessage);
    }catch(std::out_of_range ex){
        SetColor(MValue.Handle(),"red");
        SetTposition(MValue.Handle(), 0, 1);
        WriteString(MValue.Handle(), ex.what());
    }
    catch(std::exception ex){
        SetColor(MValue.Handle(),"red");
        SetTposition(MValue.Handle(), 0, 1);
        WriteString(MValue.Handle(), ex.what());
    }

}

void FormBlackSide::on_pushButton_SAVEIMAGE_clicked()
{
    //  QString ImageFile = PROJECTNAME + "/" + time.toString("yyyy-MM-dd-HH-mm-ss-zzz-") + "Cam" + QString::number(1+MValue.I()) + ".jpg";
    //  HalconCpp::WriteImage(ho_Image,"jpg",0,ImageFile.toLocal8Bit().data());
    if(!ISSAVEIMAGE){
        ui->pushButton_SAVEIMAGE->setStyleSheet("background-color:rgb(0,255,0)");
        ISSAVEIMAGE = 1;
    }
    else
    {
        ui->pushButton_SAVEIMAGE->setStyleSheet("background-color:rgb(255,0,0)");
        ISSAVEIMAGE = 0;
    }
    SAVEPRJPARA;
}

void FormBlackSide::on_pushButton_2_clicked()
{
    exit(0);
}
