#ifndef DIALOGLETTERCARD_H
#define DIALOGLETTERCARD_H

#include <QDialog>

#include "dataitem.h"

namespace Ui {
class DialogLetterCard;
}

class DialogLetterCard : public QDialog
{
    Q_OBJECT
public:
    explicit DialogLetterCard(QWidget *parent = 0);
    ~DialogLetterCard();

    void setGroup(const QString &idGroup, const QString &nameGroup);
    void setLetter(const DataItem *letter);

signals:
    void save(const bool isNew, DataItem *letter);

private:    
    Ui::DialogLetterCard *ui;
    DataItem *mLetter;

    void showEvent(QShowEvent *e);

    void onOk();
    void onChangeData();
    void initVariables();
    void initSignals();
    void connectSignalsOnChanged();    
    void fillControls();

};

#endif // DIALOGLETTERCARD_H
