#ifndef DATAFEATURE_H
#define DATAFEATURE_H

#include <QColor>

#include "dataitem.h"

class DataFeature;
typedef SEVector<DataFeature *> ListFeatures;

class DataFeature : public DataItem
{
    Q_OBJECT
    Q_PROPERTY(QString measure READ measure)
    Q_PROPERTY(QString type READ typeTitle)
    Q_PROPERTY(QPixmap icoType READ icoType)
    Q_PROPERTY(QString color READ hexColor)
    Q_PROPERTY(QString namesTypesGoods READ namesTypesGoods)

public:
    DataFeature();
    DataFeature(const DataFeature *d);
    DataFeature(const QJsonObject &jsonobject);

    void copy(const DataFeature *d);

    void setMeasure(const QString &measurement);
    void setIsSEO(const bool &isSEO);
    void setHEXColor(const QString &hexColor);
    void setIdFeature(const QString &idFeature);    
    void setSortIndexGroup(const int &sortIndexGroup);

    virtual void setFromJSONObject(const QJsonObject &jsonobject);
    QJsonObject getAsJsonObject() const;

    QString measure() const;
    bool isSEO() const;    
    QString hexColor() const;
    const QColor color() const;
    QString idFeature() const;    
    int sortIndexGroup() const;
    bool isColorType() const;
    bool isListType() const;    
    QString namesTypesGoods() const;

    const QPixmap icoType() const;
    const QColor getColorByPropertyName(const QString &propertyName) const;
    const QVariant getPropertyDecoration(const QString &) const;    
    const QVariant getPropertyDisplay(const QString &propertyName) const;

    const QString typeTitle() const;

    void clearData();
    void initialization();

private:        
    QString mIdFeature;    
    QString mMeasure;
    QString mHEXColor;
    QString mNamesTypesGoods;
    bool mIsSEO;
    int mSortIndexGroup;
    QPixmap mIcoIsYAMraket;

};

#endif // DATAFEATURE_H
