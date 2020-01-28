#include <QMessageBox>

#include "datamain.h"
#include "seconfig.h"

DataMain::DataMain():
    DataItem()
{
    mListCurrency = new ListCurrencies;
    mIsManualCurrencyRate = 0;    
}

DataMain::~DataMain()
{
    delete mListCurrency;
}

void DataMain::setNameCompany(const QString &nameCompany)
{
    if (mNameCompany != nameCompany) {
        setModified("nameCompany");
        mNameCompany = nameCompany;
    }
}

void DataMain::setShopName(const QString &shopName)
{
    if (mShopName != shopName) {
        setModified("shopName");
        mShopName = shopName;
    }
}

void DataMain::setSubName(const QString &subName)
{
    if (mSubName != subName) {
        setModified("subName");
        mSubName = subName;
    }
}

void DataMain::setURLLogo(const QString &urlLogo)
{
    if (mURLLogo != urlLogo) {
        setModified("urlLogo");
        mURLLogo = urlLogo;
    }
}

void DataMain::setDomain(const QString &domain)
{
    if (mDomain != domain) {
        setModified("domain");
        mDomain = domain;
    }
}

void DataMain::setEmailSales(const QString &emailSales)
{
    if (mEmailSales != emailSales) {
        setModified("emailSales");
        mEmailSales = emailSales;
    }
}

void DataMain::setEmailSupport(const QString &emailSupport)
{
    if (mEmailSupport != emailSupport) {
        setModified("emailSupport");
        mEmailSupport = emailSupport;
    }
}

void DataMain::setBaseCurrency(const QString &baseCurrency)
{
    if (mBaseCurrency != baseCurrency) {
        setModified("baseCurrency");
        mBaseCurrency = baseCurrency;
        for (int i = 0; i < mListCurrency->size(); i++)
            if (mBaseCurrency == mListCurrency->at(i)->getCode()) {
                mBaseCurrencyPrefix = mListCurrency->at(i)->suffix();
                mBaseCurrencySuffix = mListCurrency->at(i)->prefix();
                break;
            }
    }
}

void DataMain::setHead(const QString &head)
{
    if (mHead != head) {
        setModified("head");
        mHead = head;
    }
}

void DataMain::setPostHead(const QString &postHead)
{
    if (mPostHead != postHead) {
        setModified("postHead");
        mPostHead = postHead;
    }
}

void DataMain::setBookkeeper(const QString &bookkeeper)
{
    if (mBookkeeper != bookkeeper) {
        setModified("bookkeeper");
        mBookkeeper = bookkeeper;
    }
}

void DataMain::setMailAddress(const QString &mailAddress)
{
    if (mMailAddress != mailAddress) {
        setModified("mailAddress");
        mMailAddress = mailAddress;
    }
}

void DataMain::setMainAddress(const QString &mainAddress)
{
    if (mMainAddress != mainAddress) {
        setModified("mainAddress");
        mMainAddress = mainAddress;
    }
}

void DataMain::setPhone(const QString &phone)
{
    if (mPhone != phone) {
        setModified("phone");
        mPhone = phone;
    }
}

void DataMain::setFax(const QString &fax)
{
    if (mFax != fax) {
        setModified("fax");
        mFax = fax;
    }
}

void DataMain::setNDS(const qreal &nds)
{
    if (mNDS != nds) {
        setModified("nds");
        mNDS = nds;
    }
}

void DataMain::setIsManualCurrencyRate(const bool &isManualCurrencyRate)
{
    if (mIsManualCurrencyRate != isManualCurrencyRate) {
        setModified("isManualCurrencyRate");
        mIsManualCurrencyRate = isManualCurrencyRate;
    }
}

void DataMain::setIsYAStore(const bool &isYAStore)
{
    if (mIsYAStore != isYAStore) {
        setModified("isYAStore");
        mIsYAStore = isYAStore;
    }
}

void DataMain::setIsYAPickup(const bool &isYAPickup)
{
    if (mIsYAPickup != isYAPickup) {
        setModified("isYAPickup");
        mIsYAPickup = isYAPickup;
    }
}

void DataMain::setIsYADelivery(const bool &isYADelivery)
{
    if (mIsYADelivery != isYADelivery) {
        setModified("isYADelivery");
        mIsYADelivery = isYADelivery;
    }
}

void DataMain::setCostYALocalDelivery(const double &costYALocalDelivery)
{
    if (mCostYALocalDelivery != costYALocalDelivery) {
        setModified("costYALocalDelivery");
        mCostYALocalDelivery = costYALocalDelivery;
    }
}

