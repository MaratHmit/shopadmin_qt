#ifndef FORMPAYSYSTEMCARD_H
#define FORMPAYSYSTEMCARD_H

#include <QDialog>

#include "nametablemodel.h"
#include "datapaysystem.h"
#include "datapayrequisite.h"
#include "framewysiwyg.h"

namespace Ui {
class FormPaySystemCard;
}

class FormPaySystemCard : public QDialog
{
    Q_OBJECT

public:
    explicit FormPaySystemCard(QWidget *parent = 0);
    ~FormPaySystemCard();

    void setPaySystem(const DataPaySystem *paySystem);
    void setExistPlugins(const QStringList &list);

signals:
    void save(const bool isNew, DataPaySystem *paySystem);

private slots:
    void onChangeData();
    void onChangeTab();

private:
    void showEvent(QShowEvent *e);
    void timerEvent(QTimerEvent *e);
    void onSave();
    void onOk();    

    Ui::FormPaySystemCard *ui;
    DataPaySystem *mPaySystem;
    ListDataItems *mPlugins;
    NameTableModel *mModelPayReqs;
    NameTableModel *mModelHosts;
    NameTableModel *mModelFilters;
    QStringList mExistPluginsList;

    FrameWYSIWYG *mFrameInfoPage;
    FrameWYSIWYG *mFrameAccountPage;
    FrameWYSIWYG *mFrameSuccesPage;
    FrameWYSIWYG *mFrameResultPage;
    FrameWYSIWYG *mFrameFailPage;

    void hideTabControls();
    void initVariables();
    void initSignals();
    void initPlugins();    
    void getData();
    void setData();
    void loadParameters();
    void fillControls();
    void connectChangeSignals();
    void disconnectChangeSignals();    

    void onCurrentChangedParam();
    void onAddEditParam();
    void onDelParam();
    void onCurrentChangedFilter();
    void onAddFilter();
    void onDelFilter();
    void onCurrentChangedHost();
    void onAddHost();
    void onDelHost();

};

#endif // FORMPAYSYSTEMCARD_H
