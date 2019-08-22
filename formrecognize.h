#ifndef FORMRECOGNIZE_H
#define FORMRECOGNIZE_H

#include <QWidget>
#include <global.h>

namespace Ui {
class FormRecognize;
}

class FormRecognize : public QWidget
{
    Q_OBJECT

    TupleEx MValue;
public:
    explicit FormRecognize(QWidget *parent = nullptr,TupleEx tupleEx= TupleEx());
    ~FormRecognize();

    void DrawRectangle2Ex(int Idx = 0);
private slots:
    void on_pushButton_ReadImage_clicked();

    void on_pushButton_GrabImage_clicked();

    void on_pushButton_TestImage_clicked();

    void on_pushButton_DrawPos1_clicked();

    void on_pushButton_DrawPos2_clicked();

    void on_pushButton_DrawDetect_clicked();

    void RefreshTable(std::vector<std::pair<double,double>> Data);

    void on_pushButton_SaveSample_clicked();

    void on_pushButton_Train_clicked();

private:
    Ui::FormRecognize *ui;
};

#endif // FORMRECOGNIZE_H
