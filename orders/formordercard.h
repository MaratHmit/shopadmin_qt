#ifndef FORMORDERCARD_H
#define FORMORDERCARD_H

#include "dataorder.h"
#include "datacontact.h"
#include "datadelivery.h"
#include "nametablemodel.h"
#include "field.h"
#include "formoptionsparams.h"

#include <QDialog>
#include <QVBoxLayout>
#include <QGridLayout>

namespace Ui {
class FormOrderCard;
}

class FormOrderCard : public QDialog
{
    Q_OBJECT

public:
    explicit FormOrderCard(QWidget *parent, const DataOrder *order = 0);
    void setInfoMode();
    ~FormOrderCard();

signals:
    void saved();

private slots:
    void onChangePayment();    
    void onModified();

private:
    Ui::FormOrderCard *ui;
    DataOrder *mOrder;
    NameTableModel *mModelProducts;
    NameTableModel *mModelPayments;
    QVector<Field *> *mFieldsProducts;
    QVector<Field *> *mFieldsPayments;
    ListContacts *mListManagers;
    ListDeliveries *mListDeliveries;   

    bool mIsShowedPayments;
    bool mIsSelectOptions;

    void showEvent(QShowEvent *e);

    void addProduct();
    void addService();
    void editProduct();
    void deleteProducts();
    void calcSumTotal();
    void getData();
    void delManager();

    void addPayment();
    void deletePayment();

    void fillDefaultControls();
    void fillControls();    
    void initActions();
    void initSignals();
    void initVariables();    
    void initPermissions();

    void loadPayments();

    void onChangeCurrentItemProduct();
    void onChangeManager();
    void onChangeDeliveryStatus();
    void onChangeDeliveryType();
    void onChangeOrderStatus();

    void onOk();
    bool save();

    void showContextMenu(QPoint point);
    void setFieldItems();    
    void setFieldPayments();
    void selectItemMenu();
    void selectCustomer();
    void showCustomer();

};

#endif // FORMORDERCARD_H
