#include "formrecognize.h"
#include "ui_formrecognize.h"
#include <QFileDialog>
#include "preference.h"
#include "HalconCpp.h"
#include "imagedeal.h"
#include "mcamera.h"
#include "QMessageBox.h"
#include <QDesktopServices>
#include <QUrl>

using namespace HalconCpp;


void FormRecognize::DrawRectangle2Ex(int Idx)
{
    size_t imageIdx = 2;
    HObject HObj;
    try{
        MSerials::Mat2Hobj(global::GetIns()->vecImages.at(imageIdx),HObj);
        HTuple hv_Idx = Idx,hv_winHandle = MValue.Handle();
        HTuple hv_PI = 3.1415926535;
        HTuple hv_RowIn = 0,hv_ColumnIn = 0, hv_AngleIn = 0, hv_Len1In = 0,  hv_Len2In = 0,Row= 0,Col=0,Len2=0;
        HTuple hv_Num = SAMPLENUM, hv_Width, hv_Height, hv_OrgX, hv_OrgY, hv_OrgPhi;
        HTuple hv_ReXIn, hv_ReYIn, hv_RePhiIn,hv_OutX, hv_OutY, hv_OutRePhi, hv_OutAbPhi;
        HObject ROI;
        ISDRAW = 1;

        GetImageSize(HObj,&hv_Width,&hv_Height);
        MSerials::h_disp_obj(HObj,hv_winHandle);
        if(0 != Idx){
            if (HDevWindowStack::IsOpen())
              SetDraw(HDevWindowStack::GetActive(),"margin");
            if (HDevWindowStack::IsOpen())
              SetColor(HDevWindowStack::GetActive(),"yellow");
            SetTposition(HDevWindowStack::GetActive(), 0, 1);
            WriteString(HDevWindowStack::GetActive(), "点击右键或者长按触摸屏幕等待十字消失确认");
            DrawRectangle2(HDevWindowStack::GetActive(), &hv_RowIn, &hv_ColumnIn, &hv_AngleIn, &hv_Len1In,
            &hv_Len2In);
        //模拟存储参数
        MSerials::FindTrackLine(HObj, hv_RowIn, hv_ColumnIn, hv_AngleIn, hv_Len1In, hv_Len2In,
            hv_Num, hv_Width, hv_Height, &hv_OrgX, &hv_OrgY, &hv_OrgPhi);
        DispArrow(HDevWindowStack::GetActive(), hv_OrgY, hv_OrgX, hv_OrgY-(120*(hv_OrgPhi.TupleSin())),
            hv_OrgX+(120*(hv_OrgPhi.TupleCos())), 2);

        if (0 != (hv_OrgPhi<0))
        {
          hv_OrgPhi= (2*hv_PI)+hv_OrgPhi;
        }
        ROW[Idx] = hv_RowIn[0].D();
        COLUMN[Idx] = hv_ColumnIn[0].D();
        ABANGLE[Idx] = hv_AngleIn[0].D();
        REANGLE[Idx] = hv_OrgPhi[0].D();
        LEN1[Idx] = hv_Len1In[0].D();
        LEN2[Idx] = hv_Len2In[0].D();
        }else {

        }
        SAVEPRJPARA;
    }catch(HalconCpp::HException except)
    {
        HTuple ex;
        except.ToHTuple(&ex);
        SetTposition(MValue.Handle(), 0, 1);
        WriteString(MValue.Handle(), ex);
    }catch(std::exception ex){
        SetTposition(MValue.Handle(), 0, 1);
        WriteString(MValue.Handle(), ex.what());
    }
    ISDRAW = 0;
}


