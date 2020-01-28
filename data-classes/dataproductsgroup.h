#ifndef DATAPRODUCTSGROUP_H
#define DATAPRODUCTSGROUP_H

#include "dataimages.h"
#include "datadiscount.h"

class DataProductsGroup : public DataImages
{
    Q_OBJECT
    Q_PROPERTY(QString sourcePriceTitle READ sourcePriceTitle)

public:
    DataProductsGroup();
    DataProductsGroup(const DataProductsGroup *group);
    DataProductsGroup(const QJsonObject &jsonobject);
    ~DataProductsGroup();

    void copy(const DataProductsGroup *group);

    ListDataItems *linksGroups() const;
    ListDataItems *similarGroups() const;
    ListDataItems *additionalSubgroups() const;
    ListDataItems *filters() const;
    ListDiscounts *discounts() const;
    QString idModificationGroupDef() const;
    const QString sourcePriceTitle() const;

    void setIdModificationGroupDef(const QString &idModificationGroupDef);

    void addLinkGroup(DataItem *item);
    void addSimilarGroup(DataItem *item);
    void addAdditionalSubgroup(DataItem *item);

    void setFromJSONObject(const QJsonObject &jsonobject);
    QJsonObject getAsJsonObject() const;

    void clearData();
    bool isModified() const;

    void setUnModifiedAll();

protected:
    void initialization();

private:    
    QString mIdModificationGroupDef;
    ListDataItems *mLinksGroups;  // связанные группы
    ListDataItems *mSimilarGroups;  // похожие группы
    ListDataItems *mAdditionalSubgroups;  // доп. подгруппы
    ListDataItems *mFilters; // фильтры параметров
    ListDiscounts *mDiscounts; // скидки

};

typedef SEVector<DataProductsGroup *> ListProductsGroups;

//Q_DECLARE_METATYPE (DataProductsGroup)

#endif // DATAPRODUCTSGROUP_H
