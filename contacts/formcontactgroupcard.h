#ifndef CONTACTGROUPCARD_H
#define CONTACTGROUPCARD_H

#include "dataitem.h"

#include <QDialog>

namespace Ui {
class FormContactGroupCard;
}

class FormContactGroupCard : public QDialog
{
    Q_OBJECT

public:
    explicit FormContactGroupCard(QWidget *parent = 0);
    ~FormContactGroupCard();

    void setGroup(const DataItem *group);

signals:
    void saved(const DataItem *group, const bool isNew);

private slots:
    void onCancel();
    void onEditNameChanged(const QString &arg);
    void onSave();

private:
    void showEvent(QShowEvent *e);
    Ui::FormContactGroupCard *ui;
    DataItem *group;

    void initSignals();
    bool save();
};

#endif // CONTACTGROUPCARD_H
