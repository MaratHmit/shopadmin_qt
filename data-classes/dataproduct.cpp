#include "dataproduct.h"
#include "seconfig.h"

#include <QMessageBox>

DataProduct::DataProduct()
{
    initialization();
}

DataProduct::DataProduct(const QJsonObject &jsonobject)
{
    initialization();
    setFromJSONObject(jsonobject);
}

DataProduct::DataProduct(const DataProduct *d)
{
    initialization();
    copy(d);
}

DataProduct::~DataProduct()
{
    delete mBrand;
    delete mListSpecifications;
    delete mListModificationsGroups;
    delete mListSimilarProducts;
    delete mListAccompProducts;
    delete mListComments;
    delete mListCrossGroups;
    delete mListDiscounts;
    delete mListFiles;
    delete mListLabels;
    delete mListOptions;
}


void DataProduct::initialization()
{
    DataImages::initialization();

    mBrand = new DataItem();    
    mListSpecifications = new ListFeatures();
    mListModificationsGroups = new ListModificationsGroups();
    mListAccompProducts = new ListProducts();
    mListSimilarProducts = new ListProducts();
    mListComments = new ListDataItems();
    mListCrossGroups = new ListDataItems();    
    mListDiscounts = new ListDiscounts(); 
    mListFiles = new ListDataItems();
    mListLabels = new ListDataItems();
    mListOptions = new ListOptions();
    mPrice = 0;
    mRate = 1;
    mPriceWholesale = 0;
    mPriceWholesale = 0;
    mPriceMiniWholesale = 0;
    mDiscountPercent = 0;
    mBonus = 0;
    mTax = 0;
    mCount = -1;
    mMinCount = 1;
    mIsInfinitely = true;
    mStepCount = 0;
    mIsNew = 0;
    mIsHit = 0;
    mIsSpec = 0;
    mIsAction = 0;
    mWeight = 0;
    mVolume = 0;
    mIsDiscount = 0;
    mMaxDiscount = 0;    
    mCountModifications = 0;
    mCountOptions = 0;
    mIsYAMarket = 1;
    mIsYAMarketAvailable = 1;
    mIsCrossGroup = 0;
    mIsShowFeatures = 1;

    mIcoHit.load(":/16/icons/16x16/hit.png");
    mIcoNew.load(":/16/icons/16x16/new.png");
    mIcoAction.load(":/16/icons/16x16/action.png");
    mIcoDiscount.load(":/16/icons/16x16/discount.png");
    mIcoIsYAMraket.load(":/16/icons/16x16/check_black.png");
}

void DataProduct::setArticle(const QString &article)
{
    if (mArticle != article) {
        mArticle = article;
        setModified("article");
    }
}

void DataProduct::setPrice(const qreal &price)
{
    if (mPrice != price) {
        mPrice = price;
        setModified("price");
    }
}

void DataProduct::setPricePurchase(const qreal &pricePurchase)
{
    if (mPricePurchase != pricePurchase) {
        mPricePurchase = pricePurchase;
        setModified("pricePurchase");
    }
}

void DataProduct::setPriceWholesale(const qreal &optPrice)
{
    if (mPriceWholesale != optPrice) {
        mPriceWholesale = optPrice;
        setModified("priceWholesale");
    }
}

void DataProduct::setPriceMiniWholesale(const qreal &corpPrice)
{    
    if (mPriceMiniWholesale != corpPrice) {
        mPriceMiniWholesale = corpPrice;
        setModified("priceMiniWholesale");
    }
}

void DataProduct::setDiscountPercent(const qreal &discountPercent)
{   
    if (mDiscountPercent != discountPercent &&
            (discountPercent >= 0) && (discountPercent <= 100)) {
        mDiscountPercent = discountPercent;
        setModified("discountPercent");
    }
}

void DataProduct::setTax(const qreal &tax)
{  
    if (mTax != tax) {
        mTax = tax;
        setModified("tax");
    }
}

void DataProduct::setBonus(const qreal &bonus)
{   
    if (mBonus != bonus) {
        mBonus = bonus;
        setModified("bonus");
    }
}

void DataProduct::setRate(const qreal &rate)
{
    if (mRate != rate) {
        mRate = rate;
        setModified("rate");
    }
}

void DataProduct::setMeasurement(const QString &measurement)
{
    if (mMeasurement != measurement) {
        mMeasurement = measurement;
        setModified("measurement");
    }
}

