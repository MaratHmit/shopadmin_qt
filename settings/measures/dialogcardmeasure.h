#ifndef DIALOGCARDMEASURE_H
#define DIALOGCARDMEASURE_H

#include <QDialog>

#include "dataitem.h"
#include "dialogcard.h"
#include "dialogdelivregion.h"
#include "nametablemodel.h"

namespace Ui {
class DialogCardMeasure;
}

class DialogCardMeasure : public DialogCard
{
    Q_OBJECT

public:
    explicit DialogCardMeasure(QWidget *parent = 0);
    ~DialogCardMeasure();

    void setVolumeMode();

protected:
    void initSignals();
    void initVariables();

    virtual void initControls();
    virtual void fillControls();

    bool setData();
    bool save();

private slots:
    void setDecimalsPrecision(const int value);

private:
    Ui::DialogCardMeasure *ui;

    bool mIsVolumeMode;
    QString mApiName;
};

#endif // DIALOGCARDMEASURE_H
