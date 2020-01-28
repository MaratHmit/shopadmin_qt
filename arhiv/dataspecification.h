#ifndef DATASPECIFICATION_H
#define DATASPECIFICATION_H

#include "data.h"

class DataSpecification : public Data
{
public:
    DataSpecification(QWidget *parent = 0);

    void setFromJSONObject(const QJsonObject &jsonobject);

    void setIdFeature(const int &idFeature);
    void setType(const QString &type);
    void setMeasure(const QString &measure);
    void setValueIdList(const int &valueIdList);
    void setValueList(const QString &valueList);
    void setValueNumber(const qreal &valueNumber);
    void setValueBool(const bool &valueBool);
    void setValueString(const QString &valueString);
    void setSortIndexGroup(const int &sortIndexGroup);
    void setColor(const QString &color);    

    int idFeature() const;
    QString type() const;
    QString measure() const;
    int valueIdList() const;
    QString valueList() const;
    qreal valueNumber() const;
    bool valueBool() const;
    QString valueString() const;
    int sortIndexGroup() const;
    QString color() const;

private:
    int mIdFeature;
    QString mType;
    QString mMeasure;
    int mValueIdList;
    QString mValueList;
    qreal mValueNumber;
    bool mValueBool;
    QString mValueString;
    int mSortIndexGroup;
    QString mColor;
};

typedef TemplateListData<DataSpecification *> ListSpecifications;


#endif // DATASPECIFICATION_H
