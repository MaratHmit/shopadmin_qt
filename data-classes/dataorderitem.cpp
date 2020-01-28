#include "dataorderitem.h"
#include "seconfig.h"

DataOrderItem::DataOrderItem()
{
    initialization();
}

DataOrderItem::DataOrderItem(const DataOrderItem *d)
{
    initialization();
    copy(d);
}

DataOrderItem::DataOrderItem(const DataProduct *d)
{
    initialization();
    copy(d);
}

DataOrderItem::DataOrderItem(const QJsonObject &jsonobject)
{
    initialization();
    setFromJSONObject(jsonobject);
}

DataOrderItem::~DataOrderItem()
{
    delete mListOptions;
}

void DataOrderItem::copy(const DataItem *d)
{
    copy(dynamic_cast <const DataOrderItem *> (d));
}

void DataOrderItem::copy(const DataOrderItem *d)
{
    DataItem::copy(d);

    mIdPrice = d->mIdPrice;
    mArticle = d->mArticle;
    mIdsModifications = d->mIdsModifications;
    mPrice = d->mPrice;
    mPricePurchase = d->mPricePurchase;
    mDiscount = d->mDiscount;
    mCount = d->mCount;
    mCurrency = d->mCurrency;
    mMeasurement = d->mMeasurement;
}

void DataOrderItem::copy(const DataProduct *d)
{
    DataItem::copy(d);

    setId(QString());
    mIdPrice = d->getId();
    mArticle = d->getArticle();
    mPrice = d->price();
    mPricePurchase = d->pricePurchase();
    mDiscount = d->discountPercent();
    mCount = 1;
    mCountOptions = d->getCountOptions();
    mCurrency = d->getCurrency();
    mMeasurement = d->measurement();
    mIdsModifications = d->idsModifications();
    mListOptions->deleteItems();
    for (int i = 0; i < d->listOptions()->size(); i++)
        mListOptions->append(new DataOption(d->listOptions()->at(i)));
}

void DataOrderItem::setFromJSONObject(const QJsonObject &jsonobject)
{
    DataItem::setFromJSONObject(jsonobject);

    if (!jsonobject["idPrice"].isNull())
        mIdPrice = jsonobject["idPrice"].toString();
    if (!jsonobject["article"].isNull())
        mArticle = jsonobject["article"].toString();
    if (!jsonobject["idsModifications"].isNull())
        mIdsModifications = jsonobject["idsModifications"].toString();
    if (!jsonobject["price"].isNull())
        mPrice = jsonobject["price"].toDouble();
    if (!jsonobject["pricePurchase"].isNull())
        mPricePurchase = jsonobject["pricePurchase"].toDouble();
    if (!jsonobject["count"].isNull())
        mCount = jsonobject["count"].toDouble();
    if (!jsonobject["discount"].isNull())
        mDiscount = jsonobject["discount"].toDouble();
    if (!jsonobject["currency"].isNull())
        mCurrency = jsonobject["currency"].toString();
    if (!jsonobject["originalName"].isNull())
        mOriginalName = jsonobject["originalName"].toString();
    if (!jsonobject["measurement"].isNull())
        mMeasurement = jsonobject["measurement"].toString();
    if (!jsonobject["countOptions"].isNull())
        mCountOptions = jsonobject["countOptions"].toInt();
    mListOptions->deleteItems();
    QJsonArray items = jsonobject["options"].toArray();
    foreach (const QJsonValue& value, items)
        mListOptions->append(new DataOption(value.toObject()));
}

QJsonObject DataOrderItem::getAsJsonObject() const
{
    QJsonObject obj = DataItem::getAsJsonObject();

    if (isSetAllFields || modifiedFields.contains("idPrice"))
        obj.insert("idPrice", QJsonValue(mIdPrice));
    if (isSetAllFields || modifiedFields.contains("article"))
        obj.insert("article", QJsonValue(mArticle));
    if (isSetAllFields || modifiedFields.contains("idsModifications"))
        obj.insert("idsModifications", QJsonValue(mIdsModifications));
    if (isSetAllFields || modifiedFields.contains("price"))
        obj.insert("price", QJsonValue(mPrice));
    if (isSetAllFields || modifiedFields.contains("pricePurchase"))
        obj.insert("pricePurchase", QJsonValue(mPricePurchase));
    if (isSetAllFields || modifiedFields.contains("count"))
        obj.insert("count", QJsonValue(mCount));
    if (isSetAllFields || modifiedFields.contains("discount"))
        obj.insert("discount", QJsonValue(mDiscount));
    if (isSetAllFields || mListOptions->isModified()) {
        mListOptions->setIsSetAllFields(true);
        obj.insert("options", QJsonValue(mListOptions->getAsJsonArray()));
    }

    return obj;
}

void DataOrderItem::clearData()
{
    DataItem::clearData();

    mPrice = 0;
    mPricePurchase = 0;
    mCount = 0;
    mDiscount = 0;
    mCountOptions = 0;
    mArticle.clear();
    mIdPrice.clear();
    mIdsModifications.clear();
    mListOptions->clear();

    setUnModified();
}