void FormRecognize::RefreshTable(std::vector<std::pair<double,double>> Data){
    ui->tableWidget->setRowCount(Data.size());
    ui->tableWidget->setColumnCount(2);
    QStringList StrList;
    StrList << QString::fromLocal8Bit("组1偏移值") << QString::fromLocal8Bit("组2偏移值");
    ui->tableWidget->setHorizontalHeaderLabels(StrList);
    for(size_t r = 0; r<(Data.size());r++){
        auto Item = ui->tableWidget->item(r,0);//
        if(nullptr == Item){
            auto item = new QTableWidgetItem(QString::number(Data.at(r).first));
            ui->tableWidget->setItem(r,0,item);
        }
        else{
            Item->setText(QString::number(Data.at(r).first));
        }
        auto Item1 = ui->tableWidget->item(r,1);//
        if(nullptr == Item1){
            auto item = new QTableWidgetItem(QString::number(Data.at(r).second));
            ui->tableWidget->setItem(r,1,item);
        }
        else{
            Item1->setText(QString::number(Data.at(r).second));
        }

    }
}

FormRecognize::FormRecognize(QWidget *parent, TupleEx tupleEx) :
    QWidget(parent),
    ui(new Ui::FormRecognize)
{
    ui->setupUi(this);
    MValue = tupleEx;
    std::vector<std::pair<double,double>> Tmp;
    for(int i = 0; i < CHECKNUM; i++){
        Tmp.push_back(std::pair<double,double>(LINEOFFSET[i],LINEOFFSET1[i]));
    }
    RefreshTable(Tmp);

    ui->lineEdit_CheckNum->setText(QString::number(CHECKNUM));
    ui->lineEdit_CheckWidth->setText(QString::number(LEN1[0]));
    ui->lineEdit_CheckHeight->setText(QString::number(LEN2[0]));

    connect(ui->lineEdit_CheckNum,&QLineEdit::textChanged,[=](){
        int i = ui->lineEdit_CheckNum->text().toInt();
        if(i < 0 ){
            i = 0;
        }
        else if(i>=4*MAX_VOL){
            i = 4*MAX_VOL-1;
        }
        CHECKNUM = i;
        ui->lineEdit_CheckNum->setText(QString::number(i));
        SAVEPRJPARA;
        std::vector<std::pair<double,double>> Tmp;
        for(int i = 0; i < CHECKNUM; i++){
            Tmp.push_back(std::pair<double,double>(LINEOFFSET[i],LINEOFFSET1[i]));
        }
        RefreshTable(Tmp);
    });


    connect(ui->lineEdit_CheckWidth,&QLineEdit::textChanged,[=](){
        LEN1[0]= ui->lineEdit_CheckWidth->text().toInt();
        if(LEN1[0] < 1 ){LEN1[0] = 1;}
        LEN2[0] = 8*LEN1[0];
        ui->lineEdit_CheckWidth->setText(QString::number(LEN1[0]));
        ui->lineEdit_CheckHeight->setText(QString::number(LEN2[0]));
        SAVEPRJPARA;
    });

    connect(ui->lineEdit_CheckHeight,&QLineEdit::textChanged,[=](){
        LEN2[0]= ui->lineEdit_CheckHeight->text().toInt();
        if(LEN2[0] < 8 ){LEN2[0] = 8;}
        ui->lineEdit_CheckHeight->setText(QString::number(LEN2[0]));
        SAVEPRJPARA;
    });

    connect(ui->tableWidget,&QTableWidget::itemChanged,[=](QTableWidgetItem * item){
        double v = item->text().toDouble();
        int c = item->column();
        int r = item->row();
        if(c == 0){
            LINEOFFSET[r] = v;
            try{
            HalconCpp::HObject ho_Image;
            MSerials::Mat2Hobj(global::GetIns()->vecImages.at(2),ho_Image);
            if (HDevWindowStack::IsOpen())
              MSerials::h_disp_obj(ho_Image, HDevWindowStack::GetActive());
            int Idx = 1;
            std::vector<double> Bias;
            for(int i = 0; i<CHECKNUM;i++){
                Bias.push_back(LINEOFFSET[i]);
            }
            MSerials::CheckLogo(&global::GetIns()->vecImages.at(2),ROW[Idx],COLUMN[Idx],ABANGLE[Idx],LEN1[Idx],LEN2[Idx],Bias,LEN1[0],LEN2[0],r);
            }
            catch (std::exception ex) {
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
                           WriteString(MValue.Handle(), ExceptionMessage);
                    }

        }
        else {
            LINEOFFSET1[r] = v;
            try{
            HalconCpp::HObject ho_Image;
            MSerials::Mat2Hobj(global::GetIns()->vecImages.at(2),ho_Image);
            if (HDevWindowStack::IsOpen())
              MSerials::h_disp_obj(ho_Image, HDevWindowStack::GetActive());
            int Idx = 2;
            std::vector<double> Bias;
            for(int i = 0; i<CHECKNUM;i++){
                Bias.push_back(LINEOFFSET1[i]);
            }
            MSerials::CheckLogo(&global::GetIns()->vecImages.at(2),ROW[Idx],COLUMN[Idx],ABANGLE[Idx],LEN1[Idx],LEN2[Idx],Bias,LEN1[0],LEN2[0],r);
            }
            catch (std::exception ex) {
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
                           WriteString(MValue.Handle(), ExceptionMessage);
           }



        }
        SAVEPRJPARA;
        std::vector<std::pair<double,double>> Tmp;
        for(int i = 0; i < CHECKNUM; i++){
            Tmp.push_back(std::pair<double,double>(LINEOFFSET[i],LINEOFFSET1[i]));
        }
        RefreshTable(Tmp);
    });


       // connect(ui->t)
}