void DataProduct::setCount(const qreal &count)
{    
    if (mCount != count) {
        mCount = count;             
        setModified("count");
    }
}

void DataProduct::setMinCount(const qreal &minCount)
{
    if (mMinCount != minCount) {
        mMinCount = minCount;
        setModified("minCount");
    }
}

void DataProduct::setIsInfinitely(const bool &isInfinitely)
{
    if (mIsInfinitely != isInfinitely) {
        mIsInfinitely = isInfinitely;        
        setModified("isInfinitely");
    }
}

void DataProduct::setCurrency(const QString &currency)
{
    if (mCurrency != currency) {
        mCurrency = currency;
        setModified("currency");
    }
}

void DataProduct::setStepCount(const qreal &stepCount)
{   
    if (mStepCount != stepCount) {
        mStepCount = stepCount;
        setModified("stepCount");
    }
}

void DataProduct::setPrecense(const QString &precense)
{
    if (mPrecense != precense) {
        mPrecense = precense;        
        setModified("precense");
    }
}

void DataProduct::setIsNew(const bool &isNew)
{  
    if (mIsNew != isNew) {
        mIsNew = isNew;
        setModified("isNew");
    }
}

void DataProduct::setIsHit(const bool &isHit)
{    
    if (mIsHit != isHit) {
        mIsHit = isHit;
        setModified("isHit");
    }
}

void DataProduct::setIsSpec(const bool &isSpec)
{
    if (mIsSpec != isSpec) {
        mIsSpec = isSpec;
        setModified("isSpec");
    }
}

void DataProduct::setIsAction(const bool &isAction)
{   
    if (mIsAction != isAction) {
        mIsAction = isAction;
        setModified("isAction");
    }
}

void DataProduct::setIdManufacturer(const QString &idManufacturer)
{
    if (mIdManufacturer != idManufacturer) {
        mIdManufacturer = idManufacturer;
        setModified("idManufacturer");
    }
}

void DataProduct::setManufacturer(const QString &manufacturer)
{
    if (mManufacturer != manufacturer) {
        mManufacturer = manufacturer;
        setModified("manufacturer");
    }
}

void DataProduct::setDateManufactured(const QDate &dateManufactured)
{
    if (mDateManufactured != dateManufactured) {
        mDateManufactured = dateManufactured;
        setModified("dateManufactured");
    }
}

void DataProduct::setWeight(const double &weight)
{    
    if (mWeight != weight) {
        mWeight = weight;
        setModified("weight");
    }
}

void DataProduct::setVolume(const double &volume)
{
    if (mVolume != volume) {
        mVolume = volume;
        setModified("volume");
    }
}

void DataProduct::setIsDiscount(const bool &isDiscount)
{  
    if (mIsDiscount != isDiscount) {
        mIsDiscount = isDiscount;
        setModified("isDiscount");
    }
}

void DataProduct::setMaxDiscount(const double &maxDiscount)
{    
    if (mMaxDiscount != maxDiscount) {
        mMaxDiscount = maxDiscount;
        setModified("maxDiscount");
    }
}

void DataProduct::setLicense(const QString &license)
{
    if (mLicenses != license) {
        mLicenses = license;
        setModified("license");
    }
}

void DataProduct::setIdsModifications(const QString &idsModifications)
{
    mIdsModifications = idsModifications;
}

void DataProduct::setIsYAMarket(const bool &isYAMarket)
{
    if (mIsYAMarket != isYAMarket) {
        mIsYAMarket = isYAMarket;
        setModified("isYAMarket");
    }
}

void DataProduct::setIsYAMarketAvailable(const bool &isYAMarketAvailable)
{
    if (mIsYAMarketAvailable != isYAMarketAvailable) {
        mIsYAMarketAvailable = isYAMarketAvailable;
        setModified("isYAMarketAvailable");
    }
}

void DataProduct::setIsShowFeatures(const bool &isShowFeatures)
{
    if (mIsShowFeatures != isShowFeatures) {
        mIsShowFeatures = isShowFeatures;
        setModified("isShowFeatures");
    }
}

void DataProduct::setIdModificationGroupDef(const QString &idModificationGroupDef)
{
    mIdModificationGroupDef = idModificationGroupDef;
}

