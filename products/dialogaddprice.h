#ifndef DIALOGADDPRICE_H
#define DIALOGADDPRICE_H

#include "dataproduct.h"

#include <QDialog>

namespace Ui {
class DialogAddPrice;
}

class DialogAddPrice : public QDialog
{
    Q_OBJECT

public:
    explicit DialogAddPrice(QWidget *parent, DataProduct *product);
    ~DialogAddPrice();

private:
    Ui::DialogAddPrice *ui;
    DataProduct *mProduct;

    void onChangeType();
    void save();
};

#endif // DIALOGADDPRICE_H
