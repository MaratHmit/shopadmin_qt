#ifndef DIALOGSETORDERSTATUS_H
#define DIALOGSETORDERSTATUS_H

#include "dataorder.h"

#include <QDialog>

namespace Ui {
class DialogSetOrderStatus;
}

class DialogSetOrderStatus : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSetOrderStatus(QWidget *parent = 0);
    ~DialogSetOrderStatus();

    enum TYPE_STATUS
    {
        ORDER_STATUS = 0,
        DELIVERY_STATUS = 1
    };

    void setTypeStatus(DialogSetOrderStatus::TYPE_STATUS typeStatus);
    void setOrder(DataOrder *);

private:
    Ui::DialogSetOrderStatus *ui;
    TYPE_STATUS mTypeStatus;
    DataOrder *mOrder;
    ListDataItems *mStatuses;

    void showEvent(QShowEvent *);

    void getData();
    void initVariables();
    void initSignals();
    void save();
    void onModified();

};

#endif // DIALOGSETORDERSTATUS_H
