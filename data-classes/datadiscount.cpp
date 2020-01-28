#include "datadiscount.h"
#include "seconfig.h"
#include "dataproduct.h"
#include "dataproductsgroup.h"
#include "datacontact.h"

#include "QRegExp"

DataDiscount::DataDiscount()
{
    initialization();
}

DataDiscount::DataDiscount(const DataDiscount *d)
{
    initialization();
    copy(d);
}

DataDiscount::DataDiscount(const QJsonObject &jsonobject)
{
    initialization();
    setFromJSONObject(jsonobject);
}

DataDiscount::~DataDiscount()
{
    delete mListGroupsProducts;
    delete mListProducts;
    delete mListModificationsProducts;
    delete mListContacts;
}

void DataDiscount::initialization()
{
    DataItem::initialization();
    mStepTime = 0;
    mStepDiscount = 0;
    mDateTimeFrom = QDateTime::currentDateTime();
    mDateTimeTo = QDateTime::currentDateTime().addDays(1);
    mSumFrom = 0;
    mSumTo = 0;
    mCountFrom = 0;
    mCountTo = 0;
    mDiscount = 5.0;
    mTypeDiscount = "percent";
    mWeek = "1111111";
    mTypeSum = 0;
    mListGroupsProducts = new ListDataItems();
    mListProducts = new ListDataItems();
    mListModificationsProducts = new ListDataItems();
    mListContacts = new ListDataItems();    
}

void DataDiscount::clearData()
{
    DataItem::clearData();

    mStepTime = 0;
    mStepDiscount = 0;
    mDateTimeFrom = QDateTime::currentDateTime();
    mDateTimeTo = QDateTime::currentDateTime().addDays(1);
    mSumFrom = 0;
    mSumTo = 0;
    mCountFrom = 0;
    mCountTo = 0;
    mDiscount = 5.0;
    mTypeDiscount = "percent";
    mWeek = "1111111";
    mTypeSum = 0;
}

void DataDiscount::copy(const DataItem *d)
{
    copy(dynamic_cast <const DataDiscount *> (d));
}

void DataDiscount::copy(const DataDiscount *d)
{
    DataItem::copy(d);    
    setStepTime(d->stepTime());
    setStepDiscount(d->stepDiscount());
    setDiscount(d->discount());
    setDateTimeFrom(d->dateTimeFrom());
    setDateTimeTo(d->dateTimeTo());
    setSumFrom(d->sumFrom());
    setSumTo(d->sumTo());
    setCountFrom(d->countFrom());
    setCountTo(d->countTo());
    setDiscount(d->discount());
    setTypeDiscount(d->typeDiscount());
    setWeek(d->week());
    setTypeSum(d->typeSum());
}

void DataDiscount::setFromJSONObject(const QJsonObject &jsonobject)
{
    DataItem::setFromJSONObject(jsonobject);    
    setStepTime(jsonobject["stepTime"].toInt());
    setStepDiscount(jsonobject["stepDiscount"].toDouble());
    setDiscount(jsonobject["discount"].toDouble());    
    setDateTimeFrom(QDateTime::fromString(jsonobject["dateTimeFrom"].toString(), "yyyy-MM-dd hh:mm:ss"));
    setDateTimeTo(QDateTime::fromString(jsonobject["dateTimeTo"].toString(), "yyyy-MM-dd hh:mm:ss"));
    setSumFrom(jsonobject["sumFrom"].toDouble());
    setSumTo(jsonobject["sumTo"].toDouble());
    setCountFrom(jsonobject["countFrom"].toDouble());
    setCountTo(jsonobject["countTo"].toDouble());
    setTypeDiscount(jsonobject["typeDiscount"].toString());
    setWeek(jsonobject["week"].toString());
    setTypeSum(jsonobject["typeSum"].toInt());

    mListProducts->deleteItems();
    QJsonArray items = jsonobject["listProducts"].toArray();
    foreach (const QJsonValue& value, items)
        mListProducts->append(new DataProduct(value.toObject()));

    mListGroupsProducts->deleteItems();
    items = jsonobject["listGroupsProducts"].toArray();
    foreach (const QJsonValue& value, items)
        mListGroupsProducts->append(new DataItem(value.toObject()));    

    mListModificationsProducts->deleteItems();
    items = jsonobject["listModificationsProducts"].toArray();
    foreach (const QJsonValue& value, items)
        mListModificationsProducts->append(new DataProduct(value.toObject()));

    mListContacts->deleteItems();
    items = jsonobject["listContacts"].toArray();
    foreach (const QJsonValue& value, items)
        mListContacts->append(new DataContact(value.toObject()));

    setUnModified();
}

