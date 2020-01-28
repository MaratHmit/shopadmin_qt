#ifndef FORMCOMMENTCARD_H
#define FORMCOMMENTCARD_H

#include <QDialog>

#include "dataitem.h"
#include "dialogcard.h"

namespace Ui {
class FormCommentCard;
}

class FormCommentCard : public DialogCard
{
    Q_OBJECT

public:
    explicit FormCommentCard(QWidget *parent = 0);
    ~FormCommentCard();    

    virtual void setLocalMode();

private slots:    
    void onSelectProduct();

protected:    
    virtual void initControls();
    virtual void fillControls();

    bool setData();
    bool save();

private:
    void showEvent(QShowEvent *e);

    Ui::FormCommentCard *ui;    
    void initVariables();
    void initSignals();
};

#endif // FORMCOMMENTCARD_H
