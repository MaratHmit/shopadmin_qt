#ifndef FORMNEWSGROUPCARD_H
#define FORMNEWSGROUPCARD_H

#include <QDialog>

#include "dataitem.h"

namespace Ui {
class FormNewsGroupCard;
}

class FormNewsGroupCard : public QDialog
{
    Q_OBJECT

public:
    explicit FormNewsGroupCard(QWidget *parent = 0);
    ~FormNewsGroupCard();

    void setIdParentGroup(const QString &idParent,
                          const QString &titleParent);
    void setGroup(const DataItem *group);

    const DataItem *group() const;

private:
    Ui::FormNewsGroupCard *ui;    
    DataItem *mGroup;

    void showEvent(QShowEvent *e);

    void onSelectParentGroup();
    void onClearParentGroup();
    void onOK();

    void checkEnabled();
    void fillControls();
    void initSignals();
    bool save();
};

#endif // FORMNEWSGROUPCARD_H