void DataMain::setShopFolder(const QString shopFolder)
{
    if (mShopFolder != shopFolder) {
        setModified("shopFolder");
        mShopFolder = shopFolder;
    }
}

QString DataMain::nameCompany() const
{
    return mNameCompany;
}

QString DataMain::shopName() const
{
    return mShopName;
}

QString DataMain::subName() const
{
    return mSubName;
}

QString DataMain::urlLogo() const
{
    return mURLLogo;
}

QString DataMain::domain() const
{
    return mDomain;
}

QString DataMain::emailSales() const
{
    return mEmailSales;
}

QString DataMain::emailSupport() const
{
    return mEmailSupport;
}

QString DataMain::baseCurrency() const
{
    if (mBaseCurrency.isEmpty())
        return "RUR";
    return mBaseCurrency;
}

const QString DataMain::baseCurrencyPrefix() const
{
    return mBaseCurrencyPrefix;
}

const QString DataMain::baseCurrencySuffix() const
{
    return mBaseCurrencySuffix;
}

QString DataMain::head() const
{
    return mHead;
}

QString DataMain::postHead() const
{
    return mPostHead;
}

QString DataMain::bookkeeper() const
{
    return mBookkeeper;
}

QString DataMain::mailAddress() const
{
    return mMailAddress;
}

QString DataMain::mainAddress() const
{
    return mMainAddress;
}

QString DataMain::phone() const
{
    return mPhone;
}

QString DataMain::fax() const
{
    return mFax;
}

qreal DataMain::nds() const
{
    return mNDS;
}

bool DataMain::isManualCurrencyRate() const
{
    return mIsManualCurrencyRate;
}

bool DataMain::isYAStore() const
{
    return mIsYAStore;
}

bool DataMain::isYAPickup() const
{
    return mIsYAPickup;
}

bool DataMain::isYADelivery() const
{
    return mIsYADelivery;
}

double DataMain::costYALocalDelivery() const
{
    return mCostYALocalDelivery;
}

QString DataMain::shopFolder() const
{
    return mShopFolder;
}

bool DataMain::isShowSESection() const
{
    return mIsShowSESction;
}

bool DataMain::isShowIncPrices() const
{
    return mIsShowIncPrices;
}

bool DataMain::isShowOptions() const
{
    return mIsShowOptions;
}

bool DataMain::isShowDocDelivery() const
{
    return mIsShowDocDelivery;
}

bool DataMain::isShowRequestCredit() const
{
    return mIsShowRequestCredit;
}

bool DataMain::isShowBlankShipment() const
{
    return mIsShowBlankShipment;
}

const DataCurrency *DataMain::baseDataCurrency() const
{
    for (int i = 0; i < mListCurrency->size(); i++)
        if (mListCurrency->at(i)->getCode() == mBaseCurrency)
            return mListCurrency->at(i);
    return 0;
}

ListCurrencies *DataMain::listCurrency() const
{
    return mListCurrency;
}

