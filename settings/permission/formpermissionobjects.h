#ifndef FORMPERMISSIONOBJECTS_H
#define FORMPERMISSIONOBJECTS_H

#include <QWidget>

#include "dataitem.h"
#include "permissionsmodel.h"
#include "permissionsdelegate.h"
#include "seconfig.h"
#include "seconsts.h"
#include "preloader.h"

namespace Ui {
class FormPermissionObjects;
}

class FormPermissionObjects : public QWidget
{
    Q_OBJECT

public:
    explicit FormPermissionObjects(QWidget *parent, const ListDataItems &items);
    ~FormPermissionObjects();    

private slots:
    void completedRefreshData(const bool status, const QString message);
    void refreshData();

private:
    Ui::FormPermissionObjects *ui;
    PermissionsModel *modelPermission;
    PermissionsDelegate *delegatePermissions;
    Preloader *preloader;    
    const ListDataItems &roles;

    void showEvent(QShowEvent *e);

    void initVariables();
    void initSignals();
    void initControls();

    void refreshRoles();


};

#endif // FORMPERMISSIONOBJECTS_H
