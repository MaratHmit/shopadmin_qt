#ifndef DATADISCOUNT_H
#define DATADISCOUNT_H

#include "dataitem.h"

class DataDiscount : public DataItem
{
    Q_OBJECT

    Q_PROPERTY(QString stepTime READ stepTimeTitle)
    Q_PROPERTY(QString stepDiscount READ stepDiscountTitle)
    Q_PROPERTY(QString dateTimeFrom READ dateTimeFromTitle)
    Q_PROPERTY(QString dateTimeTo READ dateTimeToTitle)
    Q_PROPERTY(QString sumFrom READ sumFromTitle)
    Q_PROPERTY(QString sumTo READ sumToTitle)
    Q_PROPERTY(QString countFrom READ countFromTitle)
    Q_PROPERTY(QString countTo READ countToTitle)
    Q_PROPERTY(QString discount READ discountTitle)    
    Q_PROPERTY(QString week READ weekTitle)
public:
    DataDiscount();    
    DataDiscount(const DataDiscount *d);
    DataDiscount(const QJsonObject &jsonobject);
    ~DataDiscount();

    void initialization();
    void clearData();
    void copy(const DataItem *d);
    void copy(const DataDiscount *d);
    void setFromJSONObject(const QJsonObject &jsonobject);
    QJsonObject getAsJsonObject() const;

    void setAction(const QString &action);
    void setStepTime(const int &stepTime);
    void setStepDiscount(const qreal &stepDiscount);
    void setDateTimeFrom(const QDateTime &dateTimeFrom);
    void setDateTimeTo(const QDateTime &dateTimeTo);
    void setSumFrom(const qreal &sumFrom);
    void setSumTo(const qreal &sumTo);
    void setCountFrom(const qreal &countFrom);
    void setCountTo(const qreal &countTo);
    void setDiscount(const qreal &discount);
    void setTypeDiscount(const QString &typeDiscount);
    void setWeek(const QString &week);
    void setTypeSum(const int &typeSum);

    const QString action() const;
    int stepTime() const;
    qreal stepDiscount() const;
    const QDateTime dateTimeFrom() const;
    const QDateTime dateTimeTo() const;
    qreal sumFrom() const;
    qreal sumTo() const;
    qreal countFrom() const;
    qreal countTo() const;
    qreal discount() const;
    const QString typeDiscount() const;
    const QString week() const;
    int typeSum() const;

    const QString stepTimeTitle() const;
    const QString stepDiscountTitle() const;
    const QString dateTimeFromTitle() const;
    const QString dateTimeToTitle() const;
    const QString sumFromTitle() const;
    const QString sumToTitle() const;
    const QString countFromTitle() const;
    const QString countToTitle() const;
    const QString discountTitle() const;    
    const QString weekTitle() const;

    ListDataItems *listProducts() const;
    ListDataItems *listGroupsProducts() const;
    ListDataItems *listModificationsProducts() const;
    ListDataItems *listContacts() const;

    bool isModified() const;

private:    
    int mStepTime;
    qreal mStepDiscount;
    QDateTime mDateTimeFrom;
    QDateTime mDateTimeTo;
    QString mWeek;
    qreal mSumFrom;
    qreal mSumTo;
    qreal mCountFrom;
    qreal mCountTo;
    qreal mDiscount;
    QString mTypeDiscount;    
    int mTypeSum;
    ListDataItems *mListGroupsProducts;
    ListDataItems *mListProducts;    
    ListDataItems *mListModificationsProducts;
    ListDataItems *mListContacts;

};

typedef SEVector<DataDiscount *> ListDiscounts;

#endif // DATADISCOUNT_H
