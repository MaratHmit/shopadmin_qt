#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>

#include "seutils.h"
#include "nametablemodel.h"
#include "preloader.h"
#include "dataitem.h"

namespace Ui {
class FormLogin;
}

class FormLogin : public QDialog
{
    Q_OBJECT

public:
    explicit FormLogin(QWidget *parent = 0);
    bool authAuto();
    ~FormLogin();

private slots:
     void checkEnabled();
     void onChangeProject();
     void onAddProject();
     void onDelProject();     
     void onSetProxySettings();
     void onSearchProject(const QString &);

     void on_pushButtonOk_clicked();     
     void on_lineEditPassword_editingFinished();     

protected:
    bool eventFilter(QObject *object, QEvent *event);

private:
    Ui::FormLogin *ui;
    DataItem *auth;
    QString keyHash;
    NameTableModel *modelProjects;
    ListDataItems *projects;
    Preloader *preloader;

    void showEvent(QShowEvent *event);
    void timerEvent(QTimerEvent *event);

    void finishAuth();
    void initProjects();
    void initSignals();
    void showLoader();
    void hideLoader();    
    void createFolders();
    void checkUpdate();
    void startUpdate();

};

#endif // LOGIN_H