QJsonObject DataDiscount::getAsJsonObject() const
{
    QJsonObject result = DataItem::getAsJsonObject();
    result.insert("stepTime", QJsonValue(mStepTime));
    result.insert("stepDiscount", QJsonValue(mStepDiscount));
    result.insert("discount", QJsonValue(mDiscount));
    result.insert("dateTimeFrom", QJsonValue(mDateTimeFrom.toString("yyyy-MM-dd hh:mm:ss")));
    result.insert("dateTimeTo", QJsonValue(mDateTimeTo.toString("yyyy-MM-dd hh:mm:ss")));
    result.insert("sumFrom", QJsonValue(mSumFrom));
    result.insert("sumTo", QJsonValue(mSumTo));
    result.insert("countFrom", QJsonValue(mCountFrom));
    result.insert("countTo", QJsonValue(mCountTo));
    result.insert("typeDiscount", QJsonValue(mTypeDiscount));
    result.insert("week", QJsonValue(mWeek));
    result.insert("typeSum", QJsonValue(mTypeSum));
    if (mListProducts->isModified()) {
        mListProducts->setIsSetAllFields(true);
        result.insert("listProducts", QJsonValue(mListProducts->getAsJsonArray()));
    }
    if (mListGroupsProducts->isModified()) {
        mListGroupsProducts->setIsSetAllFields(true);
        result.insert("listGroupsProducts", QJsonValue(mListGroupsProducts->getAsJsonArray()));
    }
    if (mListModificationsProducts->isModified())
        result.insert("listModificationsProducts", QJsonValue(mListModificationsProducts->getAsJsonArray()));
    if (mListContacts->isModified()) {
        mListContacts->setIsSetAllFields(true);
        result.insert("listContacts", QJsonValue(mListContacts->getAsJsonArray()));
    }

    return result;
}

void DataDiscount::setStepTime(const int &stepTime)
{
    if (stepTime != mStepTime) {
        mStepTime = stepTime;
        setModified("stepTime");
    }
}

void DataDiscount::setStepDiscount(const qreal &stepDiscount)
{
    if (stepDiscount != mStepDiscount) {
        mStepDiscount = stepDiscount;
        setModified("stepDiscount");
    }
}

void DataDiscount::setDateTimeFrom(const QDateTime &dateTimeFrom)
{
    if (dateTimeFrom != mDateTimeFrom) {
        mDateTimeFrom = dateTimeFrom;
        setModified("dateTimeFrom");
    }
}

void DataDiscount::setDateTimeTo(const QDateTime &dateTimeTo)
{
    if (dateTimeTo != mDateTimeTo) {
        mDateTimeTo = dateTimeTo;
        setModified("dateTimeTo");
    }
}

void DataDiscount::setSumFrom(const qreal &sumFrom)
{
    if (sumFrom != mSumFrom) {
        mSumFrom = sumFrom;
        setModified("sumFrom");
    }
}

void DataDiscount::setSumTo(const qreal &sumTo)
{
    if (sumTo != mSumTo) {
        mSumTo = sumTo;
        setModified("sumTo");
    }
}

void DataDiscount::setCountFrom(const qreal &countFrom)
{
    if (countFrom != mCountFrom) {
        mCountFrom = countFrom;
        setModified("countFrom");
    }
}

void DataDiscount::setCountTo(const qreal &countTo)
{
    if (countTo != mCountTo) {
        mCountTo = countTo;
        setModified("countTo");
    }
}

void DataDiscount::setDiscount(const qreal &discount)
{
    if (discount != mDiscount) {
        mDiscount = discount;
        setModified("discount");
    }
}

