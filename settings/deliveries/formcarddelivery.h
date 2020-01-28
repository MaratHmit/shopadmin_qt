#ifndef FORMCARDDELIVERY_H
#define FORMCARDDELIVERY_H

#include <QDialog>

#include "nametablemodel.h"
#include "datadelivery.h"
#include "datacurrency.h"
#include "datapaysystem.h"
#include "dataproductsgroup.h"

namespace Ui {
class FormCardDelivery;
}

class FormCardDelivery : public QDialog
{
    Q_OBJECT

public:
    explicit FormCardDelivery(QWidget *parent);
    ~FormCardDelivery();

    bool save();
    void setDelivery(const DataDelivery *delivery);

private slots:
    void on_tabWidgetDelivery_currentChanged(int index);
    void on_pushButtonCancel_clicked();
    void on_pushButtonOk_clicked();    
    void onChangeTypeDelivery();
    void onChangeCondition();
    void onChangeConditionRegion();    
    void onChangeGeoLocationRegion();
    void onCheckedPaySystem();
    void onCheckedGroups(const QString &id, const bool &isChecked);

protected:
    void showEvent(QShowEvent *event);

private:
    Ui::FormCardDelivery *ui;

    bool mIsChildMode;
    bool mIsShowedPaySystems;
    bool mIsShowedGroups;
    bool mIsShowedConditions;
    bool mIsShowedGeoLocationsRegions;

    ListCurrencies *mListCurrencies;
    DataDelivery *mDelivery;
    NameTableModel *mModelPays;
    NameTableModel *mModelGroups;
    NameTableModel *mModelConditionsParameters;
    NameTableModel *mModelConditionsRegions;
    NameTableModel *mModelGeoLocationsRegions;
    ListProductsGroups *mListGroups;
    ListDataPaySystems *mListPaySystems;
    const ListDataItems *mCountries;
    const ListDataItems *mRegions;
    Request mRequestCity;
    ListDataItems *mCities;

    void getData();
    void getDisplayGeo();
    void setData();
    void fillControls();

    void initVariables();
    void initCurrencies();
    void initSignals();    
    void initRegions();    
    void initPermissions();
    void setValidators();
    void showPaySystems();
    void setPaySystems();
    void showGroups();
    void showConditions();  
    void showGeoLocationsRegions();
    bool setCheckedGroupById(const QString &id);
    void setCheckedGroups();    

    void onAddEditCondition();
    void onDeleteCondition();
    void onAddEditConditionRegion();
    void onDeleteConditionRegion();
    void onAddEditGeo();
    void onDeleteGeo();
};

#endif // FORMCARDDELIVERY_H
