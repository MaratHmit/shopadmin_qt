#ifndef DIALOGDELIVREGION_H
#define DIALOGDELIVREGION_H

#include "request.h"
#include "datadelivery.h"

#include <QDialog>

namespace Ui {
class DialogDelivRegion;
}

class DialogDelivRegion : public QDialog
{
    Q_OBJECT

public:
    explicit DialogDelivRegion(QWidget *parent, DataDelivery *region);
    ~DialogDelivRegion();

    void setExistRegions(const ListDeliveries *);
    void setGeoMode();

private slots:
    void onChangeCountry();
    void onChangeRegion();

private:
    Ui::DialogDelivRegion *ui;    
    Request mRequestCity;
    const ListDataItems *mCountries;
    const ListDataItems *mRegions;
    ListDataItems *mCities;
    DataDelivery *mRegion;
    const ListDeliveries *mExistRegions;
    QString mIdCountryDefault;
    QString mIdRegionDefault;
    QString mIdCityDefault;

    void showEvent(QShowEvent *);

    void onOk();    
    void loadCities(const QString &idCountry, const QString &idRegion = QString());
    void initSignals();
    void initVariables();
    void fillControls();
    void restoreIdsCountryRegionCity();
    void storeCountry(const QString &idCountry);
    void storeRegion(const QString &idRegion);
    void storeCity(const QString &idCity);


};

#endif // DIALOGDELIVREGION_H