void DataProduct::setCountModifications(const int &countModifications)
{
    mCountModifications = countModifications;
}

void DataProduct::setFromJSONObject(const QJsonObject &jsonobject)
{
    DataImages::setFromJSONObject(jsonobject);

    if (!jsonobject["article"].isNull())
        mArticle = jsonobject["article"].toString();
    if (!jsonobject["price"].isNull())
          mPrice = jsonobject["price"].toDouble();
    if (!jsonobject["pricePurchase"].isNull())
          mPricePurchase = jsonobject["pricePurchase"].toDouble();
    if (!jsonobject["priceWholesale"].isNull())
          mPriceWholesale = jsonobject["priceWholesale"].toDouble();
    if (!jsonobject["priceMiniWholesale"].isNull())
          mPriceMiniWholesale = jsonobject["priceMiniWholesale"].toDouble();
    if (!jsonobject["tax"].isNull())
          mTax = jsonobject["tax"].toDouble();
    if (!jsonobject["bonus"].isNull())
          mBonus = jsonobject["bonus"].toDouble();
    if (!jsonobject["rate"].isNull())
          mRate = jsonobject["rate"].toDouble();
    if (!jsonobject["discountPercent"].isNull())
          mDiscountPercent = jsonobject["discountPercent"].toDouble();
    if (!jsonobject["count"].isNull())
          mCount = jsonobject["count"].toDouble();
    if (!jsonobject["minCount"].isNull())
          mMinCount = jsonobject["minCount"].toDouble();
    if (!jsonobject["stepCount"].isNull())
          mStepCount = jsonobject["stepCount"].toDouble();
    if (!jsonobject["measurement"].isNull())
        mMeasurement = jsonobject["measurement"].toString();
    if (!jsonobject["isInfinitely"].isNull())
       mIsInfinitely = jsonobject["isInfinitely"].toBool();
    if (!jsonobject["currency"].isNull())
       mCurrency = jsonobject["currency"].toString();
    if (!jsonobject["presence"].isNull())
        mPrecense = jsonobject["presence"].toString();
    if (!jsonobject["isNew"].isNull())
        mIsNew = jsonobject["isNew"].toBool();
    if (!jsonobject["isHit"].isNull())
        mIsHit = jsonobject["isHit"].toBool();
    if (!jsonobject["isAction"].isNull())
        mIsAction = jsonobject["isAction"].toBool();
    if (!jsonobject["isSpecial"].isNull())
        mIsSpec = jsonobject["isSpecial"].toBool();
    if (!jsonobject["idManufacturer"].isNull())
        mIdManufacturer = jsonobject["idManufacturer"].toString();
    if (!jsonobject["manufacturer"].isNull())
        mManufacturer = jsonobject["manufacturer"].toString();
    if (!jsonobject["dateManufactured"].isNull())
        mDateManufactured = QDate::fromString(
                    jsonobject["dateManufactured"].toString(), "yyyy-MM-dd");
    mBrand->setFromJSONObject(jsonobject["brand"].toObject());
    if (!jsonobject["weight"].isNull())
        mWeight = jsonobject["weight"].toDouble();
    if (!jsonobject["volume"].isNull())
        mVolume = jsonobject["volume"].toDouble();
    if (!jsonobject["isDiscount"].isNull())
        mIsDiscount = jsonobject["isDiscount"].toBool();
    if (!jsonobject["maxDiscount"].isNull())
        mMaxDiscount = jsonobject["maxDiscount"].toDouble();
    if (!jsonobject["license"].isNull())
        mLicenses = jsonobject["license"].toString();
    if (!jsonobject["countModifications"].isNull())
        mCountModifications = jsonobject["countModifications"].toInt();    
    if (!jsonobject["countOptions"].isNull())
        mCountOptions = jsonobject["countOptions"].toInt();
    if (!jsonobject["isYAMarket"].isNull())
        mIsYAMarket = jsonobject["isYAMarket"].toBool();
    if (!jsonobject["isYAMarketAvailable"].isNull())
        mIsYAMarketAvailable = jsonobject["isYAMarketAvailable"].toBool();
    if (!jsonobject["isCrossGroup"].isNull())
        mIsCrossGroup = jsonobject["isCrossGroup"].toBool();
    if (!jsonobject["idModificationGroupDef"].isNull())
        mIdModificationGroupDef = jsonobject["idModificationGroupDef"].toString();
    if (!jsonobject["isShowFeatures"].isNull())
        mIsShowFeatures = jsonobject["isShowFeatures"].toBool();

    mListSpecifications->deleteItems();
    QJsonArray items = jsonobject["specifications"].toArray();
    foreach (const QJsonValue& value, items)
        mListSpecifications->append(new DataFeature(value.toObject()));

    mListModificationsGroups->deleteItems();
    items = jsonobject["modifications"].toArray();
    foreach (const QJsonValue& value, items)
        mListModificationsGroups->append(new DataModificationGroup(value.toObject()));

    mListSimilarProducts->deleteItems();
    items = jsonobject["similarProducts"].toArray();
    foreach (const QJsonValue& value, items)
        mListSimilarProducts->append(new DataProduct(value.toObject()));

    mListAccompProducts->deleteItems();
    items = jsonobject["accompanyingProducts"].toArray();
    foreach (const QJsonValue& value, items)
        mListAccompProducts->append(new DataProduct(value.toObject()));

    mListComments->deleteItems();
    items = jsonobject["comments"].toArray();
    foreach (const QJsonValue& value, items)
        mListComments->append(new DataItem(value.toObject()));

    if (jsonobject["crossGroups"].isArray()) {
        mListCrossGroups->deleteItems();
        items = jsonobject["crossGroups"].toArray();
        foreach (const QJsonValue& value, items)
            mListCrossGroups->append(new DataItem(value.toObject()));
    }

    mListDiscounts->deleteItems();
    items = jsonobject["discounts"].toArray();
    foreach (const QJsonValue& value, items)
        mListDiscounts->append(new DataDiscount(value.toObject()));

    mListFiles->deleteItems();
    items = jsonobject["files"].toArray();
    foreach (const QJsonValue& value, items)
        mListFiles->append(new DataItem(value.toObject()));

    mListLabels->deleteItems();
    items = jsonobject["labels"].toArray();
    foreach (const QJsonValue& value, items)
        mListLabels->append(new DataItem(value.toObject()));

    mListOptions->deleteItems();
    items = jsonobject["options"].toArray();
    foreach (const QJsonValue& value, items)
        mListOptions->append(new DataOption(value.toObject()));

    setUnModified();    
}

