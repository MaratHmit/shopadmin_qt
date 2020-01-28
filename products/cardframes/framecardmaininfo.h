#ifndef FRAMECARDMAININFO_H
#define FRAMECARDMAININFO_H

#include <QWidget>

#include "dataproduct.h"
#include "datacurrency.h"

namespace Ui {
class FrameCardMainInfo;
}

class FrameCardMainInfo : public QWidget
{
    Q_OBJECT

public:
    explicit FrameCardMainInfo(QWidget *parent = 0);
    ~FrameCardMainInfo();

    void setProduct(DataProduct *product);
    void showEvent(QShowEvent *e);
    void setMultiMode(const bool &isMultiMode);
    void fillControls();
    void refreshData();
    void correctDataBeforeSave();

signals:
    void modified();

private slots:
    void onSelectGroup();
    void onDeleteGroup();
    void onSetInfinitely(const bool &value);    
    void onSelectBrand();
    void onDeleteBrand();
    void onSelectYAMarketCategories();
    void onDeleteYAMarketCategories();
    void onChangeData();
    void onChangeMeasureWeght();
    void onChangeMeasureVolume();

private:
    Ui::FrameCardMainInfo *ui;
    ListCurrencies *mListCurrencies;
    ListDataItems *mListMeasuresWeight;
    ListDataItems *mListMeasuresVolume;
    bool mIsShowed;
    bool mIsFilled;        
    bool mIsMultiMode;
    DataProduct *mProduct;

    void initSignals();
    void transliteCode();
    void setValidators();
    void initCurrencies();
    void initMeasures();
    void initLabels();
    void connectSignalsChanged();
    void disconnectSignalsChanged();

};

#endif // FRAMECARDMAININFO_H