void DataMain::setFromJSONObject(const QJsonObject &jsonobject)
{    
    DataItem::setFromJSONObject(jsonobject);        

    if (!jsonobject["nameCompany"].isNull())
        mNameCompany = jsonobject["nameCompany"].toString();
    if (!jsonobject["shopName"].isNull())
        mShopName = jsonobject["shopName"].toString();
    if (!jsonobject["subName"].isNull())
        mSubName =  jsonobject["subName"].toString();
    if (!jsonobject["urlLogo"].isNull())
        mURLLogo = jsonobject["urlLogo"].toString();
    if (!jsonobject["domain"].isNull())
        mDomain = jsonobject["domain"].toString();
    if (!jsonobject["emailSales"].isNull())
        mEmailSales = jsonobject["emailSales"].toString();
    if (!jsonobject["emailSupport"].isNull())
        mEmailSupport = jsonobject["emailSupport"].toString();
    if (!jsonobject["baseCurrency"].isNull())
        mBaseCurrency = jsonobject["baseCurrency"].toString();
    if (!jsonobject["baseCurrencyPrefix"].isNull())
        mBaseCurrencyPrefix = jsonobject["baseCurrencyPrefix"].toString();
    if (!jsonobject["baseCurrencySuffix"].isNull())
        mBaseCurrencySuffix = jsonobject["baseCurrencySuffix"].toString();
    if (!jsonobject["head"].isNull())
        mHead = jsonobject["head"].toString();
    if (!jsonobject["postHead"].isNull())
        mPostHead = jsonobject["postHead"].toString();
    if (!jsonobject["bookkeeper"].isNull())
        mBookkeeper = jsonobject["bookkeeper"].toString();
    if (!jsonobject["mailAddress"].isNull())
        mMailAddress = jsonobject["mailAddress"].toString();
    if (!jsonobject["mainAddress"].isNull())
        mMainAddress = jsonobject["mainAddress"].toString();
    if (!jsonobject["phone"].isNull())
        mPhone = jsonobject["phone"].toString();
    if (!jsonobject["fax"].isNull())
        mFax = jsonobject["fax"].toString();
    if (!jsonobject["nds"].isNull())
        mNDS = jsonobject["nds"].toDouble();
    if (!jsonobject["nds"].isNull())
        mNDS = jsonobject["nds"].toDouble();
    if (!jsonobject["isManualCurrencyRate"].isNull())
        mIsManualCurrencyRate = jsonobject["isManualCurrencyRate"].toBool();
    if (!jsonobject["isYAStore"].isNull())
        mIsYAStore = jsonobject["isYAStore"].toBool();
    if (!jsonobject["isYAPickup"].isNull())
        mIsYAPickup = jsonobject["isYAPickup"].toBool();
    if (!jsonobject["isYADelivery"].isNull())
        mIsYADelivery = jsonobject["isYADelivery"].toBool();
    if (!jsonobject["costYALocalDelivery"].isNull())
        mCostYALocalDelivery = jsonobject["costYALocalDelivery"].toDouble();
    if (!jsonobject["shopFolder"].isNull())
        mShopFolder = jsonobject["shopFolder"].toString();
    if (!jsonobject["isShowSESections"].isNull())
        mIsShowSESction = jsonobject["isShowSESections"].toBool();
    if (!jsonobject["isShowIncPrices"].isNull())
        mIsShowIncPrices = jsonobject["isShowIncPrices"].toBool();
    if (!jsonobject["isShowOptions"].isNull())
        mIsShowOptions = jsonobject["isShowOptions"].toBool();
    if (!jsonobject["isShowDocDelivery"].isNull())
        mIsShowDocDelivery = jsonobject["isShowDocDelivery"].toBool();
    if (!jsonobject["isShowRequestCredit"].isNull())
        mIsShowRequestCredit = jsonobject["isShowRequestCredit"].toBool();
    if (!jsonobject["isShowBlankShipment"].isNull())
        mIsShowBlankShipment = jsonobject["isShowBlankShipment"].toBool();

    mListCurrency->deleteItems();
    if (!jsonobject["listCurrency"].isNull()) {
        QJsonArray items;
        items = jsonobject["listCurrency"].toArray();
        foreach (const QJsonValue& value, items)
            mListCurrency->append(new DataCurrency(value.toObject()));
    }
}

QJsonObject DataMain::getAsJsonObject() const
{
    QJsonObject obj = DataItem::getAsJsonObject();

    obj.insert("nameCompany", QJsonValue(mNameCompany));
    obj.insert("shopName", QJsonValue(mShopName));
    obj.insert("subName", QJsonValue(mSubName));
    obj.insert("urlLogo", QJsonValue(mURLLogo));
    obj.insert("domain", QJsonValue(mDomain));
    obj.insert("emailSales", QJsonValue(mEmailSales));
    obj.insert("emailSupport", QJsonValue(mEmailSupport));
    obj.insert("baseCurrency", QJsonValue(mBaseCurrency));    
    obj.insert("head", QJsonValue(mHead));
    obj.insert("poastHead", QJsonValue(mPostHead));
    obj.insert("bookkeeper", QJsonValue(mBookkeeper));
    obj.insert("mailAddress", QJsonValue(mMailAddress));
    obj.insert("mainAddress", QJsonValue(mMainAddress));
    obj.insert("phone", QJsonValue(mPhone));
    obj.insert("fax", QJsonValue(mFax));
    obj.insert("nds", QJsonValue(mNDS));
    obj.insert("isManualCurrencyRate", QJsonValue(mIsManualCurrencyRate));
    obj.insert("isYAStore", QJsonValue(mIsYAStore));
    obj.insert("isYAPickup", QJsonValue(mIsYAPickup));
    obj.insert("isYADelivery", QJsonValue(mIsYADelivery));
    obj.insert("costYALocalDelivery", QJsonValue(mCostYALocalDelivery));
    obj.insert("shopFolder", QJsonValue(mShopFolder));

    return obj;
}

