#ifndef FORMCOUPONCARD_H
#define FORMCOUPONCARD_H

#include "datacoupon.h"
#include "nametablemodel.h"

#include <QDialog>

namespace Ui {
class FormCouponCard;
}

class FormCouponCard : public QDialog
{
    Q_OBJECT

public:
    explicit FormCouponCard(QWidget *parent = 0);
    ~FormCouponCard();

    void setCoupon(const DataCoupon *coupon);
    const DataCoupon *coupon() const;

private:
    Ui::FormCouponCard *ui;
    DataCoupon *mCoupon;    
    NameTableModel *mModelProducts;
    NameTableModel *mModelGroups;

    void showEvent(QShowEvent *e);

    void onOK();
    void onAddProducts();
    void onDeleteProducts();
    void onAddGroups();
    void onDeleteGroups();
    void onChangeCurrentItemGroups();
    void onChangeCurrentItemProducts();
    void onGenerateCode();
    void onChangeData();
    void onChangeDiscountType();
    void onChangeTabCoupons();
    void onCopyCode();
    void onSelectUser();
    void onClearUser();

    void getData();
    void connectOnChangeSignals();
    void initCurrencies();
    void initSignals();
    void initVariables();
    void fillControls();

};

#endif // FORMCOUPONCARD_H
