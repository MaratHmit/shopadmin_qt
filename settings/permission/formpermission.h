#ifndef FORMPERMISSION_H
#define FORMPERMISSION_H

#include <QWidget>
#include <QSplitter>
#include <QVBoxLayout>

#include "dataitem.h"
#include "dialogcardrole.h"
#include "apiadapter.h"
#include "seconfig.h"
#include "seconsts.h"
#include "formcatalog.h"
#include "formmanagers.h"
#include "formpermissionobjects.h"

namespace Ui {
class FormPermission;
}

class FormPermission : public QWidget
{
    Q_OBJECT

public:
    explicit FormPermission(QWidget *parent = 0);
    ~FormPermission();

private:
    Ui::FormPermission *ui;
    FormCatalog *formRoles;
    FormManagers *formManagers;
    FormPermissionObjects *formObjects;
    QSplitter *splittter;

    bool isShown;
    DialogCardRole *cardRole;

    void showEvent(QShowEvent *e);
    void hideEvent(QHideEvent *e);
    
    void initControls();
    void initControlsRoles();
    void initControlsObjects();
    void initVariables();
    void initSignals();
};

#endif // FORMPERMISSION_H
