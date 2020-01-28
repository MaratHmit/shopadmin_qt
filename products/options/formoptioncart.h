#ifndef FORMOPTIONCART_H
#define FORMOPTIONCART_H

#include <QDialog>

#include "dataitem.h"

namespace Ui {
class FormOptionCart;
}

class FormOptionCart : public QDialog
{
    Q_OBJECT

public:
    explicit FormOptionCart(QWidget *parent = 0);
    ~FormOptionCart();

     void setId(QString &id);

private slots:
    void on_buttonSave_clicked();
    void on_buttonCancel_clicked();

    void checkEnabled();

    void on_pushButtonMainInfo_clicked();

    void on_pushButtonNote_clicked();

private:
    Ui::FormOptionCart *ui;

    void showEvent(QShowEvent *e);

    void initVariables();
    void initSignals();

    DataItem *mOption;

    void loadGroups();

    void setData();
    void getData();
    void fillControls();
    bool save();
};

#endif // FORMOPTIONCART_H
