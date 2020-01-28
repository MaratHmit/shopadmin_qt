#ifndef FORMOPTIONSGROUPS_H
#define FORMOPTIONSGROUPS_H

#include "apiadapter.h"
#include "seconfig.h"
#include "request.h"
#include "field.h"
#include "userpermission.h"
#include "nametablemodel.h"

#include <QWidget>

namespace Ui {
class FormOptionsGroups;
}

class FormOptionsGroups : public QWidget
{
    Q_OBJECT

public:
    explicit FormOptionsGroups(QWidget *parent = 0);
    ~FormOptionsGroups();

    void setSelectMode();
    QString currentIdGroup() const;
    QString currentNameGroup() const;
    QStringList idsChecked() const;

signals:
    void checkedGroups();

public slots:
    void refreshData();

private slots:
    void onAddEditOptionGroup();
    void onUpDownOptionGroup();
    void onChangeGroups();
    void onCheckedGroups();
    void on_toolButtonDelGroup_clicked();

private:
    void showEvent(QShowEvent *e);
    void timerEvent(QTimerEvent *event);
    bool eventFilter(QObject *object, QEvent *event);

    Ui::FormOptionsGroups *ui;
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

#endif // FORMOPTIONSGROUPS_H
