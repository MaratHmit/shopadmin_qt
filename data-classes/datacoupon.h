#ifndef DATACOUPON_H
#define DATACOUPON_H

#include <QString>

#include "dataproduct.h"
#include "dataproductsgroup.h"

class DataCoupon : public DataItem        
{
    Q_OBJECT
    Q_PROPERTY(QString type READ typeTitle)
    Q_PROPERTY(QString discount READ discountTitle)
    Q_PROPERTY(QString currencyCode READ currencyCode)
    Q_PROPERTY(QDate timeEnd READ timeEnd)
    Q_PROPERTY(QString minSum READ minSumTitle)
    Q_PROPERTY(int count READ count)
    Q_PROPERTY(QString isRegUser READ isRegUserTitle)

public:
    DataCoupon();
    DataCoupon(const DataCoupon *d);
    DataCoupon(const QJsonObject &jsonobject);
    ~DataCoupon();

    void clearData();
    void copy(const DataItem *d);
    void copy(const DataCoupon *d);
    void setFromJSONObject(const QJsonObject &jsonobject);
    QJsonObject getAsJsonObject() const;
    void initialization();
    
    void setType(const QString &type);
    void setDiscount(const qreal &discount);
    void setCurrencyCode(const QString &currencyCode);
    void setTimeEnd(const QDate &timeEnd);
    void setCount(const int &count);
    void setIsRegUser(const bool &isRegUser);
    void setMinSum(const qreal &minSum);
    void setIdUser(const QString &idUser);

    const QString type() const;
    const QString typeTitle() const;
    qreal discount() const;
    const QString discountTitle() const;
    const QString currencyCode() const;
    const QDate timeEnd() const;
    qreal minSum() const;
    const QString minSumTitle() const;
    int count() const;
    bool isRegUser() const;
    const QString isRegUserTitle() const;
    ListProducts *listProducts() const;
    ListProductsGroups *listGroups() const;
    const QString idUser() const;
    const QString userName() const;

    bool isModified() const;

private:
    QString mType;
    qreal mDiscount;
    QString mCurrencyCode;
    QDate mTimeEnd;
    qreal mMinSum;
    QString mIdUser;
    QString mUserName;
    int mCount;
    bool mIsRegUser;
    ListProducts *mListProducts;
    ListProductsGroups *mListGroups;

};

typedef SEVector<DataCoupon *> ListCoupons;

#endif // DATACOUPON_H
