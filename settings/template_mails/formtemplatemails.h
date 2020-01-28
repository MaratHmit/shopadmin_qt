#ifndef FORMTEMPLATEMAILS_H
#define FORMTEMPLATEMAILS_H

#include <QWidget>
#include <QVector>
#include <QTreeView>

#include "datalettergroup.h"
#include "nametablemodel.h"
#include "preloader.h"
#include "dialoglettercard.h"

namespace Ui {
class FormTemplateMails;
}

class FormTemplateMails : public QWidget
{
    Q_OBJECT

public:
    explicit FormTemplateMails(QWidget *parent = 0);
    ~FormTemplateMails();

private slots:
    void onAddEditItem();
    void onDeleteItem();
    void onAddGroup();
    void onDeleteGroup(int index);
    void onChangeItem();
    void onSaveItem(const bool isNew, DataItem *letter);

private:
    void showEvent(QShowEvent *e);
    void timerEvent(QTimerEvent *e);

    Ui::FormTemplateMails *ui;
    ListLettersGroups *mGroups;
    bool mIsShowed;    
    Preloader *mPreloader;    

    bool isAddPermission;
    bool isEditPermission;
    bool isDeletePermission;

    void addGroup(const DataLetterGroup *group);
    void goToRowById(const QString &id);
    void refreshData();
    void initVariables();
    void initSignals();
    void initPermissions();
    bool existGroupByName(const QString &name);
    const QTreeView *currentTree() const;
    const DataLetterGroup *currentGroup() const;
    const DataItem *currentLetter() const;
    NameTableModel *currentModel() const;

};

#endif // FORMTEMPLATEMAILS_H
