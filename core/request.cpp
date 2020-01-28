#include "request.h"
#include "seconfig.h"

Request::Request()
{
    clear();
}

void Request::addId(const QString &id)
{
    mIds.append(id);
}

void Request::addId(const QStringList &idList)
{
    for (int i = 0; i < idList.size(); i++)
        addId(idList.at(i));
}

void Request::setIdParent(const QString &idParent)
{
    mIdParent = idParent;
}

void Request::setIdCountry(const QString &idCountry)
{
    mIdCountry = idCountry;
}

void Request::setLimit(const int &limit)
{
    mLimit = limit;
}

void Request::setOffset(const int &offset)
{
    mOffset = offset;
}

void Request::setSortBy(const QString &sortBy)
{
    mSortBy = sortBy;
}

void Request::setAscendingOrder(const bool &isAscending)
{
    mAscendingOrder = isAscending;
}

void Request::setSearchText(const QString &searchText)
{
    mSearchText = searchText;
}


void Request::setIdPayment(const QString &idPayment)
{
    mIdPayment = idPayment;
}

void Request::addSortIndex(const QString &id, const int &index)
{
    mSortPostions.insert(id, index);
}

void Request::setFilter(const QString &filter)
{
    mFilter = filter;
}

void Request::clearFilter()
{
    mFilter.clear();
}

void Request::addStringValue(const QString &key, const QString &value)
{
    if (mDynValues.contains(key))
        mDynValues.remove(key);
    mDynValues.insert(key, value);
}

void Request::clearStringValues()
{
    mDynValues.clear();
}

const QString Request::filter() const
{
    return mFilter;
}

const QString Request::search() const
{
    return mSearchText;
}

QJsonObject Request::getAsJsonObject() const
{
    QJsonObject obj;
    QJsonArray ids;

    for (int i = 0; i < mIds.size(); i++)
        ids.append(QJsonValue(mIds.at(i)));
    obj.insert("ids", QJsonValue(ids));

    if (!mIdParent.isNull())
        obj.insert("idParent", QJsonValue(mIdParent));
    if (!mFilter.isEmpty())
        obj.insert("filter", QJsonValue(mFilter));
    if (!mIdCountry.isEmpty())
        obj.insert("idCountry", QJsonValue(mIdCountry));
    obj.insert("limit", QJsonValue(mLimit));
    obj.insert("offset", QJsonValue(mOffset));
    if (mAscendingOrder)
        obj.insert("sortOrder", QJsonValue("asc"));
    else obj.insert("sortOrder", QJsonValue("desc"));
    if (!mSortBy.isEmpty())
        obj.insert("sortBy", QJsonValue(mSortBy));
    if (!mSearchText.isEmpty())
        obj.insert("searchText", QJsonValue(mSearchText));
    if (!mIdPayment.isEmpty())
        obj.insert("idPayment", QJsonValue(mIdPayment));
    QJsonArray sorts;
    QMap<QString, int>::const_iterator it = mSortPostions.constBegin();
    while (it != mSortPostions.constEnd()) {
       QJsonObject objItem;
       objItem.insert("id", QJsonValue(it.key()));
       objItem.insert("index", QJsonValue(it.value()));
       sorts.append(QJsonValue(objItem));
       ++it;
    }
    if (sorts.count())
        obj.insert("sortIndexes", QJsonValue(sorts));
    QMap<QString, QString>::const_iterator itS = mDynValues.constBegin();
    while (itS != mDynValues.constEnd()) {
       obj.insert(itS.key(), QJsonValue(itS.value()));
       ++itS;
    }
    return obj;
}

QString Request::getAsJsonString() const
{
    QJsonDocument doc;
    doc.setObject(getAsJsonObject());
    return QString(doc.toJson(QJsonDocument::Compact));
}

void Request::clear()
{    
    mLimit = 100;
    mOffset = 0;
    mIdParent.clear();
    mIds.clear();
    mIdsGroups.clear();
    mAscendingOrder = true;
    mSortBy.clear();
    mSearchText.clear();
    mAscendingOrder = true;
    mIdPayment.clear();
    mSortPostions.clear();
    mFilter.clear();
}
