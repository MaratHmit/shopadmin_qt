#ifndef DIALOGINCPRICES_H
#define DIALOGINCPRICES_H

#include <QDialog>

#include "dataproductsgroup.h"

namespace Ui {
class DialogIncPrices;
}

class DialogIncPrices : public QDialog
{
    Q_OBJECT

public:
    explicit DialogIncPrices(QWidget *parent, DataProductsGroup *item);
    ~DialogIncPrices();

private slots:
    void save();
    void onChangePrice(bool checked);

private:
    Ui::DialogIncPrices *ui;

    void showEvent(QShowEvent *);

    void getData();

    DataProductsGroup *mGroup;
};

#endif // DIALOGINCPRICES_H
