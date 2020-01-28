#include "dataproductsgroup.h"
#include "apiadapter.h"

#include <QMessageBox>

DataProductsGroup::DataProductsGroup()
{
    initialization();
}

DataProductsGroup::DataProductsGroup(const DataProductsGroup *group)
{
    initialization();
    copy(group);
}

DataProductsGroup::DataProductsGroup(const QJsonObject &jsonobject)
{
    initialization();
    setFromJSONObject(jsonobject);
}

DataProductsGroup::~DataProductsGroup()
{
    delete mLinksGroups;
    delete mFilters;
    delete mDiscounts;
    delete mSimilarGroups;
    delete mAdditionalSubgroups;
}

void DataProductsGroup::copy(const DataProductsGroup *group)
{
    DataImages::copy(group);
    mLinksGroups->clear();
    for (int i = 0; i < group->mLinksGroups->size(); i++) {
        DataItem *item = new DataItem(group->mLinksGroups->at(i));
        mLinksGroups->append(item);
    }
    mSimilarGroups->clear();
    for (int i = 0; i < group->mSimilarGroups->size(); i++) {
        DataItem *item = new DataItem(group->mSimilarGroups->at(i));
        mSimilarGroups->append(item);
    }
    mAdditionalSubgroups->clear();
    for (int i = 0; i < group->mAdditionalSubgroups->size(); i++) {
        DataItem *item = new DataItem(group->mAdditionalSubgroups->at(i));
        mAdditionalSubgroups->append(item);
    }
    mDiscounts->clear();
    for (int i = 0; i < group->discounts()->size(); i++)
        mDiscounts->append(new DataDiscount(group->discounts()->at(i)));
    mIdModificationGroupDef = group->mIdModificationGroupDef;
}

ListDataItems *DataProductsGroup::linksGroups() const
{
    return mLinksGroups;
}

ListDataItems *DataProductsGroup::similarGroups() const
{
    return mSimilarGroups;
}

ListDataItems *DataProductsGroup::additionalSubgroups() const
{
    return mAdditionalSubgroups;
}

ListDataItems *DataProductsGroup::filters() const
{
    return mFilters;
}

ListDiscounts *DataProductsGroup::discounts() const
{
    return mDiscounts;
}

QString DataProductsGroup::idModificationGroupDef() const
{
    return mIdModificationGroupDef;
}

const QString DataProductsGroup::sourcePriceTitle() const
{
    if (getId().isEmpty())
        return QString();

    if (property("sourcePrice").toString() == "FILE")
        return "прайс-лист";
    else return "office-zakaz";
}

void DataProductsGroup::setIdModificationGroupDef(const QString &idModificationGroupDef)
{
    if (mIdModificationGroupDef != idModificationGroupDef) {
        mIdModificationGroupDef = idModificationGroupDef;
        setModified("idModificationGroupDef");
    }
}

void DataProductsGroup::addLinkGroup(DataItem *item)
{
    if (mLinksGroups->existById(item))
        return;

    mLinksGroups->append(new DataItem(item));
}

void DataProductsGroup::addSimilarGroup(DataItem *item)
{
    if (mSimilarGroups->existById(item))
        return;

    mSimilarGroups->append(new DataItem(item));
}

void DataProductsGroup::addAdditionalSubgroup(DataItem *item)
{
    if (mAdditionalSubgroups->existById(item))
        return;

    mAdditionalSubgroups->append(new DataItem(item));
}

