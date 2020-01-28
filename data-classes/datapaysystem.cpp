#include "datapaysystem.h"

#include <QDebug>

DataPaySystem::DataPaySystem()
{
    initialization();
}

DataPaySystem::DataPaySystem(const DataPaySystem *d)
{
    initialization();
    copy(d);
}

DataPaySystem::DataPaySystem(const QJsonObject &jsonobject)
{
    initialization();
    setFromJSONObject(jsonobject);
}

DataPaySystem::~DataPaySystem()
{
    delete mPaysReqs;
    delete mHosts;
    delete mFilters;
}

void DataPaySystem::setFromJSONObject(const QJsonObject &jsonobject)
{
    DataItem::setFromJSONObject(jsonobject);
    setIsExtBlank(jsonobject["isExtBlank"].toBool());
    setIsAuthorize(jsonobject["isAuthorize"].toBool());
    setIsAdvance(jsonobject["isAdvance"].toBool());
    setIsTestMode(jsonobject["isTestMode"].toBool());
    setUrlHelp(jsonobject["urlHelp"].toString());
    setIdentifier(jsonobject["identifier"].toString());
    setPageSuccess(jsonobject["pageSuccess"].toString());
    setPageFail(jsonobject["pageFail"].toString());
    setPageMainInfo(jsonobject["pageMainInfo"].toString());
    setPageResult(jsonobject["pageResult"].toString());
    setPageBlank(jsonobject["pageBlank"].toString());
    mPaysReqs->deleteItems();
    if (jsonobject["params"].isArray()) {
        QJsonArray params = jsonobject["params"].toArray();
        foreach (const QJsonValue& value, params)
            mPaysReqs->append(new DataPayRequisite(value.toObject()));
    }
    mFilters->deleteItems();
    if (jsonobject["filters"].isArray()) {
        QJsonArray params = jsonobject["filters"].toArray();
        foreach (const QJsonValue& value, params)
            mFilters->append(new DataProduct(value.toObject()));
    }
    mHosts->deleteItems();
    if (jsonobject["hosts"].isArray()) {
        QJsonArray params = jsonobject["hosts"].toArray();
        foreach (const QJsonValue& value, params) {
            mHosts->append(new DataItem(value.toObject()));            
        }
    }
    setUnModifiedAll();
}

QJsonObject DataPaySystem::getAsJsonObject() const
{
    QJsonObject result = DataItem::getAsJsonObject();
    if (isSetAllFields || modifiedFields.contains("isExtBlank"))
        result.insert("isExtBlank", QJsonValue(mIsExtBlank));
    if (isSetAllFields || modifiedFields.contains("isAuthorize"))
        result.insert("isAuthorize", QJsonValue(mIsAuthorize));
    if (isSetAllFields || modifiedFields.contains("isAdvance"))
        result.insert("isAdvance", QJsonValue(mIsAdvance));
    if (isSetAllFields || modifiedFields.contains("isTestMode"))
        result.insert("isTestMode", QJsonValue(mIsTestMode));
    if (!mUrlHelp.isNull())
        result.insert("urlHelp", QJsonValue(mUrlHelp));
    if (!mIdentifier.isNull())
        result.insert("identifier", QJsonValue(mIdentifier));
    if (!mPageSuccess.isNull())
        result.insert("pageSuccess", QJsonValue(mPageSuccess));
    if (!mPageFail.isNull())
        result.insert("pageFail", QJsonValue(mPageFail));
    if (!mPageMainInfo.isNull())
        result.insert("pageMainInfo", QJsonValue(mPageMainInfo));
    if (!mPageResult.isNull())
        result.insert("pageResult", QJsonValue(mPageResult));
    if (!mPageBlank.isNull())
        result.insert("pageBlank", QJsonValue(mPageBlank));
    if (mPaysReqs->isModified()) {
        mPaysReqs->setIsSetAllFields(1);
        result.insert("params", QJsonValue(mPaysReqs->getAsJsonArray()));
    }
    if (mHosts->isModified()) {
        mHosts->setIsSetAllFields(1);
        result.insert("hosts", QJsonValue(mHosts->getAsJsonArray()));
    }
    if (mFilters->isModified()) {
        mFilters->setIsSetAllFields(1);
        result.insert("filters", QJsonValue(mFilters->getAsJsonArray()));
    }

    return result;
}

void DataPaySystem::clearData()
{
    DataItem::clearData();
    mIsExtBlank = false;
    mIsAuthorize = false;
    mIsAdvance = false;
    mIsTestMode = false;
    mUrlHelp = QString();
    mIdentifier = QString();
    mPageSuccess = QString();
    mPageFail = QString();
    mPageBlank = QString();
    mPageResult = QString();
    mPageMainInfo = QString();
    mPaysReqs->deleteItems();
    mHosts->deleteItems();
    mFilters->deleteItems();
}

