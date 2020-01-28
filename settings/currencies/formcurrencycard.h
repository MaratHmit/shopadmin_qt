#ifndef FORMCURRENCYCARD_H
#define FORMCURRENCYCARD_H

#include <QDialog>

#include "datacurrency.h"

namespace Ui {
class FormCurrencyCard;
}

class FormCurrencyCard : public QDialog
{
    Q_OBJECT

public:
    explicit FormCurrencyCard(QWidget *parent = 0);
    ~FormCurrencyCard();

    void setCurrency(const DataCurrency *currency);
    const DataCurrency *currency() const;

private:
    DataCurrency *mCurrency;
    Ui::FormCurrencyCard *ui;

    void showEvent(QShowEvent *e);

    void onChangeBoxCurrencies(const int &index);
    void onChangeData();
    void onOK();

    void initListCurrencies();
    void initSignals();
    void initVariables();
    void connectChangeSignals();
    void fillControls();
    void setValidators();

};

#endif // FORMCURRENCYCARD_H
