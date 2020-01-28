#ifndef FORMDISCOUNTCARD_H
#define FORMDISCOUNTCARD_H

#include "datadiscount.h"
#include "seconfig.h"
#include "nametablemodel.h"

#include <QCheckBox>
#include <QHBoxLayout>
#include <QDialog>
#include <QVector>

namespace Ui {
class FormDiscountCard;
}

class FormDiscountCard : public QDialog
{
    Q_OBJECT

public:
    explicit FormDiscountCard(QWidget *parent = 0);
    ~FormDiscountCard();

    void setDiscount(const DataDiscount *discount);
    const DataDiscount *discount() const;

private:
    Ui::FormDiscountCard *ui;
    DataDiscount *mDiscount;
    DataMain *mMainInfo;
    NameTableModel *mModelGroupsProducts;
    NameTableModel *mModelProducts;    
    NameTableModel *mModelModifications;
    NameTableModel *mModelContacts;    

    void showEvent(QShowEvent *e);

    void onOK();
    void onAddGroupsProducts();    
    void onAddProducts();
    void onAddModifications();
    void onAddContacts();
    void onDeleteItems();
    void onChangeItem();
    void fillDataDiscount();
    void onChangeTypeValue(int index);        

    void getData();
    void initSignals();
    void initVariables();
    void fillControls();

};

#endif // FORMDISCOUNTCARD_H
