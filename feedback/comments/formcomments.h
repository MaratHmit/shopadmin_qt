#ifndef FORMCOMMENTS_H
#define FORMCOMMENTS_H

#include <QWidget>

#include "nametablemodel.h"
#include "seconfig.h"
#include "dataitem.h"
#include "formcommentcard.h"

namespace Ui {
class FormComments;
}

class FormComments : public QWidget
{
    Q_OBJECT

public:
    FormComments(QWidget *parent = 0);    
    FormComments(QWidget *parent, ListDataItems *comments);
    ~FormComments();

    void fillControls();
    void clearInfo();
    void setIdProduct(const QString &id);

signals:
    void modified();

public slots:
    void refreshData();

private slots:
    void onAddEditComment();
    void onDeleteComment();
    void onChange();
    void showContextMenu(QPoint point);

private:
    void showEvent(QShowEvent *e);
    void hideEvent(QHideEvent *e);
    void timerEvent(QTimerEvent *event);

    Ui::FormComments *ui;
    FormCommentCard *mCardComment;
    NameTableModel *mModel;
    Request mRequest;    
    ListDataItems *mComments;
    QString mIdProduct;
    bool mIsShowed;
    bool mIsLoadedLayouts;
    bool mIsLocalMode;

    void goToRowById(const QString &id);
    void initialization();
    void initActions();
    void initVariables();
    void initTable();
    void initSignals();
    void resetRequest();
    void saveState();
};

#endif // FORMCOMMENTS_H
