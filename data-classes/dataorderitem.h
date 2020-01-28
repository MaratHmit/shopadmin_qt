#ifndef DATAORDERITEM_H
#define DATAORDERITEM_H

#include "dataitem.h"
#include "dataproduct.h"

class DataOrderItem : public DataItem
{
    Q_OBJECT
    Q_PROPERTY(QString article READ article)
    Q_PROPERTY(QString price READ priceTitle)
    Q_PROPERTY(QString count READ countTitle)
    Q_PROPERTY(QString cost READ costTitle)
    Q_PROPERTY(QString discount READ discountTitle)

public:
    DataOrderItem();
    DataOrderItem(const DataOrderItem *d);
    DataOrderItem(const DataProduct *d);
    DataOrderItem(const QJsonObject &jsonobject);
    ~DataOrderItem();

    void copy(const DataItem *d);
    void copy(const DataOrderItem *d);
    void copy(const DataProduct *d);
    void setFromJSONObject(const QJsonObject &jsonobject);
    QJsonObject getAsJsonObject() const;
    void clearData();
    void initialization();

    QVariant getPropertyEdit(const QString &propertyName) const;
    bool setEditDataByPropertyName(const QString &propertyName, const QVariant &data);
    bool getIsEditableByPropertyName(const QString &propertyName);    

    void convertInBaseCurrency();

    void setIdPrice(const QString &idPrice);
    void setArticle(const QString &article);
    void setIdsModifications(const QString &idsModifications);
    void setPrice(const double &price);
    void setPricePurchase(const double &price);
    void setDiscount(const double &discount);
    void setDiscountPercent(const double &discountPercent);
    void setCount(const double &count);
    void setCurrency(const QString &currency);

    const QString idPrice() const;
    const QString article() const;
    const QString originalName() const;
    const QString idsModifications() const;
    double price() const;
    double pricePurchase() const;
    double discount() const;
    double discountPercent();
    double count() const;
    const QString currency() const;
    double cost() const;
    ListOptions *listOptions() const;
    int getCountOptions() const;

    const QString priceTitle() const;
    const QString countTitle() const;
    const QString costTitle() const;
    const QString discountTitle();

private:
    QString mIdPrice;
    QString mArticle;
    QString mIdsModifications;
    QString mOriginalName;
    double mPrice;
    double mPricePurchase;
    double mDiscount;
    double mCount;
    QString mCurrency;
    QString mMeasurement;
    int mCountOptions;
    ListOptions *mListOptions;

};

typedef SEVector<DataOrderItem *> ListOrderItems;

#endif // DATAORDERITEM_H