void DataOrderItem::initialization()
{
    mPrice = 0;
    mPricePurchase = 0;
    mCount = 0;
    mCountOptions = 0;
    mDiscount = 0;
    mListOptions = new ListOptions();
}

QVariant DataOrderItem::getPropertyEdit(const QString &propertyName) const
{
    if (propertyName == "price")
        return mPrice;
    if (propertyName == "count")
        return mCount;
    if (propertyName == "discount")
        return mDiscount;
    if (propertyName == "name")
        return getName();
    if (propertyName == "article")
        return article();
    if (propertyName == "note")
        return property("note").toString();

    return DataItem::getPropertyEdit(propertyName);
}

bool DataOrderItem::setEditDataByPropertyName(const QString &propertyName, const QVariant &data)
{
    if (propertyName == "price") {
        setPrice(data.toDouble());
        return true;
    }
    if (propertyName == "count") {
        setCount(data.toDouble());
        return true;
    }
    if (propertyName == "discount") {
        setDiscount(data.toDouble());
        return true;
    }
    if (propertyName == "name") {
        setName(data.toString());
        return true;
    }
    if (propertyName == "article") {
        setArticle(data.toString());
        return true;
    }
    if (propertyName == "note") {
        setNote(data.toString());
        return true;
    }

    return DataItem::setEditDataByPropertyName(propertyName, data);
}

bool DataOrderItem::getIsEditableByPropertyName(const QString &propertyName)
{
    if (propertyName == "price")
        return true;
    if (propertyName == "count")
        return true;
    if (propertyName == "discount")
        return true;
    if (propertyName == "name")
        return true;
    if (propertyName == "article")
        return true;
    if (propertyName == "note")
        return true;

    return false;
}

void DataOrderItem::convertInBaseCurrency()
{
    if (mCurrency != SEConfig::getMainInfo()->baseCurrency()) {
        setPrice(SEConfig::priceConvertToBaseCurrency(price(), mCurrency));
        mCurrency = SEConfig::getMainInfo()->baseCurrency();
    }
}

void DataOrderItem::setIdPrice(const QString &idPrice)
{
    if (mIdPrice != idPrice) {
        mIdPrice = idPrice;
        setModified("idPrice");
    }
}

void DataOrderItem::setArticle(const QString &article)
{
    if (mArticle != article) {
        mArticle = article;
        setModified("article");
    }
}

void DataOrderItem::setIdsModifications(const QString &idsModifications)
{
    if (mIdsModifications != idsModifications) {
        mIdsModifications = idsModifications;
        setModified("idsModifications");
    }
}

void DataOrderItem::setPrice(const double &price)
{
    if (mPrice != price) {
        mPrice = price;
        setModified("price");
    }
}

void DataOrderItem::setPricePurchase(const double &price)
{
    if (mPricePurchase != price) {
        mPricePurchase = price;
        setModified("pricePurchase");
    }
}

void DataOrderItem::setDiscount(const double &discount)
{
    if (mDiscount != discount) {
        mDiscount = discount;
        setModified("discount");
    }
}

void DataOrderItem::setDiscountPercent(const double &discountPercent)
{
    setDiscount((mPrice * mCount) * discountPercent / 100);
}

void DataOrderItem::setCount(const double &count)
{
    if (mCount != count) {
        mCount = count;
        setModified("count");
    }
}

void DataOrderItem::setCurrency(const QString &currency)
{
    mCurrency = currency;
}

const QString DataOrderItem::idPrice() const
{
    return mIdPrice;
}

const QString DataOrderItem::article() const
{
    return mArticle;
}

const QString DataOrderItem::originalName() const
{
    return mOriginalName;
}

const QString DataOrderItem::idsModifications() const
{
    return mIdsModifications;
}

double DataOrderItem::price() const
{
    return mPrice;
}

double DataOrderItem::pricePurchase() const
{
    return mPricePurchase;
}

double DataOrderItem::discount() const
{
    return mDiscount;
}

double DataOrderItem::discountPercent()
{
    if (mPrice)
        return mDiscount / (mPrice / 100);
    return 0;
}

double DataOrderItem::count() const
{
    return mCount;
}

const QString DataOrderItem::currency() const
{
    return mCurrency;
}

double DataOrderItem::cost() const
{
    if (mPrice > mDiscount)
        return mCount * (mPrice - mDiscount);
    else return 0;
}

ListOptions *DataOrderItem::listOptions() const
{
    return mListOptions;
}

int DataOrderItem::getCountOptions() const
{
    return mCountOptions;
}

const QString DataOrderItem::priceTitle() const
{
    return SEConfig::priceCurrency(mPrice, mCurrency);
}

const QString DataOrderItem::countTitle() const
{
    QString s = QString();
    if (mCount >= 0)
        s = QString::number(mCount, 'f', 2);
    if (!mMeasurement.isEmpty())
        return s + mMeasurement;
    return s;
}

const QString DataOrderItem::costTitle() const
{
    return SEConfig::priceCurrency(cost(), mCurrency);
}

const QString DataOrderItem::discountTitle()
{
    return QString::number(mCount * mDiscount, 'f', 2) +
            "(" + QString::number(discountPercent(), 'f', 2) + "%)";
}

