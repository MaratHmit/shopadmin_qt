#ifndef FRAMECARDDISCOUNTS_H
#define FRAMECARDDISCOUNTS_H

#include "datadiscount.h"
#include "nametablemodel.h"
#include "dataproduct.h"

#include <QDialog>

namespace Ui {
class FrameCardDiscounts;
}

class FrameCardDiscounts : public QDialog
{
    Q_OBJECT

public:
    FrameCardDiscounts(QWidget *parent, const DataProduct *product,
                       const bool &isDialogMode = false);
    ~FrameCardDiscounts();

    void setIdProduct(const QString &id);
    void fillControls();    

signals:
    void modified();

private:
    Ui::FrameCardDiscounts *ui;
    ListDiscounts *mDiscounts;
    NameTableModel *mModel;
    QString mIdProduct;
    const DataProduct *mProduct;
    bool mIsFilled;
    bool mIsDialogMode;

    void initDiscountsModel();
    void initDiscounts();
    void initSignals();
    void addDiscount();
    void delDiscounts();
    void onChangeDiscountItem();
};

#endif // FRAMECARDDISCOUNTS_H
