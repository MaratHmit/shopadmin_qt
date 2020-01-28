#include "datadelivery.h"
#include "seconfig.h"
#include "apiadapter.h"
#include "request.h"

DataDelivery::DataDelivery():
    DataItem()
{
    initialization();
}

DataDelivery::DataDelivery(const DataDelivery *d)
{
    initialization();
    copy(d);
}

DataDelivery::DataDelivery(const QJsonObject &jsonobject)
{
    initialization();
    setFromJSONObject(jsonobject);
}

DataDelivery::~DataDelivery()
{
    delete mIdsPaySystems;    
    delete mConditionsParams;
    delete mConditionsRegions;
    delete mGeoLocationsRegions;
}

void DataDelivery::setFromJSONObject(const QJsonObject &jsonobject)
{    
    DataItem::setFromJSONObject(jsonobject);

    mPeriod = jsonobject["period"].toInt();
    mPrice = jsonobject["price"].toDouble();
    mCityFrom = jsonobject["cityFrom"].toString();
    mIdCityFrom = jsonobject["idCityFrom"].toString();
    mWeek = jsonobject["week"].toString();
    mOnePosition = jsonobject["onePosition"].toBool();
    mMaxVolume = jsonobject["maxVolume"].toInt();
    mMaxWeight = jsonobject["maxWeight"].toDouble();
    mNeedAddress = jsonobject["needAddress"].toBool();
    mCurrency = jsonobject["currency"].toString();
    mIdCountryTo = jsonobject["idCountryTo"].toString();
    mCountryTo = jsonobject["countryTo"].toString();
    mIdRegionTo = jsonobject["idRegionTo"].toString();
    mRegionTo = jsonobject["regionTo"].toString();
    mIdCityTo = jsonobject["idCityTo"].toString();
    mCityTo = jsonobject["cityTo"].toString();
    mIdGeo = jsonobject["idGeo"].toString();


    QJsonArray idPaySystemsJ = jsonobject["idsPaySystems"].toArray();
    foreach (const QJsonValue& value, idPaySystemsJ)
        mIdsPaySystems->append(value.toString());        

    if (jsonobject["conditionsRegions"].isArray()) {
        QJsonArray regions = jsonobject["conditionsRegions"].toArray();
        foreach (const QJsonValue& value, regions)
            mConditionsRegions->append(new DataDelivery(value.toObject()));
    }

    if (jsonobject["conditionsParams"].isArray()) {
        QJsonArray params = jsonobject["conditionsParams"].toArray();
        foreach (const QJsonValue& value, params)
            mConditionsParams->append(new DataDeliveryConditions(value.toObject()));
    }

    if (jsonobject["geoLocationsRegions"].isArray()) {
        QJsonArray regions = jsonobject["geoLocationsRegions"].toArray();
        foreach (const QJsonValue& value, regions)
            mGeoLocationsRegions->append(new DataDelivery(value.toObject()));
    }
}

QJsonObject DataDelivery::getAsJsonObject() const
{
    QJsonObject obj = DataItem::getAsJsonObject();

    obj.insert("period", QJsonValue(mPeriod));
    obj.insert("price", QJsonValue(mPrice));    
    obj.insert("cityFrom", QJsonValue(mCityFrom));
    obj.insert("idCityFrom", QJsonValue(mIdCityFrom));
    obj.insert("week", QJsonValue(mWeek));
    obj.insert("onePosition", QJsonValue(mOnePosition));
    obj.insert("maxVolume", QJsonValue(mMaxVolume));
    obj.insert("maxWeight", QJsonValue(mMaxWeight));
    obj.insert("needAddress", QJsonValue(mNeedAddress));
    obj.insert("currency", QJsonValue(mCurrency));
    obj.insert("idCountryTo", QJsonValue(mIdCountryTo));
    obj.insert("countryTo", QJsonValue(mCountryTo));
    obj.insert("idRegionTo", QJsonValue(mIdRegionTo));
    obj.insert("regionTo", QJsonValue(mRegionTo));
    obj.insert("idCityTo", QJsonValue(mIdCityTo));
    obj.insert("cityTo", QJsonValue(mCityTo));


    QJsonArray idsPaySystems;
    QVectorIterator<QString> it(*mIdsPaySystems);
    while (it.hasNext()) {
        QString d = it.next();
        if (!d.isEmpty()) {
            if (!idsPaySystems.isEmpty())
                idsPaySystems += ",";
            idsPaySystems += d;
        }
    }
    obj.insert("idsPaySystems", QJsonValue(idsPaySystems));    

    if (mConditionsRegions->isModified()) {
        mConditionsRegions->setIsSetAllFields(1);
        obj.insert("conditionsRegions", QJsonValue(mConditionsRegions->getAsJsonArray()));
    }
    if (mConditionsParams->isModified())
        obj.insert("conditionsParams", QJsonValue(mConditionsParams->getAsJsonArray()));
    if (mGeoLocationsRegions->isModified())
        obj.insert("geoLocationsRegions", QJsonValue(mGeoLocationsRegions->getAsJsonArray()));

    return obj;
}

