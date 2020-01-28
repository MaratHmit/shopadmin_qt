#include "datamodification.h"

#include <QDebug>

DataModification::DataModification()
{
    initialization();
}

DataModification::DataModification(const DataModification *d)
{
    initialization();
    copy(d);
}

DataModification::DataModification(const QJsonObject &jsonobject)
{
    initialization();
    setFromJSONObject(jsonobject);
}

DataModification::~DataModification()
{
    mValues->deleteItems();
    delete mValues;    
    delete mImages;
}

void DataModification::initialization()
{
    DataItem::initialization();

    mValues = new ListFeatures();    
    mImages = new ListDataItems();
    mCount = -1;
    mPrice = 0;
    mPriceSmallOpt = 0;
    mPriceOpt = 0;
    mPricePurchase = 0;

}

void DataModification::copy(const DataModification *d)
{
    DataItem::copy(d);
    mCount = d->count();
    mPrice = d->price();
    mPriceSmallOpt = d->priceSmallOpt();
    mPriceOpt = d->priceOpt();
    mPricePurchase = d->pricePurchase();
    mArticle = d->article();
    mValues->deleteItems();    
    for (int i = 0; i < d->values()->size(); i++)
        mValues->append(new DataFeature(d->values()->at(i)));
    mImages->deleteItems();
    for (int i = 0; i < d->images()->size(); i++)
        mImages->append(new DataItem(d->images()->at(i)));
}

void DataModification::setFromJSONObject(const QJsonObject &jsonobject)
{
    DataItem::setFromJSONObject(jsonobject);
    if (!jsonobject["count"].isNull())
        mCount = jsonobject["count"].toDouble();
    else mCount = -1;
    mPrice = jsonobject["price"].toDouble();
    mPricePurchase = jsonobject["pricePurchase"].toDouble();
    mPriceSmallOpt = jsonobject["priceSmallOpt"].toDouble();
    mPriceOpt = jsonobject["priceOpt"].toDouble();
    mArticle = jsonobject["article"].toString();
    QJsonArray items = jsonobject["values"].toArray();
    foreach (const QJsonValue& value, items)
        mValues->append(new DataFeature(value.toObject()));
    items = jsonobject["images"].toArray();
    foreach (const QJsonValue& value, items)
        mImages->append(new DataItem(value.toObject()));

    setUnModified();
}

QJsonObject DataModification::getAsJsonObject() const
{
    QJsonObject obj = DataItem::getAsJsonObject();

    obj.insert("price", QJsonValue(mPrice));
    obj.insert("pricePurchase", QJsonValue(mPricePurchase));
    obj.insert("priceOpt", QJsonValue(mPriceOpt));
    obj.insert("priceSmallOpt", QJsonValue(mPriceSmallOpt));
    obj.insert("count", QJsonValue(mCount));
    obj.insert("article", QJsonValue(mArticle));
    mValues->setIsSetAllFields(isSetAllFields);
    obj.insert("values", QJsonValue(mValues->getAsJsonArray()));
    mImages->setIsSetAllFields(isSetAllFields);
    obj.insert("images", QJsonValue(mImages->getAsJsonArray()));

    return obj;
}

void DataModification::clearData()
{
    DataItem::clearData();

    mCount = -1;
    mPrice = 0;
    mPriceSmallOpt = 0;
    mPriceOpt = 0;
    mPricePurchase = 0;
    mArticle.clear();
    mValues->deleteItems();
    mImages->clear();
}

void DataModification::setCount(const qreal &count)
{
    if (mCount != count) {
        mCount = count;
        setModified("count");
    }
}

void DataModification::setPrice(const qreal &price)
{
    if (mPrice != price) {
        mPrice = price;
        setModified("price");
    }
}

void DataModification::setPricePurscase(const qreal &pricePurchase)
{
    if (mPricePurchase != pricePurchase) {
        mPricePurchase = pricePurchase;
        setModified("pricePurchase");
    }
}

void DataModification::setPriceSmallOpt(const qreal &priceSmallOpt)
{
    if (mPriceSmallOpt != priceSmallOpt) {
        mPriceSmallOpt = priceSmallOpt;
        setModified("priceSmallOpt");
    }
}

void DataModification::setPriceOpt(const qreal &priceOpt)
{
    if (mPriceOpt != priceOpt) {
        mPriceOpt = priceOpt;
        setModified("priceOpt");
    }
}

void DataModification::setArticle(const QString article)
{
    if (mArticle != article) {
        mArticle = article;
        setModified("article");
    }
}

qreal DataModification::count() const
{
    return mCount;
}

qreal DataModification::price() const
{
    return mPrice;
}

qreal DataModification::pricePurchase() const
{
    return mPricePurchase;
}

qreal DataModification::priceSmallOpt() const
{
    return mPriceSmallOpt;
}

qreal DataModification::priceOpt() const
{
    return mPriceOpt;
}

const QString &DataModification::article() const
{
    return mArticle;
}

ListDataItems *DataModification::images() const
{
    return mImages;
}

ListFeatures *DataModification::values() const
{
    return mValues;
}

bool DataModification::isEqualValues(const DataModification *m) const
{
    if (m->values()->size() == mValues->size()) {
        bool result = true;
        bool *pFlags = new bool[mValues->size()];
        for (int i = 0; i < mValues->size(); i++)
            pFlags[i] = false;
        for (int i = 0; i < mValues->size(); i++)
            for (int j = 0; j < m->values()->size(); j++)
                pFlags[i] = pFlags[i] ||
                        (mValues->at(i)->getId() == m->values()->at(j)->getId());
        for (int i = 0; i < mValues->size(); i++)
            result = result && pFlags[i];
        delete [] pFlags;
        return result;
    } else
        return false;
    return true;
}

bool DataModification::isModified() const
{
    return DataItem::isModified() || mValues->isModified() || mImages->isModified();
}

void DataModification::setUnModified()
{
    DataItem::setUnModified();

    mValues->setUnModified();
    mImages->setUnModified();
}

