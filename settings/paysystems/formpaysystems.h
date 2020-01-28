#ifndef FORMPAYSYSTEMS_H
#define FORMPAYSYSTEMS_H

#include "datapaysystem.h"
#include "nametablemodel.h"
#include "preloader.h"

#include <QWidget>

namespace Ui {
class FormPaySystems;
}

class FormPaySystems : public QWidget
{
    Q_OBJECT

public:
    explicit FormPaySystems(QWidget *parent = 0);
    ~FormPaySystems();

private slots:
    void showContextMenu(QPoint point);
    void onSetIsActive(const QString &id, const bool &isActive);
    void onChange();
    void onUpDownSort();
    void onAddEditPaySystem();
    void onDeletePaySystem();
    void onSavePayment(const bool& isNew, DataPaySystem *paySystem);

private:
    void showEvent(QShowEvent *e);
    void timerEvent(QTimerEvent *e);

    Ui::FormPaySystems *ui;
    Preloader *mPreloader;
    ListDataPaySystems *mPaySystems;
    NameTableModel *mPaysSystemsModel;
    int mTypeTimer;
    int mIdTimer;
    bool mIsShowed;

    bool isAddPermission;
    bool isEditPermission;
    bool isDeletePermission;

    QString currentId();
    void goToRowById(const QString &id);
    void saveSortPositions();

    void refreshData();
    void initActions();
    void initVariables();
    void initSignals();
    void initPermissions();
};

#endif // FORMPAYSYSTEMS_H
