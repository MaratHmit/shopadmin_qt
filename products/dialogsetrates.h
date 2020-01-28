#ifndef DIALOGSETRATES_H
#define DIALOGSETRATES_H

#include "dataproduct.h"

#include <QDialog>

namespace Ui {
class DialogSetRates;
}

class DialogSetRates : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSetRates(QWidget *parent, const DataProduct *product);
    ~DialogSetRates();

private:
    Ui::DialogSetRates *ui;

    DataProduct *mProduct;

    void showEvent(QShowEvent *);

    void onOk();
    void initVariables();
    void initSignals();
    void initSignalsChangedData();

    bool save();
};

#endif // DIALOGSETRATES_H
