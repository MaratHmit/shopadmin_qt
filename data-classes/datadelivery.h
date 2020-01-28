#ifndef DATADELIVERY_H
#define DATADELIVERY_H

#include "datadeliveryconditions.h"

class DataDelivery : public DataItem        
{
    Q_OBJECT
    Q_PROPERTY(QString type READ typeTitle)
    Q_PROPERTY(QString period READ periodTitle)
    Q_PROPERTY(QString price READ priceTitle)
    Q_PROPERTY(QString city READ cityFrom)
    Q_PROPERTY(QString countryTo READ countryTo)
    Q_PROPERTY(QString regionTo READ regionTo)
    Q_PROPERTY(QString cityTo READ cityTo)
    Q_PROPERTY(QString geoDisplay READ geoDisplay)

public:
    DataDelivery();
    DataDelivery(const DataDelivery *d);
    DataDelivery(const QJsonObject &jsonobject);
    ~DataDelivery();

    void setFromJSONObject(const QJsonObject &jsonobject);
    QJsonObject getAsJsonObject() const;
    void initialization();
    void clearData();

    int period() const;
    qreal price() const;    
    QString cityFrom() const;
    QString idCityFrom() const;
    QString week() const;
    bool onePostion() const;
    int maxVolume() const;
    qreal maxWeight() const;
    bool needAddress() const;
    QString currency() const;
    QString idCityTo() const;
    QString idCountryTo() const;
    QString idRegionTo() const;
    QString country() const;
    QVector<QString> *idsPaySystems() const;
    SEVector<DataDelivery *> *conditionsRegions() const;
    SEVector<DataDelivery *> *geoLocationsRegions() const;
    QString idGeo() const;

    const QString countryTo() const;
    const QString regionTo() const;
    const QString cityTo() const;
    const QString geoDisplay() const;
    const QString typeTitle() const;
    const QString periodTitle() const;
    const QString priceTitle() const;

    void setPeriod(const int &period);
    void setPrice(const qreal &price);
    void setCurrency(const QString &currency);
    void setCityFrom(const QString &cityFrom);
    void setIdCityFrom(const QString &idCityFrom);
    void setWeek(const QString &week);
    void setOnePosition(const bool &onePostion);
    void setMaxVolume(const int &maxVolume);
    void setMaxWeight(const qreal &maxWeight);
    void setNeedAddress(const bool &needAddress);
    void setIdCountryTo(const QString &idCountryTo);
    void setCountryTo(const QString &countryTo);
    void setIdRegionTo(const QString &idRegionTo);
    void setRegionTo(const QString &regionTo);
    void setIdCityTo(const QString &idCityTo);
    void setCityTo(const QString &cityTo);

    void copy(const DataDelivery *d);
    void copy(const DataItem *d);

    bool isModified() const;

    ListDelivConditions *conditionsParams() const;

private:
    int mPeriod;
    qreal mPrice;
    QString mIdGeo;
    QString mCurrency;
    QString mIdCityFrom;
    QString mCityFrom;
    QString mWeek;    
    bool mOnePosition;
    int mMaxVolume;
    qreal mMaxWeight;
    bool mNeedAddress;
    QString mIdCountryTo;
    QString mCountryTo;
    QString mIdRegionTo;
    QString mRegionTo;
    QString mIdCityTo;
    QString mCityTo;    
    QVector<QString> *mIdsPaySystems;        
    ListDelivConditions *mConditionsParams;
    SEVector<DataDelivery *> *mConditionsRegions;
    SEVector<DataDelivery *> *mGeoLocationsRegions;

};

typedef SEVector<DataDelivery *> ListDeliveries;

#endif // DATADELIVERY_H
