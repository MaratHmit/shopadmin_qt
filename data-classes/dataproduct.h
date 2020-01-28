#ifndef DATAPRODUCT_H
#define DATAPRODUCT_H

#include "datafeature.h"
#include "datamodificationgroup.h"
#include "datadiscount.h"
#include "dataimages.h"
#include "dataoption.h"

class DataProduct;

    typedef SEVector<DataProduct *> ListProducts;

class DataProduct : public DataImages
{
    Q_OBJECT
    Q_PROPERTY(QString article READ getArticle)
    Q_PROPERTY(QString price READ priceTitle)
    Q_PROPERTY(QPixmap isHitIco READ isHitIco)
    Q_PROPERTY(QPixmap isNewIco READ isNewIco)
    Q_PROPERTY(QPixmap isActionIco READ isActionIco)
    Q_PROPERTY(QPixmap isDiscountIco READ isDiscountIco)    
    Q_PROPERTY(QPixmap isYAMarket READ isYAMarketIco)
    Q_PROPERTY(QString measurement READ measurement)
    Q_PROPERTY(QString count READ countTitle)
    Q_PROPERTY(QString cost READ costTitle)
    Q_PROPERTY(QString discount READ discountTitle)
    Q_PROPERTY(int countModifications READ getCountModifications)
    Q_PROPERTY(QString brand READ brandTitle)
    Q_PROPERTY(QString weight READ weightTitle)

public:
    DataProduct();
    DataProduct(const QJsonObject &jsonobject);
    DataProduct(const DataProduct *d);
    ~DataProduct();

    void setFromJSONObject(const QJsonObject &jsonobject);
    QJsonObject getAsJsonObject() const;

    QString getArticle() const;        
    qreal price() const;
    qreal pricePurchase() const;
    qreal priceWholesale() const;
    qreal priceMiniWholesale() const;
    qreal discountPercent() const;    
    qreal bonus() const;
    qreal rate() const;
    qreal tax() const;
    qreal cost() const;
    QString measurement() const;
    qreal count() const;
    bool isInfinitely() const;
    QString getCurrency() const;
    qreal stepCount() const;
    qreal minCount() const;
    QString precense() const;
    bool isNew() const;
    bool isHit() const;
    bool isAction() const;
    bool isSpec() const;
    QString idManufacturer() const;
    QString manufacturer() const;
    QDate dateManufactured() const;
    DataItem *brand() const;
    double weight() const;
    double volume() const;
    bool isDiscount() const;
    bool isShowFeatures() const;
    qreal maxDiscount() const;
    QString licenses() const;
    int getCountModifications() const;
    int getCountOptions() const;
    QString idsModifications() const;
    bool isYAMarket() const;
    bool isYAMarketAvailable() const;
    QString idModificationGroupDef() const;
    void addOption(DataOption *option);

    ListFeatures *listSpecifications() const;
    ListModificationsGroups *listModificationsGroups() const;
    ListProducts *listSimilarProducts() const;
    ListProducts *listAccompProducts() const;
    ListDataItems *listComments() const;
    ListDataItems *listCrossGroups() const;
    ListDiscounts *listDiscounts() const;
    ListDataItems *listFiles() const;    
    ListDataItems *listLabels() const;
    ListOptions *listOptions() const;

    void convertInBaseCurrency();

    const QPixmap isHitIco() const;
    const QPixmap isNewIco() const;
    const QPixmap isActionIco() const;
    const QPixmap isDiscountIco() const;
    const QPixmap isYAMarketIco() const;
    const QString priceTitle() const;
    const QString countTitle() const;
    const QString costTitle() const;
    const QString discountTitle() const;
    const QString brandTitle() const;
    const QString weightTitle() const;

    void clearData();
    void copy(const DataProduct *d);    
    void copy(const DataImages *d);

    bool isModified() const;
    void setUnModified();            

    const QColor getColorByPropertyName(const QString &propertyName) const override;
    virtual void fillFromAllSimpleValues(const DataItem *item);

public slots:
    void setArticle(const QString &getArticle);
    void setPrice(const qreal &price);
    void setPricePurchase(const qreal &pricePurchase);
    void setPriceWholesale(const qreal &price);
    void setPriceMiniWholesale(const qreal &price);
    void setDiscountPercent(const qreal &discount);
    void setTax(const qreal &tax);
    void setBonus(const qreal &bonus);
    void setRate(const qreal &rate);
    void setMeasurement(const QString &measurement);
    void setCount(const qreal &count);
    void setMinCount(const qreal &minCount);
    void setIsInfinitely(const bool &isInfinitely);
    void setCurrency(const QString &getCurrency);
    void setStepCount(const qreal &stepCount);
    void setPrecense(const QString &precense);
    void setIsNew(const bool &isNew);
    void setIsHit(const bool &isHit);
    void setIsSpec(const bool &isSpec);
    void setIsAction(const bool &isAction);
    void setIdManufacturer(const QString &idManufacturer);
    void setManufacturer(const QString &manufacturer);
    void setDateManufactured(const QDate &dateManufactured);
    void setWeight(const double &weight);
    void setVolume(const double &volume);
    void setIsDiscount(const bool &isDiscount);
    void setMaxDiscount(const double &maxDiscount);
    void setLicense(const QString &license);
    void setIdsModifications(const QString &idsModifications);
    void setIsYAMarket(const bool &isYAMarket);
    void setIsYAMarketAvailable(const bool &isYAMarketAvailable);
    void setIsShowFeatures(const bool &isShowFeatures);
    void setIdModificationGroupDef(const QString &idModificationGroupDef);
    void setCountModifications(const int &getCountModifications);

protected:
    void initialization();

private:    
    QString mArticle;        
    qreal mPrice;
    qreal mPricePurchase;
    qreal mPriceWholesale;
    qreal mPriceMiniWholesale;
    qreal mDiscountPercent;
    qreal mBonus;
    qreal mRate;
    qreal mTax;
    QString mMeasurement;
    qreal mCount;
    qreal mMinCount;
    QString mCurrency;
    bool mIsInfinitely;
    qreal mStepCount;
    QString mPrecense;
    bool mIsNew;
    bool mIsHit;
    bool mIsSpec;
    bool mIsAction;
    QString mIdManufacturer;
    QString mManufacturer;
    QDate mDateManufactured;
    QString mIdsModifications;
    DataItem *mBrand;
    double mWeight;
    double mVolume;
    bool mIsDiscount;
    double mMaxDiscount;
    QString mLicenses;
    int mCountModifications;    
    int mCountOptions;
    bool mIsYAMarket;
    bool mIsYAMarketAvailable;
    bool mIsCrossGroup;
    bool mIsShowFeatures;
    QPixmap mIcoHit;
    QPixmap mIcoNew;
    QPixmap mIcoAction;
    QPixmap mIcoDiscount;
    QPixmap mIcoIsYAMraket;    
    ListFeatures *mListSpecifications;
    ListModificationsGroups *mListModificationsGroups;
    ListProducts *mListSimilarProducts;
    ListProducts *mListAccompProducts;
    ListDataItems *mListComments;
    ListDataItems *mListCrossGroups;
    ListDiscounts *mListDiscounts;
    ListDataItems *mListFiles;
    ListDataItems *mListLabels;
    ListOptions *mListOptions;
    QString mIdModificationGroupDef;
};


#endif // DATAPRODUCT_H
