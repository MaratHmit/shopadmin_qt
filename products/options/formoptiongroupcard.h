#ifndef FORMOPTIONGROUPCARD_H
#define FORMOPTIONGROUPCARD_H

#include <QDialog>

#include "dataitem.h"

namespace Ui {
class FormOptionGroupCard;
}

class FormOptionGroupCard : public QDialog
{
    Q_OBJECT

public:
    explicit FormOptionGroupCard(QWidget *parent = 0);
    ~FormOptionGroupCard();

    void setId(QString &id);

private slots:
    void on_buttonCancel_clicked();
    void on_buttonSave_clicked();

    void checkEnabled();

private:
    Ui::FormOptionGroupCard *ui;

    void showEvent(QShowEvent *e);

    DataItem *mGroup;

    void setData();
    void getData();
    void fillControls();
    bool save();
};

#endif // FORMOPTIONGROUPCARD_H