QJsonObject DataProduct::getAsJsonObject() const
{
    QJsonObject obj = DataImages::getAsJsonObject();

    if (isSetAllFields || modifiedFields.contains("article"))
        obj.insert("article", QJsonValue(mArticle));
    if (isSetAllFields || modifiedFields.contains("currency"))
        obj.insert("currency", QJsonValue(mCurrency));
    if (isSetAllFields || modifiedFields.contains("price"))
        obj.insert("price", QJsonValue(mPrice));
    if (isSetAllFields || modifiedFields.contains("pricePurchase"))
        obj.insert("pricePurchase", QJsonValue(mPricePurchase));
    if (isSetAllFields || modifiedFields.contains("priceMiniWholesale"))
        obj.insert("priceMiniWholesale", QJsonValue(mPriceMiniWholesale));
    if (isSetAllFields || modifiedFields.contains("priceWholesale"))
        obj.insert("priceWholesale", QJsonValue(mPriceWholesale));
    if (isSetAllFields || modifiedFields.contains("bonus"))
        obj.insert("bonus", QJsonValue(mBonus));
    if (isSetAllFields || modifiedFields.contains("rate"))
        obj.insert("rate", QJsonValue(mRate));
    if (isSetAllFields || modifiedFields.contains("tax"))
        obj.insert("tax", QJsonValue(mTax));
    if (isSetAllFields || modifiedFields.contains("discountPercent"))
        obj.insert("discount", QJsonValue(mDiscountPercent));
    if (mIsInfinitely  && (isSetAllFields || modifiedFields.contains("isInfinitely")))
        obj.insert("isInfinitely", QJsonValue(mIsInfinitely));
    if (!mIsInfinitely &&
            (isSetAllFields || modifiedFields.contains("count") || modifiedFields.contains("isInfinitely"))) {
        obj.insert("count", QJsonValue(mCount));
        obj.insert("isInfinitely", QJsonValue(false));
    }
    if (isSetAllFields || modifiedFields.contains("stepCount"))
        obj.insert("stepCount", QJsonValue(mStepCount));
    if (isSetAllFields || modifiedFields.contains("minCount"))
        obj.insert("minCount", QJsonValue(mMinCount));
    if (isSetAllFields || modifiedFields.contains("precense"))
        obj.insert("precense", QJsonValue(mPrecense));
    if (isSetAllFields || modifiedFields.contains("measurement"))
        obj.insert("measurement", QJsonValue(mMeasurement));
    if (isSetAllFields || modifiedFields.contains("isNew"))
        obj.insert("isNew", QJsonValue(mIsNew));
    if (isSetAllFields || modifiedFields.contains("isHit"))
        obj.insert("isHit", QJsonValue(mIsHit));
    if (isSetAllFields || modifiedFields.contains("isActione"))
        obj.insert("isActione", QJsonValue(mIsAction));
    if (isSetAllFields || modifiedFields.contains("isSpec"))
        obj.insert("isSpecial", QJsonValue(mIsSpec));
    if (isSetAllFields || modifiedFields.contains("manufacturer"))
        obj.insert("manufacturer", QJsonValue(mManufacturer));
    if (isSetAllFields || (modifiedFields.contains("dateManufactured") && mDateManufactured > QDate()))
        obj.insert("dateManufactured", QJsonValue(mDateManufactured.toString("yyyy-MM-dd")));
    if (isSetAllFields || brand()->isModified() || modifiedFields.contains("brand"))
        obj.insert("brand", QJsonValue(brand()->getAsJsonObject()));
    if (isSetAllFields || modifiedFields.contains("weight"))
        obj.insert("weight", QJsonValue(mWeight));
    if (isSetAllFields || modifiedFields.contains("volume"))
        obj.insert("volume", QJsonValue(mVolume));
    if (isSetAllFields || modifiedFields.contains("isDiscount"))
        obj.insert("isDiscount", QJsonValue(mIsDiscount));
    if (isSetAllFields || modifiedFields.contains("maxDiscount"))
        obj.insert("maxDiscount", QJsonValue(mMaxDiscount));    
    if (isSetAllFields || modifiedFields.contains("isYAMarket"))
        obj.insert("isYAMarket", QJsonValue(mIsYAMarket));    
    if (isSetAllFields || modifiedFields.contains("isYAMarketAvailable"))
        obj.insert("isYAMarketAvailable", QJsonValue(mIsYAMarketAvailable));
    if (isSetAllFields || modifiedFields.contains("isShowFeatures"))
        obj.insert("isShowFeatures", QJsonValue(mIsShowFeatures));
    if (isSetAllFields || mListSpecifications->isModified()) {
        mListSpecifications->setIsSetAllFields(true);
        obj.insert("specifications", QJsonValue(mListSpecifications->getAsJsonArray()));
    }
    if (isSetAllFields || mListModificationsGroups->isModified()) {
        mListModificationsGroups->setIsSetAllFields(true);
        obj.insert("modifications", QJsonValue(mListModificationsGroups->getAsJsonArray()));
    }
    if (isSetAllFields || mListSimilarProducts->isModified()) {
        mListSimilarProducts->setIsSetAllFields(true);
        obj.insert("similarProducts", QJsonValue(mListSimilarProducts->getAsJsonArray()));
    }
    if (isSetAllFields || mListAccompProducts->isModified()) {
        mListAccompProducts->setIsSetAllFields(true);
        obj.insert("accompanyingProducts", QJsonValue(mListAccompProducts->getAsJsonArray()));
    }
    if (isSetAllFields || mListComments->isModified()) {
        mListComments->setIsSetAllFields(true);
        obj.insert("comments", QJsonValue(mListComments->getAsJsonArray()));
    }
    if (isSetAllFields || mListCrossGroups->isModified()) {
        mListCrossGroups->setIsSetAllFields(true);
        obj.insert("crossGroups", QJsonValue(mListCrossGroups->getAsJsonArray()));
    }
    if (isSetAllFields || mListDiscounts->isModified()) {
        mListDiscounts->setIsSetAllFields(true);
        obj.insert("discounts", QJsonValue(mListDiscounts->getAsJsonArray()));
    }
    if (isSetAllFields || mListFiles->isModified()) {
        mListFiles->setIsSetAllFields(true);
        obj.insert("files", QJsonValue(mListFiles->getAsJsonArray()));
    }
    if (isSetAllFields || mListLabels->isModified()) {
        mListLabels->setIsSetAllFields(true);
        obj.insert("labels", QJsonValue(mListLabels->getAsJsonArray()));
    }
    if (isSetAllFields || mListOptions->isModified()) {
        mListOptions->setIsSetAllFields(true);
        obj.insert("options", QJsonValue(mListOptions->getAsJsonArray()));
    }

    return obj;
}

