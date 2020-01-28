#ifndef DIALOGPRODUCTITEM_H
#define DIALOGPRODUCTITEM_H

#include "dataorderitem.h"

#include <QDialog>

namespace Ui {
class DialogProductItem;
}

class DialogProductItem : public QDialog
{
    Q_OBJECT

public:
    explicit DialogProductItem(QWidget *parent, const DataOrderItem *orderItem = 0,
                               const bool &isServiceMode = false);
    ~DialogProductItem();

    const DataOrderItem *product() const;

private:
    Ui::DialogProductItem *ui;
    DataOrderItem *mOrderItem;
    bool mIsServiceMode;

    void calcSumTotal();
    void checkEnabled();
    void fillControls();
    void initSignals();    
    void selectProduct();
    void setNote();

    void initPermissions();
};

#endif // DIALOGPRODUCTITEM_H