void DataDelivery::initialization()
{    
    DataItem::initialization();

    mIdsPaySystems = new QVector<QString>;    
    mConditionsParams = new ListDelivConditions();
    mConditionsRegions = new ListDeliveries;
    mGeoLocationsRegions = new ListDeliveries;

    mPeriod = 0;
    mPrice = 0.0;
    mWeek = "1111111";
    mOnePosition = 0;
    mMaxVolume = 0;
    mMaxWeight = 0.0;
    mNeedAddress = 0;
    mCurrency = "RUB";
}

void DataDelivery::clearData()
{
    DataItem::clearData();

    mPeriod = 0;
    mPrice = 0.0;
    mCityFrom.clear();
    mIdCityFrom.clear();
    mIdGeo.clear();
    mWeek = "1111111";
    mOnePosition = false;
    mMaxVolume = 0;
    mMaxWeight = 0.0;
    mNeedAddress = false;
    mCurrency = "RUB";
    mIdCountryTo.clear();
    mIdRegionTo.clear();
    mIdCityTo.clear();    
    mConditionsParams->clear();
    mConditionsRegions->deleteItems();
    mGeoLocationsRegions->deleteItems();
    mIdsPaySystems->clear();    
    mCountryTo.clear();
    mRegionTo.clear();
    mCityTo.clear();
}

int DataDelivery::period() const
{
    return  mPeriod;
}

qreal DataDelivery::price() const
{
    return mPrice;
}

QString DataDelivery::cityFrom() const
{
    return mCityFrom;
}

QString DataDelivery::idCityFrom() const
{
    return mIdCityFrom;
}

QString DataDelivery::week() const
{
    return mWeek;
}

bool DataDelivery::onePostion() const
{
    return mOnePosition;
}

int DataDelivery::maxVolume() const
{
    return mMaxVolume;
}

qreal DataDelivery::maxWeight() const
{
    return mMaxWeight;
}

bool DataDelivery::needAddress() const
{
    return mNeedAddress;
}

QString DataDelivery::currency() const
{
    return mCurrency;
}

QString DataDelivery::idCityTo() const
{
    return mIdCityTo;
}

QString DataDelivery::idCountryTo() const
{
    return mIdCountryTo;
}

QString DataDelivery::idRegionTo() const
{
    return mIdRegionTo;
}

QString DataDelivery::country() const
{
    return mCountryTo;
}

QVector<QString> *DataDelivery::idsPaySystems() const
{
    return mIdsPaySystems;
}

SEVector<DataDelivery *> *DataDelivery::geoLocationsRegions() const
{
    return mGeoLocationsRegions;
}

QString DataDelivery::idGeo() const
{
    return mIdGeo;
}

const QString DataDelivery::countryTo() const
{
    return mCountryTo;
}

const QString DataDelivery::regionTo() const
{
    return mRegionTo;
}

const QString DataDelivery::cityTo() const
{
    return mCityTo;
}

const QString DataDelivery::geoDisplay() const
{
    QString result = QString();
    if (!mCountryTo.isEmpty())
        result = mCountryTo;
    if (!mRegionTo.isEmpty()) {
        if (!result.isEmpty())
            result += ", ";
        result += mRegionTo;
    }
    if (!mCityTo.isEmpty()) {
        if (!result.isEmpty())
            result += ", ";
        result += mCityTo;
    }
    return result;
}

ListDeliveries *DataDelivery::conditionsRegions() const
{
    return mConditionsRegions;
}

const QString DataDelivery::typeTitle() const
{
    for (int i = 0; i < SEConfig::getTypesDeliveries()->size(); i++)
        if (SEConfig::getTypesDeliveries()->at(i)->getCode() == getCode())
            return SEConfig::getTypesDeliveries()->at(i)->getName();
    return QString();
}

const QString DataDelivery::periodTitle() const
{
    if (mPeriod)
        return QString::number(mPeriod) + tr("дн.");
    return QString();
}

const QString DataDelivery::priceTitle() const
{
    return SEConfig::priceCurrency(mPrice, mCurrency);
}

void DataDelivery::setPeriod(const int &period)
{
    if (mPeriod != period) {
        setModified("period");
        mPeriod = period;
    }
}