QString DataProduct::getArticle() const
{
    return mArticle;
}

qreal DataProduct::price() const
{    
    return mPrice;
}

qreal DataProduct::pricePurchase() const
{
    return mPricePurchase;
}

qreal DataProduct::priceWholesale() const
{    
    return mPriceWholesale;
}

qreal DataProduct::priceMiniWholesale() const
{
    return mPriceMiniWholesale;
}

qreal DataProduct::discountPercent() const
{    
    return mDiscountPercent;
}

qreal DataProduct::bonus() const
{    
    return mBonus;
}

qreal DataProduct::rate() const
{
    return mRate;
}

qreal DataProduct::tax() const
{
    return mTax;
}

qreal DataProduct::cost() const
{
   return count() * price() * (1 - discountPercent() / 100);
}

QString DataProduct::measurement() const
{
    return mMeasurement;
}

qreal DataProduct::count() const
{
    return mCount;
}

bool DataProduct::isInfinitely() const
{
    return mIsInfinitely;
}

QString DataProduct::getCurrency() const
{
    return mCurrency;
}

qreal DataProduct::stepCount() const
{    
    return mStepCount;
}

qreal DataProduct::minCount() const
{
    return mMinCount;
}

QString DataProduct::precense() const
{
    return mPrecense;
}

