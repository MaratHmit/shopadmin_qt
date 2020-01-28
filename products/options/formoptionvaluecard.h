#ifndef FORMOPTIONVALUECARD_H
#define FORMOPTIONVALUECARD_H

#include <QDialog>

#include "dataoption.h"

namespace Ui {
class FormOptionValueCard;
}

class FormOptionValueCard : public QDialog
{
    Q_OBJECT

public:
    explicit FormOptionValueCard(QWidget *parent = 0);
    ~FormOptionValueCard();

    void setId(const QString id);
    void setOptions(const ListOptions *options);
    void setCurrentIdOption(const QString id);
    void hideSelectedOption();

private slots:

    void save();
    void changeData();

    void on_pushButtonMainInfo_clicked();

    void on_pushButtonNote_clicked();

private:
    Ui::FormOptionValueCard *ui;

    void showEvent(QShowEvent *);

    void initSignals();
    void getData();
    void fillControls();
    void setComboBoxOptions();

    DataOptionValue *mValue;
};

#endif // FORMOPTIONPARAMCARD_H
