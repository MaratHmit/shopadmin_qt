#ifndef FORMCONTACTCARD_H
#define FORMCONTACTCARD_H

#include <QDialog>

#include "datacontact.h"
#include "dataorder.h"
#include "nametablemodel.h"

namespace Ui {
class FormContactCard;
}

class FormContactCard : public QDialog
{
    Q_OBJECT

public:
    explicit FormContactCard(QWidget *parent = 0);
    ~FormContactCard();

    QString idContact();
    void setIdContact(const QString &idContact, const bool &isInfoMode = false);
    const DataContact *getContact() const;

signals:
    void saved(const DataContact *contact, const bool isNew);

private slots:
    void on_buttonSave_clicked();    
    void on_pushButtonMainInfo_clicked();
    void on_pushButtonNote_clicked();
    void on_pushButtonContactInfo_clicked();
    void on_pushButtonDocInfo_clicked();
    void on_pushButtonGroups_clicked();    
    void on_buttonCancel_clicked();
    void on_pushButtonPersonalAccount_clicked();
    void on_pushButtonOrders_clicked();
    void on_pushButtonImage_clicked();
    void on_pushButtonRequisites_clicked();
    void on_pushButtonCustomFields_clicked();

    void addEditAccount();
    void showOrderCard();
    void onChanged();
    void onOK();
    void selectReferContact();
    void deleteReferContact();

protected:
    void showEvent(QShowEvent *);

private:
    Ui::FormContactCard *ui;    
    NameTableModel *modelOrders;
    ListOrders *orders;
    bool isShownGroups;
    bool isShownAccounts;
    NameTableModel *modelGroupsContact;
    NameTableModel *modelAccounts;
    NameTableModel *modelRequisites;
    ListDataItems *groupsContact;
    DataContact *contact;

    bool saveContact();
    void getParams();
    void fillControls();
    void showGroups();
    void showPersonalAccount();
    void initSignals();
    void initSignalsChanged(const QWidget *widget = 0);
    void initFieldsAccountModel();
    void initPermission();
    void showImage();
    void showOrders();

};

#endif //  FORMCONTACTCARD_H