FormRecognize::~FormRecognize()
{
    delete ui;
}

void FormRecognize::on_pushButton_ReadImage_clicked()
{
    using namespace  HalconCpp;
    using namespace  cv;
    QString fileName = QFileDialog::getOpenFileName(NULL,
        tr("Open Image"), "",
        tr("Image File (*.jpg)||Image File (*.bmp)||All Files (*)"));
    try{
       size_t Idx = 2;//static_cast<size_t>(MValue.I());
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

void FormRecognize::on_pushButton_GrabImage_clicked()
{
    int Idx = 2;
    try {
        cv::Mat G;
        HalconCpp::HObject HObj;
        MCamera::GetIns()->GrabImageA(G,Idx,0);
        MSerials::Mat2Hobj(G,HObj);
        MSerials::h_disp_obj(HObj,MValue.Handle());
    } catch (std::exception ex) {
        SetTposition(MValue.Handle(), 0, 1);
        WriteString(MValue.Handle(), ex.what());
    }catch(std::out_of_range ex){
        SetTposition(MValue.Handle(), 0, 1);
        WriteString(MValue.Handle(), ex.what());
    }
}

void FormRecognize::on_pushButton_TestImage_clicked()
{
    try {
        HalconCpp::HObject ho_Image;
        MSerials::Mat2Hobj(global::GetIns()->vecImages.at(2),ho_Image);
        if (HDevWindowStack::IsOpen())
          MSerials::h_disp_obj(ho_Image, HDevWindowStack::GetActive());
        int Idx = 1;
        std::vector<double> Bias;
        for(int i = 0; i<CHECKNUM;i++){
            Bias.push_back(LINEOFFSET[i]);
        }
        std::vector <double> res = MSerials::CheckLogo(&global::GetIns()->vecImages.at(2),ROW[Idx],COLUMN[Idx],ABANGLE[Idx],LEN1[Idx],LEN2[Idx],Bias,LEN1[0],LEN2[0]);
        Idx = 2;
        Bias.clear();
        for(int i = 0; i<CHECKNUM;i++){
             Bias.push_back(LINEOFFSET1[i]);
        }
        std::vector <double> res1 = MSerials::CheckLogo(&global::GetIns()->vecImages.at(2),ROW[Idx],COLUMN[Idx],ABANGLE[Idx],LEN1[Idx],LEN2[Idx],Bias,LEN1[0],LEN2[0]);
        for(auto r : res){if(r > 0) std::cout << " OK" ; else std::cout << " NG";}
        std::cout << " " << std::endl;
        for(auto r : res1){if(r > 0) std::cout << " OK" ; else std::cout << " NG";}
        std::cout << " " << std::endl;

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
               WriteString(MValue.Handle(), ExceptionMessage);
        }
}

void FormRecognize::on_pushButton_DrawPos1_clicked()
{
    if(ISDRAW) return;
    DrawRectangle2Ex(1);
}

void FormRecognize::on_pushButton_DrawPos2_clicked()
{
    if(ISDRAW) return;
    DrawRectangle2Ex(2);
}

void FormRecognize::on_pushButton_DrawDetect_clicked()
{

    QDesktopServices::openUrl(QUrl(PROJECTNAME, QUrl::TolerantMode));
    return;
    if(ISDRAW) return;
    DrawRectangle2Ex(0);
}

void FormRecognize::on_pushButton_SaveSample_clicked()
{
    try {
        HalconCpp::HObject ho_Image;
        MSerials::Mat2Hobj(global::GetIns()->vecImages.at(2),ho_Image);
        if (HDevWindowStack::IsOpen())
          MSerials::h_disp_obj(ho_Image, HDevWindowStack::GetActive());
        int Idx = 1;
        std::vector<double> Bias;
        for(int i = 0; i<CHECKNUM;i++){
            Bias.push_back(LINEOFFSET[i]);
        }
        MSerials::CheckLogo(&global::GetIns()->vecImages.at(2),ROW[Idx],COLUMN[Idx],ABANGLE[Idx],LEN1[Idx],LEN2[Idx],Bias,LEN1[0],LEN2[0],-2);
        Idx = 2;
        Bias.clear();
        for(int i = 0; i<CHECKNUM;i++){
             Bias.push_back(LINEOFFSET1[i]);
        }
        MSerials::CheckLogo(&global::GetIns()->vecImages.at(2),ROW[Idx],COLUMN[Idx],ABANGLE[Idx],LEN1[Idx],LEN2[Idx],Bias,LEN1[0],LEN2[0],-2);
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
               WriteString(MValue.Handle(), ExceptionMessage);
        }
}

void FormRecognize::on_pushButton_Train_clicked()
{

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("更新训练文件？"), QMessageBox::Yes | QMessageBox::No);
    if(reply != QMessageBox::Yes) return;

    try{
    QString Class = "/OK";
    std::vector<std::string> pos,neg;
    {
    QDir dir(PROJECTNAME + Class);
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    dir.setSorting(QDir::Size | QDir::Reversed);
    QStringList filterList;filterList << "*.jpg" << "*.jpeg" << "*.bmp" << "*.png";   //设置筛选条件
    QFileInfoList list = dir.entryInfoList(filterList);
    for(auto &FileInfo :list){
        pos.push_back((PROJECTNAME + Class + "/" + FileInfo.fileName()).toLocal8Bit().data());
        //std::cout << FileInfo.fileName().toLocal8Bit().data()<<std::endl;
    }
    }

    Class = "/NG";
    {
    QDir dir(PROJECTNAME + Class);
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    dir.setSorting(QDir::Size | QDir::Reversed);
    QStringList filterList;filterList << "*.jpg" << "*.jpeg" << "*.bmp" << "*.png";   //设置筛选条件
    QFileInfoList list = dir.entryInfoList(filterList);
    for(auto &FileInfo :list){
        neg.push_back((PROJECTNAME + Class + "/" + FileInfo.fileName()).toLocal8Bit().data());
        //std::cout << FileInfo.fileName().toLocal8Bit().data()<<std::endl;
    }
    }
    MSerials::cv_train_inv(pos,neg,cv::Rect(0,0,200,200),QString(PROJECTNAME).toLocal8Bit().data(),false);
    MSerials::load_train_data(QString(PROJECTNAME).toLocal8Bit().data());

    SetColor(MValue.Handle(),"green");
    SetTposition(MValue.Handle(), 0, 1);
    WriteString(MValue.Handle(), "训练完毕");
    return;

    }catch(cv::Exception ex){
        SetColor(MValue.Handle(),"red");
        SetTposition(MValue.Handle(), 20, 1);
        WriteString(MValue.Handle(), ex.what());
        std::cout << ex.what() <<std::endl;

    }catch(std::exception ex){
        SetColor(MValue.Handle(),"red");
        SetTposition(MValue.Handle(), 20, 1);
        WriteString(MValue.Handle(), ex.what());
        std::cout << ex.what() <<std::endl;
    }

    SetColor(MValue.Handle(),"red");
    SetTposition(MValue.Handle(), 0, 1);
    WriteString(MValue.Handle(), "训练失败");
}
