#ifndef DATAMODIFICATION_H
#define DATAMODIFICATION_H

#include "datafeature.h"

class DataModification : public DataItem
{
public:
    DataModification();
    DataModification(const DataModification *d);
    DataModification(const QJsonObject &jsonobject);
    ~DataModification();

    void initialization();

    void copy(const DataModification *d);
    void setFromJSONObject(const QJsonObject &jsonobject);
    QJsonObject getAsJsonObject() const;
    void clearData();

    void setCount(const qreal &count);
    void setPrice(const qreal &price);
    void setPricePurscase(const qreal &pricePurchase);
    void setPriceSmallOpt(const qreal &priceSmallOpt);
    void setPriceOpt(const qreal &priceOpt);
    void setArticle(const QString article);

    qreal count() const;
    qreal price() const;
    qreal pricePurchase() const;
    qreal priceSmallOpt() const;
    qreal priceOpt() const;
    const QString &article() const;

    ListDataItems *images() const;
    ListFeatures *values() const;

    bool isEqualValues(const DataModification *m) const;
    bool isModified() const;
    void setUnModified();

private:
    QString mArticle;
    ListFeatures *mValues;
    ListDataItems *mImages;
    qreal mCount;
    qreal mPrice;
    qreal mPricePurchase;
    qreal mPriceSmallOpt;
    qreal mPriceOpt;
};

typedef SEVector<DataModification *> ListModifications;

#endif // DATAMODIFICATION_H
