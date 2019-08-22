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
    explicit FormBlackSide(QWidget *parent = nullptr, TupleEx tupleEx = TupleEx());
    ~FormBlackSide();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_Grab_clicked();

private:
    Ui::FormBlackSide *ui;
};

#endif // FORMBLACKSIDE_H
