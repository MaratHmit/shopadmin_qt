#ifndef FORMMODIFICATIONSGROUPSVALUES_H
#define FORMMODIFICATIONSGROUPSVALUES_H

#include <QWidget>

#include "formfeatures.h"
#include "nametablemodel.h"

namespace Ui {
class FormModificationsGroupsValues;
}

class FormModificationsGroupsValues : public QWidget
{
    Q_OBJECT

public:
    FormModificationsGroupsValues(QWidget *parent = 0);
    FormModificationsGroupsValues(QWidget *parent, const bool &isDialog);
    ~FormModificationsGroupsValues();

    void setGroupName(const QString &groupName);
    void setShadowMode();

signals:
    void modified();

public slots:
    void refreshData(ListFeatures *listValues);

private slots:
    void onAddValue();
    void onDeleteValue();

private:
    Ui::FormModificationsGroupsValues *ui;
    NameTableModel *mModel;
    FormFeatures *mFormFeatures;        

    bool isAddPermission;
    bool isEditPermission;
    bool isDeletePermission;

    void showEvent(QShowEvent *e);
    void hideEvent(QHideEvent *e);
    void initialization();
    void initSignals();
    void initActions();
    void initPermissions();

    bool mIsDialogMode;

};

#endif // FORMMODIFICATIONSGROUPSVALUES_H
