#ifndef DIALOGDELIVCONDITION_H
#define DIALOGDELIVCONDITION_H

#include "datadeliveryconditions.h"

#include <QDialog>

namespace Ui {
class DialogDelivCondition;
}

class DialogDelivCondition : public QDialog
{
    Q_OBJECT

public:
    explicit DialogDelivCondition(QWidget *parent,
                                  DataDeliveryConditions *condition);
    ~DialogDelivCondition();

private slots:
    void onChangeData();

private:
    Ui::DialogDelivCondition *ui;
    DataDeliveryConditions *mCondition;

    void showEvent(QShowEvent *e);
    void initSignals();
    void initControls();
    void fillControls();
    void setValidators();
};

#endif // DIALOGDELIVCONDITION_H
