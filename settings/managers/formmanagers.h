#ifndef FORMMANAGERS_H
#define FORMMANAGERS_H

#include "nametablemodel.h"
#include "formcontacts.h"
#include "formcontactcard.h"
#include "request.h"
#include "field.h"
#include "dialogsetroles.h"
#include "api.h"
#include "preloader.h"

#include <QWidget>

namespace Ui {
class FormManagers;
}

class FormManagers : public QWidget
{
    Q_OBJECT

public:
    explicit FormManagers(QWidget *parent = 0);
    void setModelRoles(const NameTableModel *modelRoles);
    ~FormManagers();

public slots:
    void filterManagers(QStringList ids);

private slots:
    void addContact();
    void editContact();
    void delContact();
    void setRoles();

    void onChangeCurrentItem();
    void showContextMenu(QPoint point);

    void finishedRefreshData(const bool status, const QString message);

private:
    Ui::FormManagers *ui;
    NameTableModel *modelUsers;
    const NameTableModel *modelRoles;
    QVector<Field *> *fields;
    ListContacts *managers;
    Request request;
    Preloader *preloader;
    bool isShown;
    bool isLoadedLayouts;
    Api *apiFetch;

    void showEvent(QShowEvent *);    

    void checkEnabled();

    void initVariables();
    void initSignals();
    void initActions();
    void initFields();

    void goToRowById(QString id);
    void refreshData();
};

#endif // FORMMANAGERS_H
