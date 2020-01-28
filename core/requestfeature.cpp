#include "requestfeature.h"

RequestFeature::RequestFeature()
{
    clear();
}

void RequestFeature::setIsList(const bool &isList)
{
    mIsList = isList;
}

void RequestFeature::setIdFeature(const QString &idFeature)
{
    mIdFeature = idFeature;
}

void RequestFeature::setIsSEO(const QString &id, const bool &isSEO)
{
    mSEOParams.insert(id, isSEO);
}

QJsonObject RequestFeature::getAsJsonObject() const
{
    QJsonObject obj = Request::getAsJsonObject();

    if (mIsList)
        obj.insert("isList", QJsonValue(true));
    if (!mIdFeature.isEmpty())
        obj.insert("idFeature", QJsonValue(mIdFeature));
    if (!mSEOParams.isEmpty()) {
        QJsonArray seo;
        QMap<QString, bool>::const_iterator it = mSEOParams.constBegin();
        for(;it != mSEOParams.constEnd(); ++it)
        {
           QJsonObject objItem;
           objItem.insert("id", QJsonValue(it.key()));
           objItem.insert("isSEO", QJsonValue(it.value()));
           seo.append(QJsonValue(objItem));
        }
        obj.insert("itemsSEO", QJsonValue(seo));
    }

    return obj;
}

void RequestFeature::clear()
{
    Request::clear();

    mIsList = false;
    mIdFeature.clear();
    mSEOParams.clear();    
}
