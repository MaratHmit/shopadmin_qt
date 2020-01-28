#ifndef DATACONTACT_H
#define DATACONTACT_H

#include <QDate>

#include "datacompanyrequisite.h"
#include "datapersonalaccount.h"
#include "dataimages.h"

class DataContact : public DataImages
{
    Q_OBJECT
    Q_PROPERTY(QString regDate READ regDateTitle)
    Q_PROPERTY(QString login READ getLogin)
    Q_PROPERTY(QString phone READ phone)
    Q_PROPERTY(QString email READ email)
    Q_PROPERTY(QString countOrders READ countOrdersTitle)
    Q_PROPERTY(QString priceTypeDisplay READ priceTypeDisplay)

public:
    DataContact();    
    DataContact(const DataContact *d);
    DataContact(const QJsonObject &jsonobject);
    ~DataContact();

    void copy(const DataContact *d);

    enum Gender {
        Unknown = 'N',
        Female = 'F',
        Men = 'M'};

    void setFromJSONObject(const QJsonObject &jsonobject);
    QJsonObject getAsJsonObject() const;
    void initialization();

    void setLogin(const QString &getLogin);
    void setPasswordHash(const QString &passwordHash);
    void setPassword(const QString &password);
    void setGender(const Gender gender);
    void setEmail(const QString &email);
    void setPhone(const QString &phone);
    void setPostIndex(const QString &postIndex);
    void setAddress(const QString &address);
    void setRegDate(const QDate &regDate);
    void setBirthDate(const QDate &birthDate);
    void setCountOrders(const int countOrders);
    void setDiscount(const qreal discount);
    void setDocSer(const QString &docSer);
    void setDocNum(const QString &docNum);
    void setDocRegistr(const QString &docRegistr);    
    void setSkype(const QString &skype);
    void setLoyalty(const int &loyalty);
    void setCompanyName(const QString &companyName);
    void setCompanyDirector(const QString &companyDirector);
    void setCompanyPhone(const QString &companyPhone);
    void setCompanyFax(const QString &companyFax);
    void setCompanyMailAddress(const QString &companyMailAddress);
    void setCompanyOfficialAddress(const QString &companyOfficialAddress);    
    void setIsAdmin(const bool &isAdmin);
    void setPriceType(const int priceType);

    const QString display() const;
    const QString regDateTitle() const;
    const QString countOrdersTitle() const;    

    const QString priceTypeDisplay() const;
    const QString displayName() const;
    const QString getLogin() const;
    const QString passwordHash() const;
    const QString email() const;
    const QString phone() const;
    const QString postIndex() const;
    const QString address() const;
    const QDate regDate() const;
    const QDate birthDate() const;
    qreal discount() const;
    int countOrders() const;
    int priceType() const;
    const QString docSer() const;
    const QString docNum() const;
    const QString docRegistr() const;
    const QString skype() const;
    const QString companyName() const;
    const QString companyDirector();
    const QString companyPhone() const;
    const QString companyFax() const;
    const QString companyMailAddress() const;
    const QString companyOfficialAddress() const;
    bool isAdmin() const;
    int loyalty() const;
    Gender gender() const;
    ListPersonalsAccounts *personalAccount() const;
    ListCompanyRequisites *companyRequisites() const;

    void recalcAccountsBalance();

    double balance() const;
    const QString balanceTitle() const;   
    void clearData();
    bool isModified() const;

private:
    Gender mGender;
    QString mLogin;
    QString mPasswordHash;
    QString mEmail;
    QString mPhone;
    QString mPostIndex;
    QString mAddress;
    QString mSkype;
    QDate mRegDate;
    QDate mBirthDate;
    qreal mDiscount;
    int mCountOrders;
    int mLoyalty;
    int mPriceType;
    QString mDocSer;
    QString mDocNum;
    QString mDocRegistr;    
    QString mCompanyName;
    QString mCompanyDirector;
    QString mCompanyPhone;
    QString mCompanyFax;
    QString mCompanyMailAddress;
    QString mCompanyOfficialAddress;
    bool mIsAdmin;
    ListCompanyRequisites *mCompanyRequisites;
    ListPersonalsAccounts *mListAccount;    
};

Q_DECLARE_METATYPE (DataContact *)

typedef SEVector<DataContact *> ListContacts;

#endif // DATACONTACT_H
