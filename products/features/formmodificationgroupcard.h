#ifndef FORMMODIFICATIONGROUPCARD_H
#define FORMMODIFICATIONGROUPCARD_H

#include <QDialog>

#include "datamodificationgroup.h"

namespace Ui {
class FormModificationGroupCard;
}

class FormModificationGroupCard : public QDialog
{
    Q_OBJECT

public:
    explicit FormModificationGroupCard(QWidget *parent = 0);
    ~FormModificationGroupCard();

    void setGroup(DataModificationGroup *group);

private slots:
    void onSaveClicked();
    void onNameChanged();
    void onModifiedListValues();

private:
    Ui::FormModificationGroupCard *ui;
    DataModificationGroup *mGroup;    

    void showEvent(QShowEvent *e);

    void getData();
    void setData();
    void initSignals();
    void initBoxPricesTypes();
    bool save();

};

#endif // FORMMODIFICATIONGROUPCARD_H
