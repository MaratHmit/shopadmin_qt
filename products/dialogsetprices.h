#ifndef DIALOGSETPRICES_H
#define DIALOGSETPRICES_H

#include "dataproduct.h"

#include <QDialog>

namespace Ui {
class DialogSetPrices;
}

class DialogSetPrices : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSetPrices(QWidget *parent, const DataProduct *product);
    ~DialogSetPrices();    

private slots:
    void onChangeCurrency();

private:
    Ui::DialogSetPrices *ui;
    DataProduct *mProduct;

    void showEvent(QShowEvent *);

    void onOk();
    void initVariables();
    void initSignals();
    void initSignalsChangedData();

    bool save();
};

#endif // DIALOGSETPRICES_H
