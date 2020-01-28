#ifndef DIALOGSETRATE_H
#define DIALOGSETRATE_H

#include <QDialog>

#include "datacurrency.h"

namespace Ui {
class DialogSetRate;
}

class DialogSetRate : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSetRate(QWidget *parent = 0);
    ~DialogSetRate();

    void setCurrency(const DataCurrency *currency, const DataCurrency *baseCurrency);

private:
    Ui::DialogSetRate *ui;
    DataCurrency *mCurrency;
    DataCurrency *mBaseCurrency;

    void showEvent(QShowEvent *e);

    void onOK();
    void onChangeData();
    void onSetRateFromCBR();

    void fillControls();
    void initSignals();
    void initVariables();
    void connectChangeSignals();
};

#endif // DIALOGSETRATE_H
