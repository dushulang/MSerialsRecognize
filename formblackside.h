#ifndef FORMBLACKSIDE_H
#define FORMBLACKSIDE_H

#include <QWidget>
#include "global.h"

namespace Ui {
class FormBlackSide;
}

class FormBlackSide : public QWidget
{
    Q_OBJECT
    TupleEx MValue;
public:
    explicit FormBlackSide(QWidget *parent = nullptr, TupleEx tupleEx = TupleEx(0,0));
    ~FormBlackSide();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_Grab_clicked();

    void on_pushButton_FlipImage_clicked();

    void on_pushButton_SaveImage_clicked();

    void on_pushButton_SavePara_clicked();

    void on_pushButton_TestImage_clicked();

    void on_pushButton_SAVEIMAGE_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::FormBlackSide *ui;
};

#endif // FORMBLACKSIDE_H
