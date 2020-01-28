#ifndef DIALOGADDEDITACCOUNT_H
#define DIALOGADDEDITACCOUNT_H

#include "datapersonalaccount.h"

#include <QDialog>

namespace Ui {
class DialogAddEditAccount;
}

class DialogAddEditAccount : public QDialog
{
    Q_OBJECT

public:
    explicit DialogAddEditAccount(QWidget *parent, const DataPersonalAccount *account = 0);
    ~DialogAddEditAccount();

    const DataPersonalAccount *getAccount() const;

private slots:
    void changeTypeOperation();
    void changeComment();

private:
    Ui::DialogAddEditAccount *ui;
    DataPersonalAccount *account;

    void showEvent(QShowEvent *);

    void initVariables();
    void initSignals();
    void initSignalsChanged();
    void initTypesOperations();
    void getData();
};

#endif // DIALOGADDEDITACCOUNT_H