bool DataProduct::isNew() const
{    
    return mIsNew;
}

bool DataProduct::isHit() const
{    
    return mIsHit;
}

bool DataProduct::isAction() const
{    
    return mIsAction;
}

bool DataProduct::isSpec() const
{
    return mIsSpec;
}

QString DataProduct::idManufacturer() const
{
    return mIdManufacturer;
}

QString DataProduct::manufacturer() const
{
    return mManufacturer;
}

QDate DataProduct::dateManufactured() const
{
    return mDateManufactured;
}

DataItem *DataProduct::brand() const
{
    return mBrand;
}

double DataProduct::weight() const
{
    return mWeight;
}

double DataProduct::volume() const
{    
    return mVolume;
}

bool DataProduct::isDiscount() const
{    
    return mIsDiscount;
}

bool DataProduct::isShowFeatures() const
{
    return mIsShowFeatures;
}

qreal DataProduct::maxDiscount() const
{    
    return mMaxDiscount;
}

QString DataProduct::licenses() const
{
    return mLicenses;
}

int DataProduct::getCountModifications() const
{
    return mCountModifications;
}

int DataProduct::getCountOptions() const
{
    return mCountOptions;
}

QString DataProduct::idsModifications() const
{
    return mIdsModifications;
}

bool DataProduct::isYAMarket() const
{
    return mIsYAMarket;
}

bool DataProduct::isYAMarketAvailable() const
{
    return mIsYAMarketAvailable;
}

QString DataProduct::idModificationGroupDef() const
{
    return mIdModificationGroupDef;
}

void DataProduct::addOption(DataOption *option)
{
    mListOptions->append(option);
}

ListFeatures *DataProduct::listSpecifications() const
{
    return mListSpecifications;
}

ListModificationsGroups *DataProduct::listModificationsGroups() const
{
    return mListModificationsGroups;
}

ListProducts *DataProduct::listSimilarProducts() const
{
    return mListSimilarProducts;
}

ListProducts *DataProduct::listAccompProducts() const
{
    return mListAccompProducts;
}

ListDataItems *DataProduct::listComments() const
{
    return mListComments;
}

ListDataItems *DataProduct::listCrossGroups() const
{
    return mListCrossGroups;
}

ListDiscounts *DataProduct::listDiscounts() const
{
    return mListDiscounts;
}

ListDataItems *DataProduct::listFiles() const
{
    return mListFiles;
}

ListDataItems *DataProduct::listLabels() const
{
    return mListLabels;
}

ListOptions *DataProduct::listOptions() const
{
    return mListOptions;
}

