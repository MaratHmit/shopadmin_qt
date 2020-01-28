#ifndef DIALOGCARDPREORDER_H
#define DIALOGCARDPREORDER_H

#include <QDialog>
#include "dataorder.h"

namespace Ui {
class DialogCardPreorder;
}

class DialogCardPreorder : public QDialog
{
    Q_OBJECT

public:
    explicit DialogCardPreorder(QWidget *parent, const DataOrder *order = 0);
    ~DialogCardPreorder();

private slots:
    void on_pushButtonCancel_clicked();
    void on_toolButtonCustomer_clicked();

    void on_pushButtonOK_clicked();

    void on_toolButtonCustomerInfo_clicked();

private:
    Ui::DialogCardPreorder *ui;

    DataOrder *mOrder;

    void setData();
    bool save();

    void calcSumTotal();
    void getData();
};

#endif // DIALOGCARDPREORDER_H
