#include "datacoupon.h"

DataCoupon::DataCoupon()
{
    initialization();
}

DataCoupon::DataCoupon(const DataCoupon *d)
{
    initialization();
    copy(d);
}

DataCoupon::DataCoupon(const QJsonObject &jsonobject)
{
    initialization();
    setFromJSONObject(jsonobject);
}

DataCoupon::~DataCoupon()
{
    delete mListProducts;
    delete mListGroups;
}

void DataCoupon::clearData()
{
    DataItem::clearData();

    mType = "percent";
    mDiscount = 0.0;
    mCurrencyCode = "RUB";
    mTimeEnd = QDate::currentDate();
    mMinSum = 0.0;
    mCount = 1;
    mIsRegUser = true;
}

void DataCoupon::copy(const DataItem *d)
{
    copy(dynamic_cast <const DataCoupon *> (d));
}

void DataCoupon::copy(const DataCoupon *d)
{    
    DataItem::copy(d);

    setType(d->type());
    setDiscount(d->discount());
    setCurrencyCode(d->currencyCode());
    setTimeEnd(d->timeEnd());
    setMinSum(d->minSum());
    setCount(d->count());
    setIsRegUser(d->isRegUser());

    mListProducts->deleteItems();
    for (int i = 0; i < d->mListProducts->size(); ++i)
        mListProducts->append(new DataProduct(d->mListProducts->at(i)));
    mListGroups->deleteItems();
    for (int i = 0; i < d->mListGroups->size(); ++i)
        mListGroups->append(new DataProductsGroup(d->mListGroups->at(i)));
}

void DataCoupon::setFromJSONObject(const QJsonObject &jsonobject)
{
    DataItem::setFromJSONObject(jsonobject);
    setType(jsonobject["type"].toString());
    setDiscount(jsonobject["discount"].toDouble());
    setCurrencyCode(jsonobject["currencyCode"].toString());
    setTimeEnd(QDate::fromString(jsonobject["timeEnd"].toString(), "yyyy-MM-dd"));
    setMinSum(jsonobject["minSum"].toDouble());
    setCount(jsonobject["count"].toInt());
    setIsRegUser(jsonobject["isRegUser"].toBool());
    if (!jsonobject["idUser"].isNull())
        mIdUser = jsonobject["idUser"].toString();
    if (!jsonobject["userName"].isNull())
        mUserName = jsonobject["userName"].toString();

    QJsonArray items = jsonobject["products"].toArray();
    foreach (const QJsonValue& value, items)
        mListProducts->append(new DataProduct(value.toObject()));
    items = jsonobject["groups"].toArray();
    foreach (const QJsonValue& value, items)
        mListGroups->append(new DataProductsGroup(value.toObject()));
}

QJsonObject DataCoupon::getAsJsonObject() const
{
    QJsonObject result = DataItem::getAsJsonObject();

    result.insert("type", QJsonValue(mType));
    result.insert("discount", QJsonValue(mDiscount));
    result.insert("currencyCode", QJsonValue(mCurrencyCode));
    result.insert("timeEnd", QJsonValue(mTimeEnd.toString("yyyy-MM-dd")));
    result.insert("count", QJsonValue(mCount));
    result.insert("minSum", QJsonValue(mMinSum));
    result.insert("isRegUser", QJsonValue(mIsRegUser));
    if (isSetAllFields || modifiedFields.contains("idUser"))
        result.insert("idUser", QJsonValue(mIdUser));

    if (mListProducts->isModified())
        result.insert("products", mListProducts->getAsJsonArray());
    if (mListGroups->isModified())
        result.insert("groups", mListGroups->getAsJsonArray());

    return result;
}

void DataCoupon::initialization()
{
    DataItem::initialization();
    mListProducts = new ListProducts;
    mListGroups = new ListProductsGroups;
    mDiscount = 5;
    mTimeEnd = QDate::currentDate().addDays(1);
    mMinSum = 0;
    mCount = 0;
}

void DataCoupon::setType(const QString &type)
{
    if (type != mType) {
        mType = type;
        setModified("type");
    }
}

void DataCoupon::setDiscount(const qreal &discount)
{
    if (mDiscount != discount) {
        mDiscount = discount;
        setModified("discount");
    }
}

void DataCoupon::setCurrencyCode(const QString &currencyCode)
{
    if (mCurrencyCode != currencyCode) {
        mCurrencyCode = currencyCode;
        setModified("currencyCode");
    }
}

void DataCoupon::setTimeEnd(const QDate &timeEnd)
{
    if (mTimeEnd != timeEnd) {
        mTimeEnd = timeEnd;
        setModified("timeEnd");
    }
}

void DataCoupon::setCount(const int &count)
{
    if (mCount != count) {
        mCount = count;
        setModified("count");
    }
}

void DataCoupon::setIsRegUser(const bool &isRegUser)
{
    if (mIsRegUser != isRegUser) {
        mIsRegUser = isRegUser;
        setModified("isRegUser");
    }
}

void DataCoupon::setMinSum(const qreal &minSum)
{
    if (mMinSum != minSum) {
        mMinSum = minSum;
        setModified("minSum");
    }
}

void DataCoupon::setIdUser(const QString &idUser)
{
    if (mIdUser != idUser) {
        mIdUser = idUser;
        setModified("idUser");
        if (mIdUser.isEmpty())
            mUserName.clear();
    }
}

const QString DataCoupon::type() const
{
    return mType;
}

const QString DataCoupon::typeTitle() const
{
    if (mType == "p")
        return tr("Процент на всю корзину");
    if (mType == "a")
        return tr("Абсолютная скидка на корзину");
    if (mType == "g")
        return tr("Процент на позиции товара");

    return QString();
}

qreal DataCoupon::discount() const
{
    return mDiscount;
}

const QString DataCoupon::discountTitle() const
{
    return QString::number(mDiscount, 'f', 2);
}

const QString DataCoupon::currencyCode() const
{
    return mCurrencyCode;
}

const QDate DataCoupon::timeEnd() const
{
    return mTimeEnd;
}

qreal DataCoupon::minSum() const
{
    return mMinSum;
}

const QString DataCoupon::minSumTitle() const
{
    return QString::number(mMinSum, 'f', 2);
}

int DataCoupon::count() const
{
    return mCount;
}

bool DataCoupon::isRegUser() const
{
    return mIsRegUser;
}

const QString DataCoupon::isRegUserTitle() const
{
    if (mIsRegUser)
        return tr("Да");
    return tr("Нет");
}

ListProducts *DataCoupon::listProducts() const
{
    return mListProducts;
}

ListProductsGroups *DataCoupon::listGroups() const
{
    return mListGroups;
}

const QString DataCoupon::idUser() const
{
    return mIdUser;
}

const QString DataCoupon::userName() const
{
    return mUserName;
}

bool DataCoupon::isModified() const
{
    return DataItem::isModified() || mListProducts->isModified() || mListGroups->isModified();
}