void DataProduct::convertInBaseCurrency()
{
    if (mCurrency != SEConfig::getMainInfo()->baseCurrency()) {
        setPrice(SEConfig::priceConvertToBaseCurrency(price(), mCurrency));
        mCurrency = SEConfig::getMainInfo()->baseCurrency();
    }
}

const QPixmap DataProduct::isHitIco() const
{    
    if (mIsHit)
        return mIcoHit;
    return QPixmap();
}

const QPixmap DataProduct::isNewIco() const
{    
    if (mIsNew)
        return mIcoNew;
    return QPixmap();
}

const QPixmap DataProduct::isActionIco() const
{    
    if (mIsAction)
        return mIcoAction;
    return QPixmap();
}

const QPixmap DataProduct::isDiscountIco() const
{    
    if (property("isDiscountAllowed").toBool())
        return mIcoDiscount;
    return QPixmap();
}

const QPixmap DataProduct::isYAMarketIco() const
{
    if (mIsYAMarket)
        return mIcoIsYAMraket;
    return QPixmap();
}

const QString DataProduct::priceTitle() const
{
    return SEConfig::priceCurrency(mPrice, mCurrency);
}

const QString DataProduct::countTitle() const
{
    QString s;
    if (mCount >= 0) {
        if (!mIsInfinitely)
            s = QString::number(mCount, 'f', 2);
        else {
            if (mPrecense.isEmpty())
                return "∞";
            else return mPrecense;
        }
    }
    if (mCount < 0) {
        if (mPrecense.isEmpty())
            return "∞";
        else return mPrecense;
    }
    if (!mMeasurement.isEmpty())
        return s + mMeasurement;
    else return s;
}

const QString DataProduct::costTitle() const
{
    return SEConfig::priceCurrency(cost(), mCurrency);
}

const QString DataProduct::discountTitle() const
{
    return QString::number(cost() * discountPercent() / 100, 'f', 2) +
            "(" + QString::number(discountPercent(), 'f', 2) + ")";
}

const QString DataProduct::brandTitle() const
{
    return mBrand->getName();
}

const QString DataProduct::weightTitle() const
{
    if (mWeight >= 1000)
        return QString::number(mWeight / 1000, 'f', 2) + "кг.";
    return QString::number(mWeight, 'f', 2) + "гр.";
}

void DataProduct::clearData()
{    
    DataImages::clearData();

    mArticle.clear();
    mPrice = 0;
    mPricePurchase = 0;
    mPriceWholesale = 0;
    mPriceMiniWholesale = 0;
    mDiscountPercent = 0;
    mBonus = 0;
    mTax = 0;
    mRate = 1;
    mMinCount = 1;
    mMeasurement = 0.0;
    mCount = 0;
    mCurrency.clear();
    mIsInfinitely = true;
    mStepCount = 1;
    mPrecense.clear();
    mManufacturer.clear();
    mIdManufacturer.clear();
    mDateManufactured = QDate();
    mBrand->clearData();
    mIsDiscount = 0;
    mIsNew = false;
    mIsHit = false;
    mIsAction = false;
    mIsSpec = false;
    mWeight = 0;
    mVolume = 0;
    mMaxDiscount = 0;
    mCountModifications = 0;
    mCountOptions = 0;
    mIsYAMarket = 1;
    mIsYAMarketAvailable = 1;
    mIsShowFeatures = 1;
    mListSpecifications->deleteItems();    
    mListModificationsGroups->deleteItems();        
    mListAccompProducts->deleteItems();        
    mListSimilarProducts->deleteItems();        
    mListCrossGroups->deleteItems();
    mListDiscounts->deleteItems();
    mListOptions->deleteItems();
    setUnModified();
}