void DataDelivery::setPrice(const qreal &price)
{
    if (mPrice != price) {
        setModified("price");
        mPrice = price;
    }
}

void DataDelivery::setCurrency(const QString &currency)
{
    if (mCurrency != currency) {
        mCurrency = currency;
        setModified("currency");        
    }
}

void DataDelivery::setCityFrom(const QString &cityFrom)
{
    if (mCityFrom != cityFrom) {
        mCityFrom = cityFrom;
        setModified("cityFrom");
    }
}

void DataDelivery::setIdCityFrom(const QString &idCityFrom)
{
    if (mIdCityFrom != idCityFrom) {
        mIdCityFrom = idCityFrom;
        setModified("idCityFrom");
    }
}

void DataDelivery::setWeek(const QString &week)
{
    if (mWeek != week) {
        mWeek = week;
        setModified("week");
    }
}

void DataDelivery::setOnePosition(const bool &onePostion)
{
    if (mOnePosition != onePostion) {
        mOnePosition = onePostion;
        setModified("onePostion");
    }
}

void DataDelivery::setMaxVolume(const int &maxVolume)
{
    if (mMaxVolume != maxVolume) {
        mMaxVolume = maxVolume;
        setModified("maxVolume");
    }
}

void DataDelivery::setMaxWeight(const qreal &maxWeight)
{
    if (mMaxWeight != maxWeight) {
        mMaxWeight = maxWeight;
        setModified("maxWeight");
    }
}

void DataDelivery::setNeedAddress(const bool &needAddress)
{
    if (mNeedAddress != needAddress) {
        mNeedAddress = needAddress;
        setModified("needAddress");
    }
}

void DataDelivery::setIdCityTo(const QString &idCityTo)
{
    if (mIdCityTo != idCityTo) {
        mIdCityTo = idCityTo;
        setModified("idCityTo");
    }
}

void DataDelivery::setCityTo(const QString &cityTo)
{
    mCityTo = cityTo;
}

void DataDelivery::setIdCountryTo(const QString &idCountryTo)
{
    if (mIdCountryTo != idCountryTo) {
        mIdCountryTo = idCountryTo;
        setModified("idCountryTo");
    }
}

void DataDelivery::setCountryTo(const QString &countryTo)
{
    mCountryTo = countryTo;
}

void DataDelivery::setIdRegionTo(const QString &idRegionTo)
{
    if (mIdRegionTo != idRegionTo) {
        mIdRegionTo = idRegionTo;
        setModified("idRegionTo");
    }
}

void DataDelivery::setRegionTo(const QString &regionTo)
{
    mRegionTo = regionTo;
}

void DataDelivery::copy(const DataDelivery *d)
{
     DataItem::copy(d);

     mPeriod = d->mPeriod;
     mPrice = d->mPrice;
     mCurrency = d->mCurrency;
     mCityFrom = d->mCityFrom;
     mIdCityFrom = d->mIdCityFrom;
     mWeek = d->mWeek;
     mOnePosition = d->mOnePosition;
     mMaxVolume = d->mMaxVolume;
     mMaxWeight = d->mMaxWeight;
     mNeedAddress = d->mNeedAddress;
     mIdCountryTo = d->mIdCountryTo;
     mCountryTo = d->mCountryTo;
     mIdRegionTo = d->mIdRegionTo;
     mRegionTo = d->mRegionTo;
     mIdCityTo = d->mIdCityTo;
     mCityTo = d->mCityTo;     
     mIdGeo = d->mIdGeo;

     mIdsPaySystems->clear();
     for (int i = 0; i < d->mIdsPaySystems->size(); ++i)
         mIdsPaySystems->append(d->mIdsPaySystems->at(i));
     mConditionsParams->clear();
     for (int i = 0; i < d->mConditionsParams->size(); ++i)
         mConditionsParams->append(d->mConditionsParams->at(i));
     mConditionsRegions->clear();
     for (int i = 0; i < d->mConditionsRegions->size(); ++i)
         mConditionsRegions->append(d->mConditionsRegions->at(i));     
     mGeoLocationsRegions->clear();
     for (int i = 0; i < d->mGeoLocationsRegions->size(); ++i)
         mGeoLocationsRegions->append(d->mGeoLocationsRegions->at(i));
}

void DataDelivery::copy(const DataItem *d)
{
     copy(dynamic_cast <const DataDelivery *> (d));
}

bool DataDelivery::isModified() const
{
    return DataItem::isModified() || conditionsParams()->isModified() ||
            conditionsRegions()->isModified() || geoLocationsRegions()->isModified();
}

ListDelivConditions *DataDelivery::conditionsParams() const
{
    return mConditionsParams;
}
