#ifndef DIALOGSETPAYPARAMVALUE_H
#define DIALOGSETPAYPARAMVALUE_H

#include "dataitem.h"

#include <QDialog>

namespace Ui {
class DialogSetPayParamValue;
}

class DialogSetPayParamValue : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSetPayParamValue(QWidget *parent = 0);
    ~DialogSetPayParamValue();

    void setParam(DataItem *param);
    QString value() const;

private slots:
    void on_buttonBox_accepted();

private:
    Ui::DialogSetPayParamValue *ui;
    DataItem *mParam;

    void setData();
};

#endif // DIALOGSETPAYPARAMVALUE_H
