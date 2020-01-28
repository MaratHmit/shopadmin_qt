#ifndef DATACURRENCY_H
#define DATACURRENCY_H

#include "dataitem.h"

class DataCurrency : public DataItem
{
    Q_OBJECT
    Q_PROPERTY(QDate rateDate READ rateDate)
    Q_PROPERTY(QString rate READ strRate)
    Q_PROPERTY(QString rateDisplay READ rateDisplay)

public:
    DataCurrency();
    DataCurrency(const DataCurrency *d);
    DataCurrency(const QJsonObject &jsonobject);

    void copy(const DataCurrency *d);
    void copy(const DataItem *d);
    void clearData();
    void initialization();

    void setPreifx(const QString &prefix);
    void setSuffix(const QString &suffix);
    void setCbrCode(const QString &cbrCode);
    void setMinSum(const QString &minSum);
    void setMinSum(const double &minSum);
    void setRate(const double &rate);
    void setRateDate(const QDate &rateDate);    

    QString prefix() const;
    QString suffix() const;
    QString cbrCode() const;
    double minSum() const;
    double rate() const;
    QString strRate() const;
    QDate rateDate() const;
    QString rateDisplay() const;
    bool isCanAutoRate() const;

    const QColor getColorByPropertyName(const QString &propertyName) const;
    void setFromJSONObject(const QJsonObject &jsonobject);
    QJsonObject getAsJsonObject() const;

private:
    QString mPrefix;
    QString mSuffix;
    QString mCbrCode;
    double mMinSum;
    double mRate;
    QDate mRateDate;
    QString mRateDisplay;
    bool mIsCanAutoRate;
};

typedef SEVector<DataCurrency *> ListCurrencies;

#endif // DATACURRENCY_H
