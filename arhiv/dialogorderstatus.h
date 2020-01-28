#ifndef DIALOGORDERSTATUS_H
#define DIALOGORDERSTATUS_H

#include <QDialog>

#include "dataorder.h"

namespace Ui {
class DialogOrderStatus;
}

class DialogOrderStatus : public QDialog
{
    Q_OBJECT

public:
    explicit DialogOrderStatus(QWidget *parent, DataOrder *order = 0);
    ~DialogOrderStatus();

private:
    Ui::DialogOrderStatus *ui;
    DataOrder *mOrder;

    void onChangeStatus();
    void initVariables();
    void initSignals();
};

#endif // DIALOGORDERSTATUS_H
