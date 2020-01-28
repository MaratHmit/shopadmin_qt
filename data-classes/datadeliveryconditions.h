#ifndef DATADELIVERYPARAM_H
#define DATADELIVERYPARAM_H

#include "dataitem.h"

class DataDeliveryConditions : public DataItem
{
    Q_OBJECT
    Q_PROPERTY(QString typeParam READ titleTypeParam)
    Q_PROPERTY(QString decriptionPrice READ titleDescriptionPrice)
    Q_PROPERTY(QString minValue READ titleMinValue)
    Q_PROPERTY(QString maxValue READ titleMaxValue)
    Q_PROPERTY(int priority READ priority)

public:
    DataDeliveryConditions();
    DataDeliveryConditions(const DataDeliveryConditions *d);
    DataDeliveryConditions(const QJsonObject &jsonobject);

    void copy(const DataDeliveryConditions *d);
    void setFromJSONObject(const QJsonObject &jsonobject);
    QJsonObject getAsJsonObject() const;
    void clearData();
    void initialization();

    void setTypeParam(const QString &typeParam);
    void setPrice(const qreal &price);
    void setMinValue(const qreal &minValue);
    void setMaxValue(const qreal &maxValue);
    void setPriority(const int &priority);
    void setOperation(const QString &operation);
    void setTypePrice(const QString &typePrice);

    QString typeParam() const;
    qreal price() const;
    qreal minValue() const;
    qreal maxValue() const;
    int priority() const;
    QString operation() const;
    QString typePrice() const;

    const QString titleTypeParam() const;
    const QString titleDescriptionPrice() const;
    const QString titleMinValue() const;
    const QString titleMaxValue() const;

private:
    QString mTypeParam;
    qreal mPrice;
    qreal mMinValue;
    qreal mMaxValue;
    int mPriority;
    QString mOperation;
    QString mTypePrice;
};

typedef SEVector<DataDeliveryConditions *> ListDelivConditions;

#endif // DATADELIVERYPARAM_H
