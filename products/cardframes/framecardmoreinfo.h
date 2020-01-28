#ifndef FRAMECARDMOREINFO_H
#define FRAMECARDMOREINFO_H

#include "dataproduct.h"

#include <QWidget>

namespace Ui {
class FrameCardMoreInfo;
}

class FrameCardMoreInfo : public QWidget
{
    Q_OBJECT

public:    
    FrameCardMoreInfo(QWidget *parent, DataProduct *product);
    ~FrameCardMoreInfo();

    void showEvent(QShowEvent *e);

    void fillControls();
    void refreshData();
    void correctDataBeforeSave();

signals:
    void modified();

private slots:
    void onChangeData();    

private:
    Ui::FrameCardMoreInfo *ui;
    DataProduct *mProduct;
    ListDataItems *mListManufacturers;    
    bool mIsShowed;
    bool mIsFilled;    

    void timerEvent(QTimerEvent *e);

    void disconnectSignalsChanged();
    void connectSignalsChanged(const QWidget *widget = 0);

};

#endif // FRAMECARDMOREINFO_H