void DataPaySystem::copy(const DataPaySystem *d)
{
    DataItem::copy(d);

    mIsExtBlank = d->mIsExtBlank;
    mIsAuthorize = d->mIsAuthorize;
    mIsAdvance = d->mIsAdvance;
    mIsTestMode = d->mIsTestMode;
    mUrlHelp = d->mUrlHelp;
    mIdentifier = d->mIdentifier;
    mPageSuccess = d->mPageSuccess;
    mPageFail = d->mPageFail;
    mPageBlank = d->mPageBlank;
    mPageResult = d->mPageResult;
    mPageMainInfo = d->mPageMainInfo;
    mPaysReqs->deleteItems();
    for (int i = 0; i < d->mPaysReqs->size(); ++i)
        mPaysReqs->append(new DataItem(d->mPaysReqs->at(i)));
    mHosts->deleteItems();
    for (int i = 0; i < d->mHosts->size(); ++i)
        mHosts->append(new DataItem(d->mHosts->at(i)));
    mFilters->deleteItems();
    for (int i = 0; i < d->mFilters->size(); ++i)
        mFilters->append(new DataProduct(d->mFilters->at(i)));
}

void DataPaySystem::initialization()
{
    mPaysReqs = new ListDataItems();
    mHosts = new ListDataItems();
    mFilters = new ListProducts();
    DataItem::initialization();
}

void DataPaySystem::setIsExtBlank(const bool &isExtBlank)
{
    if (mIsExtBlank != isExtBlank) {
        mIsExtBlank = isExtBlank;
        setModified("isExtBlank");
    }
}

void DataPaySystem::setIsAuthorize(const bool &isAuthorize)
{
    if (mIsAuthorize != isAuthorize) {
        mIsAuthorize = isAuthorize;
        setModified("isAuthorize");
    }
}

void DataPaySystem::setIsAdvance(const bool &isAdvance)
{
    if (mIsAdvance != isAdvance) {
        mIsAdvance = isAdvance;
        setModified("isAdvance");
    }
}

void DataPaySystem::setIsTestMode(const bool &isTestMode)
{
    if (mIsTestMode != isTestMode) {
        mIsTestMode = isTestMode;
        setModified("isTestMode");
    }
}

void DataPaySystem::setUrlHelp(const QString &urlHelp)
{
    if (mUrlHelp != urlHelp) {
        mUrlHelp = urlHelp;
        setModified("urlHelp");
    }
}

void DataPaySystem::setIdentifier(const QString &identifier)
{
    if (mIdentifier != identifier) {
        mIdentifier = identifier;
        setModified("identifier");
    }
}

void DataPaySystem::setPageSuccess(const QString &pageSuccess)
{
    if (mPageSuccess != pageSuccess) {
        mPageSuccess = pageSuccess;
        setModified("pageSuccess");
    }
}

void DataPaySystem::setPageFail(const QString &pageFail)
{
    if (mPageFail != pageFail) {
        mPageFail = pageFail;
        setModified("pageFail");
    }
}

void DataPaySystem::setPageBlank(const QString &pageBlank)
{
    if (mPageBlank != pageBlank) {
        mPageBlank = pageBlank;
        setModified("pageBlank");
    }
}

void DataPaySystem::setPageResult(const QString &pageResult)
{
    if (mPageResult != pageResult) {
        mPageResult = pageResult;
        setModified("pageResult");
    }
}

void DataPaySystem::setPageMainInfo(const QString &pageMainInfo)
{
    if (mPageMainInfo != pageMainInfo) {
        mPageMainInfo = pageMainInfo;
        setModified("pageMainInfo");
    }
}

bool DataPaySystem::isExtBlank() const
{
    return mIsExtBlank;
}

bool DataPaySystem::isAuthorize() const
{
    return mIsAuthorize;
}

bool DataPaySystem::isAdvance() const
{
    return mIsAdvance;
}

bool DataPaySystem::isTestMode() const
{
    return mIsTestMode;
}

QString DataPaySystem::urlHelp() const
{
    return mUrlHelp;
}

QString DataPaySystem::identifier() const
{
    return mIdentifier;
}

QString DataPaySystem::pageSuccess() const
{
    return mPageSuccess;
}

QString DataPaySystem::pageFail() const
{
    return mPageFail;
}

QString DataPaySystem::pageBlank() const
{
    return mPageBlank;
}

QString DataPaySystem::pageResult() const
{
    return mPageResult;
}

QString DataPaySystem::pageMainInfo() const
{
    return mPageMainInfo;
}

ListDataItems *DataPaySystem::payReqs() const
{
    return mPaysReqs;
}

ListProducts *DataPaySystem::filters() const
{
    return mFilters;
}

ListDataItems *DataPaySystem::hosts() const
{
    return mHosts;
}

bool DataPaySystem::isModified() const
{
    return DataItem::isModified() || mHosts->isModified() || mPaysReqs->isModified() ||
            mFilters->isModified();
}

void DataPaySystem::setUnModifiedAll()
{
    DataItem::setUnModified();
    mPaysReqs->setUnModified();
    mFilters->setUnModified();
    mHosts->setUnModified();
}