void DataProductsGroup::setFromJSONObject(const QJsonObject &jsonobject)
{
    DataImages::setFromJSONObject(jsonobject);

    mIdModificationGroupDef = jsonobject["idModificationGroupDef"].toString();
    // связанные группы
    if (jsonobject["linksGroups"].isArray()) {
        QJsonArray links = jsonobject["linksGroups"].toArray();
        foreach (const QJsonValue& value, links)
        {
            QJsonObject obj = value.toObject();
            DataProductsGroup *gr = new DataProductsGroup();
            gr->setFromJSONObject(obj);
            mLinksGroups->append(gr);
        }
    }
    // похожие группы
    if (jsonobject["similarGroups"].isArray()) {
        QJsonArray links = jsonobject["similarGroups"].toArray();
        foreach (const QJsonValue& value, links)
        {
            QJsonObject obj = value.toObject();
            DataProductsGroup *gr = new DataProductsGroup();
            gr->setFromJSONObject(obj);
            mSimilarGroups->append(gr);
        }
    }
    // доп. подгруппы
    if (jsonobject["additionalSubgroups"].isArray()) {
        QJsonArray links = jsonobject["additionalSubgroups"].toArray();
        foreach (const QJsonValue& value, links)
        {
            QJsonObject obj = value.toObject();
            DataProductsGroup *gr = new DataProductsGroup();
            gr->setFromJSONObject(obj);
            mAdditionalSubgroups->append(gr);
        }
    }
    // фильтры параметров
    if (jsonobject["parametersFilters"].isArray()) {
        QJsonArray links = jsonobject["parametersFilters"].toArray();
        foreach (const QJsonValue& value, links)
        {
            QJsonObject obj = value.toObject();
            DataItem *f = new DataItem();
            f->setFromJSONObject(obj);
            mFilters->append(f);
        }
    }
    // скидки
    mDiscounts->deleteItems();
    QJsonArray items = jsonobject["discounts"].toArray();
    foreach (const QJsonValue& value, items)
        mDiscounts->append(new DataDiscount(value.toObject()));
}

QJsonObject DataProductsGroup::getAsJsonObject() const
{
    QJsonObject obj = DataImages::getAsJsonObject();

    if (mLinksGroups->isModified()) {
        mLinksGroups->setIsSetAllFields(1);
        obj.insert("linksGroups", QJsonValue(mLinksGroups->getAsJsonArray()));
    }
    if (mSimilarGroups->isModified()) {
        mSimilarGroups->setIsSetAllFields(1);
        obj.insert("similarGroups", QJsonValue(mSimilarGroups->getAsJsonArray()));
    }
    if (mAdditionalSubgroups->isModified()) {
        mAdditionalSubgroups->setIsSetAllFields(1);
        obj.insert("additionalSubgroups", QJsonValue(mAdditionalSubgroups->getAsJsonArray()));
    }
    if (mDiscounts->isModified()) {
        mDiscounts->setIsSetAllFields(1);
        obj.insert("discounts", QJsonValue(mDiscounts->getAsJsonArray()));
    }
    if (mFilters->isModified()) {
        mFilters->setIsSetAllFields(1);
        obj.insert("parametersFilters", QJsonValue(mFilters->getAsJsonArray()));
    }
    obj.insert("idModificationGroupDef", QJsonValue(mIdModificationGroupDef));

    return obj;
}

void DataProductsGroup::clearData()
{
    DataImages::clearData();
    mLinksGroups->deleteItems();
    mSimilarGroups->deleteItems();
    mAdditionalSubgroups->deleteItems();
    mFilters->deleteItems();
    mIdModificationGroupDef.clear();
}

bool DataProductsGroup::isModified() const
{
     return DataImages::isModified() || mDiscounts->isModified() ||
             mLinksGroups->isModified() || mFilters->isModified() ||
             mSimilarGroups->isModified() || mAdditionalSubgroups->isModified();
}

void DataProductsGroup::setUnModifiedAll()
{
    DataImages::setUnModified();
    mLinksGroups->setUnModified();
    mFilters->setUnModified();
    mDiscounts->setUnModified();
    mSimilarGroups->setUnModified();
    mAdditionalSubgroups->setUnModified();
}

void DataProductsGroup::initialization()
{
    DataImages::initialization();

    mLinksGroups = new ListDataItems;
    mSimilarGroups = new ListDataItems;
    mAdditionalSubgroups = new ListDataItems;
    mFilters = new ListDataItems();
    mDiscounts = new ListDiscounts();

}
