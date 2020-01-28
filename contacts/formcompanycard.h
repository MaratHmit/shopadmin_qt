#ifndef FORMCOMPANYCARD_H
#define FORMCOMPANYCARD_H

#include <QDialog>

#include "datacompany.h"
#include "dataorder.h"
#include "nametablemodel.h"

namespace Ui {
class FormCompanyCard;
}

class FormCompanyCard : public QDialog
{
    Q_OBJECT

public:
    explicit FormCompanyCard(QWidget *parent = 0);
    ~FormCompanyCard();

    QString idCompany();
    void setIdCompany(const QString &idCompany, const bool &isInfoMode = false);
    const DataCompany *getCompany() const;

signals:
    void saved(const DataCompany *company, const bool isNew);

private slots:
    void on_buttonCancel_clicked();
    void on_buttonSave_clicked();
    void on_buttonOK_clicked();

    void onChanged();

    void on_pushButtonMainInfo_clicked();
    void on_pushButtonContacts_clicked();
    void on_pushButtonGroups_clicked();
    void on_pushButtonOrders_clicked();

    void addContact();
    void deleteContact();
    void onChangeContact();
    void showOrderCard();

    void on_pushButtonCustomFields_clicked();

private:
    Ui::FormCompanyCard *ui;

    void showEvent(QShowEvent *e);

    NameTableModel *modelOrders;
    NameTableModel *modelContacts;
    NameTableModel *modelGroups;
    ListOrders *orders;
    ListDataItems *groupsContact;
    bool isShownGroups;

    DataCompany *company;

    void initVariables();
    void initModelContacts();    
    void uncheckedButtons();

    void showOrders();
    void showGroups();

    bool saveCompany();
    void getParams();
    void fillControls();
    void initSignals();
    void initSignalsChanged(const QWidget *widget = 0);
};

#endif // FORMCOMPANYCARD_H
