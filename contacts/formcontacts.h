#ifndef FORMCONTACTS_H
#define FORMCONTACTS_H

#include <QDialog>
#include <QBoxLayout>

#include "api.h"
#include "datacompany.h"
#include "preloader.h"
#include "request.h"
#include "formsetvisibleheadercolumns.h"
#include "formsetgroups.h"
#include "field.h"

namespace Ui {
class FormContacts;
}

class FormContacts : public QDialog
{
    Q_OBJECT

public:

    enum SelectMode
    {
        All = 0,
        Contacts = 1,
        Companies = 2
    };

    explicit FormContacts(QWidget *parent = 0);
    FormContacts(QWidget *parent, const bool &isMultiSelect, SelectMode mode = All);
    const ListContacts *getSelectedContacts();

    SelectMode getSelectMode() const;
    int getSelectedTypeContacts() const;

    ~FormContacts();

private slots:
    void onAddEditContact();
    void onDeleteContacts();
    void onClickHeaderItem(int index);
    void refreshData();
    void refreshDataContacts();
    void refreshDataCompanies();
    void onRunSearchContacts();
    void onRunSearchCompanies();
    void onChangeContact();    
    void onChangeCompany();
    void onCopyCellText();
    void showContextMenu(QPoint point);
    void onSaveContact(const DataContact *contact, const bool isNew);
    void onSaveCompany(const DataCompany *contact, const bool isNew);
    void goNextPage();
    void goPreviosPage();
    void goLastPage();
    void goFirstPage();
    void printContactInfo();    
    void tabChanged(int index);

    void finishedRefreshDataContacts(const bool status, const QString message);
    void finishedRefreshDataCompanies(const bool status, const QString message);    

protected slots:
    void reject();

private:
    void showEvent(QShowEvent *e);    

    DataContact *contact;
    Preloader *preloaderContacts;
    Preloader *preloaderCompanies;
    Request requestContacts;
    Request requestCompanies;
    FormSetVisibleHeaderColumns *formSetColumnsContacts;
    FormSetVisibleHeaderColumns *formSetColumnsCompanies;
    QVector<Field *> *fieldsContacts;
    QVector<Field *> *fieldsCompanies;
    ListContacts *contacts;
    ListContacts *companies;
    ListContacts *selectedContacts;

    bool isShown;
    bool isLoadedLayoutsContacts;
    bool isLoadedLayoutsCompanies;
    bool isSelectMode;
    SelectMode selectMode;
    int selectedTypeContacts;
    int countContacts;
    int countCompanies;
    int numPageContacts;
    int numPageCompanies;
    QString markedGroups;
    Api *apiContacts;
    Api *apiCompanies;

    QTimer *timerSearchContacts;
    QTimer *timerSearchCompanies;

    QPoint mousePress;

    void resetRequestContacts();
    void resetRequestCompanies();

    void initActions();
    void initTreeContacts();
    void initFields();
    void initVariables();
    void initSignals();
    void initPermissions();
    void goToRowContactsById(const QString &id);
    void goToRowCompaniesById(const QString &id);
    void setVisibleColumns();
    void onSetGroups();
    void setPriceType();
    void exportData();
    void importData();

    Ui::FormContacts *ui;

    NameTableModel *modelContacts;
    NameTableModel *modelCompanies;
};

#endif // FORMCONTACTS_H
