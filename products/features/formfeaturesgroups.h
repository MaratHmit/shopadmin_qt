#ifndef FORMFEATURESGROUPS_H
#define FORMFEATURESGROUPS_H

#include <QDialog>

#include "dataitem.h"
#include "nametablemodel.h"

namespace Ui {
class FormFeaturesGroups;
}

class FormFeaturesGroups : public QDialog
{
    Q_OBJECT

public:
    explicit FormFeaturesGroups(QWidget *parent = 0);
    ~FormFeaturesGroups();

    void setSelectMode();
    QString currentIdGroup() const;
    QString currentNameGroup() const;
    QStringList idsChecked() const;
    void setShadowMode();

signals:
    void checkedGroups();

public slots:
    void refreshData();

private slots:
    void onAddEditFeatureGroup();
    void onUpDownFeatureGroup();
    void onChangeGroups();
    void onCheckedGroups();
    void on_toolButtonDelGroup_clicked();

private:
    void showEvent(QShowEvent *e);
    void timerEvent(QTimerEvent *event);
    bool eventFilter(QObject *object, QEvent *event);

    Ui::FormFeaturesGroups *ui;
    int mTypeTimer;
    int mIdTimer;
    bool mSelectMode;
    bool mIsShowed;

    bool isAddPermission;
    bool isEditPermission;
    bool isDeletePermission;

    void saveSortGroups();

    void initTableGroups();
    void initButtonsSignals();
    void initPermissions();

    void goToRowByIdGroup(QString id);

    NameTableModel *mGroupsModel;
    ListDataItems *mGroups;
};

#endif // FORMFEATURESGROUPS_H
