#ifndef FORMFEATUREGROUPCARD_H
#define FORMFEATUREGROUPCARD_H

#include <QDialog>

#include "dataitem.h"

namespace Ui {
class FormFeatureGroupCard;
}

class FormFeatureGroupCard : public QDialog
{
    Q_OBJECT

public:
    explicit FormFeatureGroupCard(QWidget *parent = 0);
    ~FormFeatureGroupCard();

    void setId(QString &id);

private slots:
    void checkEnabled();

    void on_pushButtonMainInfo_clicked();
    void on_pushButtonNote_clicked();
    void on_buttonCancel_clicked();
    void on_buttonSave_clicked();

private:
    void showEvent(QShowEvent *e);

    Ui::FormFeatureGroupCard *ui;

    DataItem *mGroup;

    void setData();
    void getData();
    void fillControls();
    bool save();
};

#endif // FORMFEATUREGROUPCARD_H
