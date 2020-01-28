#ifndef DATAMAIN_H
#define DATAMAIN_H

#include "datacurrency.h"

class DataMain : public DataItem
{
public:
    DataMain();
    ~DataMain();

    void setNameCompany(const QString &nameCompany);
    void setShopName(const QString &shopName);
    void setSubName(const QString &subName);
    void setURLLogo(const QString &urlLogo);
    void setDomain(const QString &domain);
    void setEmailSales(const QString &emailSales);
    void setEmailSupport(const QString &emailSupport);
    void setBaseCurrency(const QString &baseCurrency);    
    void setHead(const QString &head);
    void setPostHead(const QString &postHead);
    void setBookkeeper(const QString &bookkeeper);
    void setMailAddress(const QString &mailAddress);
    void setMainAddress(const QString &mainAddress);
    void setPhone(const QString &phone);
    void setFax(const QString &fax);
    void setNDS(const qreal &nds);
    void setIsManualCurrencyRate(const bool &isManualCurrencyRate);    
    void setIsYAStore(const bool &isYAStore);
    void setIsYAPickup(const bool &isYAPickup);
    void setIsYADelivery(const bool &isYADelivery);
    void setCostYALocalDelivery(const double &costYALocalDelivery);
    void setShopFolder(const QString shopFolder);

    QString nameCompany() const;
    QString shopName() const;
    QString subName() const;
    QString urlLogo() const;
    QString domain() const;
    QString emailSales() const;
    QString emailSupport() const;
    QString baseCurrency() const;    
    const QString baseCurrencyPrefix() const;
    const QString baseCurrencySuffix() const;
    QString head() const;
    QString postHead() const;
    QString bookkeeper() const;
    QString mailAddress() const;
    QString mainAddress() const;
    QString phone() const;
    QString fax() const;
    qreal nds() const;
    bool isManualCurrencyRate() const;
    const DataCurrency *baseDataCurrency() const;    
    bool isYAStore() const;
    bool isYAPickup() const;
    bool isYADelivery() const;
    double costYALocalDelivery() const;
    QString shopFolder() const;
    bool isShowSESection() const;
    bool isShowIncPrices() const;
    bool isShowOptions() const;
    bool isShowDocDelivery() const;
    bool isShowRequestCredit() const;
    bool isShowBlankShipment() const;

    ListCurrencies *listCurrency() const;

    void setFromJSONObject(const QJsonObject &jsonobject);
    QJsonObject getAsJsonObject() const;

private:
    QString mNameCompany;
    QString mShopName;
    QString mSubName;
    QString mURLLogo;
    QString mDomain;
    QString mEmailSales;
    QString mEmailSupport;
    QString mBaseCurrency;    
    QString mBaseCurrencyPrefix;
    QString mBaseCurrencySuffix;
    QString mHead;
    QString mPostHead;
    QString mBookkeeper;
    QString mMailAddress;
    QString mMainAddress;
    QString mPhone;
    QString mFax;
    QString mShopFolder;
    bool mIsShowSESction;
    bool mIsShowIncPrices;
    bool mIsShowOptions;
    bool mIsShowDocDelivery;
    bool mIsShowRequestCredit;
    bool mIsShowBlankShipment;
    qreal mNDS;
    bool mIsManualCurrencyRate;
    bool mIsYAStore;
    bool mIsYAPickup;
    bool mIsYADelivery;
    double mCostYALocalDelivery;    

    ListCurrencies *mListCurrency;

};

#endif // DATAMAIN_H