void DataDiscount::setTypeDiscount(const QString &typeDiscount)
{
    if (typeDiscount != mTypeDiscount) {
        mTypeDiscount = typeDiscount;
        setModified("typeDiscount");
    }
}

void DataDiscount::setWeek(const QString &week)
{
    if (week != mWeek) {
        mWeek = week;
        setModified("week");
    }
}

void DataDiscount::setTypeSum(const int &typeSum)
{
    if (typeSum != mTypeSum) {
        mTypeSum = typeSum;
        setModified("typeSum");
    }
}

int DataDiscount::stepTime() const
{
    return mStepTime;
}

qreal DataDiscount::stepDiscount() const
{
    return mStepDiscount;
}

const QDateTime DataDiscount::dateTimeFrom() const
{
    return mDateTimeFrom;
}

const QDateTime DataDiscount::dateTimeTo() const
{
    return mDateTimeTo;
}

qreal DataDiscount::sumFrom() const
{
    return mSumFrom;
}

qreal DataDiscount::sumTo() const
{
    return mSumTo;
}

qreal DataDiscount::countFrom() const
{
    return mCountFrom;
}

qreal DataDiscount::countTo() const
{
    return mCountTo;
}

qreal DataDiscount::discount() const
{
    return mDiscount;
}

const QString DataDiscount::typeDiscount() const
{
    return mTypeDiscount;
}

const QString DataDiscount::week() const
{
    return mWeek;
}

int DataDiscount::typeSum() const
{
    return mTypeSum;
}

const QString DataDiscount::stepTimeTitle() const
{
    return QString::number(mStepTime) + "ч.";
}

const QString DataDiscount::stepDiscountTitle() const
{
    if (mTypeDiscount == "percent")
        return QString::number(mStepDiscount, 'f', 2) + "%";
     return SEConfig::priceCurrency(mStepDiscount);
}

const QString DataDiscount::dateTimeFromTitle() const
{
    return dateTimeFrom().toString("dd.MM.yyyy hh:mm");
}

const QString DataDiscount::dateTimeToTitle() const
{
    return dateTimeTo().toString("dd.MM.yyyy hh:mm");
}

const QString DataDiscount::sumFromTitle() const
{
    return QString::number(mSumFrom, 'f', 2);
}

const QString DataDiscount::sumToTitle() const
{
    return QString::number(mSumTo, 'f', 2);
}

const QString DataDiscount::countFromTitle() const
{
    return QString::number(mCountFrom, 'f', 2);
}

const QString DataDiscount::countToTitle() const
{
    return QString::number(mCountTo, 'f', 2);
}

const QString DataDiscount::discountTitle() const
{
    if (mTypeDiscount == "percent")
        return QString::number(mDiscount, 'f', 2) + "%";
    return SEConfig::priceCurrency(mDiscount);
}

const QString DataDiscount::weekTitle() const
{
    QString result;
    QStringList list;

    if (mWeek.size() == 7) {
        if (mWeek[0] == '1')
            list.append(tr("Пн."));
        if (mWeek[1] == '1')
            list.append("Вт.");
        if (mWeek[2] == '1')
            list.append("Ср.");
        if (mWeek[3] == '1')
            list.append(tr("Чт."));
        if (mWeek[4] == '1')
            list.append(tr("Пт."));
        if (mWeek[5] == '1')
            list.append(tr("Сб."));
        if (mWeek[6] == '1')
            list.append(tr("Вс."));

        for (int i = 0; i < list.size(); i++) {
            if (!result.isEmpty())
                result += ",";
            result += list.at(i);
        }
    }
    return result;
}

ListDataItems *DataDiscount::listProducts() const
{
    return mListProducts;
}

ListDataItems *DataDiscount::listGroupsProducts() const
{
    return mListGroupsProducts;
}

ListDataItems *DataDiscount::listModificationsProducts() const
{
    return mListModificationsProducts;
}

ListDataItems *DataDiscount::listContacts() const
{
    return mListContacts;
}

bool DataDiscount::isModified() const
{
    return DataItem::isModified() || mListProducts->isModified() ||
            mListGroupsProducts->isModified() || mListContacts->isModified() ||
            mListModificationsProducts->isModified();
}