void DataProduct::copy(const DataProduct *d)
{
    DataImages::copy(d);

    mArticle = d->mArticle;
    mPrice = d->mPrice;
    mPricePurchase = d->mPricePurchase;
    mPriceMiniWholesale = d->mPriceMiniWholesale;
    mPriceWholesale = d->mPriceWholesale;
    mDiscountPercent = d->mDiscountPercent;
    mBonus = d->mBonus;
    mTax = d->mTax;
    mRate = d->mRate;
    mMeasurement = d->mMeasurement;
    mCount = d->mCount;
    mMinCount = d->mMinCount;
    mCurrency = d->mCurrency;
    mIsInfinitely = d->mIsInfinitely;
    mStepCount = d->mStepCount;
    mIsNew = d->mIsNew;
    mIsHit = d->mIsHit;
    mIsSpec = d->mIsSpec;
    mIsAction = d->mIsAction;
    mIsYAMarket = d->mIsYAMarket;
    mIsYAMarketAvailable = d->mIsYAMarketAvailable;
    mIdManufacturer = d->mIdManufacturer;
    mManufacturer = d->mManufacturer;
    mDateManufactured = d->mDateManufactured;
    mBrand->copy(d->mBrand);
    mWeight = d->mWeight;
    mVolume = d->mVolume;
    mIsDiscount = d->mIsDiscount;
    mMaxDiscount = d->mMaxDiscount;
    mCountModifications = d->mCountModifications;    
    mCountOptions = d->mCountOptions;
    mIdsModifications = d->mIdsModifications;
    mIsShowFeatures = d->mIsShowFeatures;
    mIdModificationGroupDef = d->mIdModificationGroupDef;
    mListSpecifications->deleteItems();
    for (int i = 0; i < d->listSpecifications()->size(); i++)
        mListSpecifications->append(new DataFeature(d->listSpecifications()->at(i)));    
    mListModificationsGroups->deleteItems();
    for (int i = 0; i < d->listModificationsGroups()->size(); i++)
        mListModificationsGroups->append(
                    new DataModificationGroup(d->listModificationsGroups()->at(i)));    
    mListSimilarProducts->deleteItems();
    for (int i = 0; i < d->listSimilarProducts()->size(); i++)
        mListSimilarProducts->append(
                    new DataProduct(d->listSimilarProducts()->at(i)));    
    mListAccompProducts->deleteItems();
    for (int i = 0; i < d->listAccompProducts()->size(); i++)
        mListAccompProducts->append(new DataProduct(d->listAccompProducts()->at(i)));
    mListCrossGroups->deleteItems();
    for (int i = 0; i < d->listCrossGroups()->size(); i++)
        mListCrossGroups->append(new DataItem(d->listCrossGroups()->at(i)));
    for (int i = 0; i < d->listDiscounts()->size(); i++)
        mListDiscounts->append(new DataDiscount(d->listDiscounts()->at(i)));
    mListComments->deleteItems();
    for (int i = 0; i < d->listComments()->size(); i++)
        mListComments->append(new DataItem(d->listComments()->at(i)));
    mListFiles->deleteItems();
    for (int i = 0; i < d->listFiles()->size(); i++)
        mListFiles->append(new DataItem(d->listFiles()->at(i)));
    mListLabels->deleteItems();
    for (int i = 0; i < d->listLabels()->size(); i++)
        mListLabels->append(new DataItem(d->listLabels()->at(i)));
    mListOptions->deleteItems();
    for (int i = 0; i < d->listOptions()->size(); i++)
        mListOptions->append(new DataOption(d->listOptions()->at(i)));

    setUnModified();
}

void DataProduct::copy(const DataImages*d)
{
    copy(dynamic_cast <const DataProduct *> (d));
}

bool DataProduct::isModified() const
{
   return DataImages::isModified() || mListSpecifications->isModified() ||
           mListModificationsGroups->isModified() ||
           mListAccompProducts->isModified() || mListCrossGroups->isModified() ||
           mListSimilarProducts->isModified() || mBrand->isModified() ||
           mListDiscounts->isModified() || mListComments->isModified() ||
           mListFiles->isModified() || mListOptions->isModified() || mListLabels->isModified();
}

void DataProduct::setUnModified()
{
    DataImages::setUnModified();

    mBrand->setUnModified();
    mListSpecifications->setUnModified();
    mListModificationsGroups->setUnModified();    
    mListAccompProducts->setUnModified();
    mListSimilarProducts->setUnModified();
    mListComments->setUnModified();
    mListCrossGroups->setUnModified();
    mListDiscounts->setUnModified();
    mListFiles->setUnModified();
    mListOptions->setUnModified();
    mListLabels->setUnModified();
}

const QColor DataProduct::getColorByPropertyName(const QString &propertyName) const
{
    if (propertyName == "countModifications" && mCountModifications)
        return QColor("#66CDAA");

    if (mIsCrossGroup)
        return QColor("#FAEBD7");

    return QColor();
}

void DataProduct::fillFromAllSimpleValues(const DataItem *item)
{
    DataItem::fillFromAllSimpleValues(item);


}



