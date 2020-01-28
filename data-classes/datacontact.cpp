#include "datacontact.h"
#include "seutils.h"

#include <QMessageBox>

DataContact::DataContact()
{
    initialization();
}

DataContact::DataContact(const DataContact *d)
{
    initialization();
    copy(d);
}

DataContact::DataContact(const QJsonObject &jsonobject)
{
    initialization();
    setFromJSONObject(jsonobject);
}

DataContact::~DataContact()
{       
    delete mCompanyRequisites;
    delete mListAccount;    
}

void DataContact::copy(const DataContact *d)
{
    DataImages::copy(d);

    mLogin = d->mLogin;
    mGender = d->mGender;
    mEmail = d->mEmail;
    mPhone = d->mPhone;
    mPostIndex = d->mPostIndex;
    mAddress = d->mAddress;
    mSkype = d->mSkype;
    mRegDate = d->mRegDate;
    mBirthDate = d->mBirthDate;
    mDiscount = d->mDiscount;
    mCountOrders = d->mCountOrders;
    mDocSer = d->mDocSer;
    mDocNum = d->mDocNum;
    mDocRegistr = d->mDocRegistr;
    mPriceType = d->mPriceType;
}

void DataContact::setFromJSONObject(const QJsonObject &jsonobject)
{
    DataImages::setFromJSONObject(jsonobject);

    if (!jsonobject["login"].isNull())
        mLogin = jsonobject["login"].toString();
    if (!jsonobject["gender"].isNull()) {
        if (jsonobject["gender"].toString() == "F")
          mGender = Female;
        if (jsonobject["gender"].toString() == "M")
          mGender = Men;
    }
    if (!jsonobject["priceType"].isNull())
        mPriceType = jsonobject["priceType"].toInt();
    if (!jsonobject["email"].isNull())
        mEmail = jsonobject["email"].toString();
    if (!jsonobject["phone"].isNull())
        mPhone = jsonobject["phone"].toString();
    if (!jsonobject["postIndex"].isNull())
        mPostIndex = jsonobject["postIndex"].toString();
    if (!jsonobject["address"].isNull())
        mAddress = jsonobject["address"].toString();
    if (!jsonobject["skype"].isNull())
        mSkype = jsonobject["skype"].toString();
    if (jsonobject["regDate"].isString() && !jsonobject["regDate"].toString().isEmpty())
        mRegDate = QDate::fromString(jsonobject["regDate"].toString(), "yyyy-MM-dd");
    if (jsonobject["birthDate"].isString() && !jsonobject["birthDate"].toString().isEmpty())
        mBirthDate = QDate::fromString(jsonobject["birthDate"].toString(), "yyyy-MM-dd");
    if (jsonobject["discount"].isDouble() && jsonobject["discount"].toDouble() > 0)
        mDiscount = jsonobject["discount"].toDouble();
    if (!jsonobject["docSer"].isNull())
        mDocSer = jsonobject["docSer"].toString();
    if (!jsonobject["docNum"].isNull())
        mDocNum = jsonobject["docNum"].toString();
    if (!jsonobject["docRegistr"].isNull())
        mDocRegistr = jsonobject["docRegistr"].toString();
    if (!jsonobject["loyalty"].isNull())
        mLoyalty = jsonobject["loyalty"].toInt();
    if (!jsonobject["companyName"].isNull())
        mCompanyName = jsonobject["companyName"].toString();
    if (!jsonobject["companyDirector"].isNull())
        mCompanyDirector = jsonobject["companyDirector"].toString();
    if (!jsonobject["companyPhone"].isNull())
        mCompanyPhone = jsonobject["companyPhone"].toString();
    if (!jsonobject["companyFax"].isNull())
        mCompanyFax = jsonobject["companyFax"].toString();
    if (!jsonobject["companyMailAddress"].isNull())
        mCompanyMailAddress = jsonobject["companyMailAddress"].toString();
    if (!jsonobject["companyOfficialAddress"].isNull())
        mCompanyOfficialAddress = jsonobject["companyOfficialAddress"].toString();
    if (!jsonobject["isAdmin"].isNull())
        mIsAdmin = jsonobject["isAdmin"].toBool();
    if (jsonobject["companyRequisites"].isArray()) {
        QJsonArray items = jsonobject["companyRequisites"].toArray();
        foreach (const QJsonValue& value, items)
            mCompanyRequisites->append(new DataCompanyRequisite(value.toObject()));
        mCompanyRequisites->setUnModified();
    }
    if (!jsonobject["countOrders"].isNull()) {
        if (jsonobject["countOrders"].isString())
            mCountOrders = QString(jsonobject["countOrders"].toString()).toInt();
        if (jsonobject["countOrders"].isDouble())
            mCountOrders = jsonobject["countOrders"].toInt();
    }
    if (jsonobject["personalAccount"].isArray()) {
        mListAccount->deleteItems();
        QJsonArray items = jsonobject["personalAccount"].toArray();
        foreach (const QJsonValue& value, items)
            mListAccount->append(new DataPersonalAccount(value.toObject()));
    }
}

void DataContact::setLogin(const QString &login)
{
    if (mLogin != login) {
        mLogin = login;
        setModified("login");
    }
}

void DataContact::setPasswordHash(const QString &passwordHash)
{
    if (mPasswordHash != passwordHash) {
        mPasswordHash = passwordHash;
        setModified("passwordHash");
    }
}

void DataContact::setPassword(const QString &password)
{
    setPasswordHash(QMD5(password));
}

void DataContact::setGender(const DataContact::Gender gender)
{
    if (mGender != gender) {
        mGender = gender;
        setModified("gender");
    }
}

void DataContact::setEmail(const QString &email)
{
    if (mEmail != email) {
        mEmail = email;
        setModified("email");
    }
}

void DataContact::setPhone(const QString &phone)
{
    if (mPhone != phone) {
        mPhone = phone;
        setModified("phone");
    }
}

void DataContact::setPostIndex(const QString &postIndex)
{
    if (mPostIndex != postIndex) {
        mPostIndex = postIndex;
        setModified("postIndex");
    }
}

void DataContact::setAddress(const QString &address)
{
    if (mAddress != address) {
        mAddress = address;
        setModified("address");
    }
}

void DataContact::setRegDate(const QDate &regDate)
{
    if (mRegDate != regDate) {
        mRegDate = regDate;
        setModified("regDate");
    }
}

void DataContact::setBirthDate(const QDate &birthDate)
{
    if (mBirthDate != birthDate) {
        mBirthDate = birthDate;
        setModified("birthDate");
    }
}

void DataContact::setCountOrders(const int countOrders)
{
    if (mCountOrders != countOrders) {
        mCountOrders = countOrders;
        setModified("countOrders");
    }
}

void DataContact::setDiscount(const qreal discount)
{    
    if (mDiscount != discount) {
        mDiscount = discount;
        setModified("discount");
    }
}

void DataContact::setDocSer(const QString &docSer)
{
    if (mDocSer != docSer) {
        mDocSer = docSer;
        setModified("docSer");
    }
}

void DataContact::setDocNum(const QString &docNum)
{
    if (mDocNum != docNum) {
        mDocNum = docNum;
        setModified("docNum");
    }
}

void DataContact::setDocRegistr(const QString &docRegistr)
{
    if (mDocRegistr != docRegistr) {
        mDocRegistr = docRegistr;
        setModified("docRegistr");
    }
}

void DataContact::setSkype(const QString &skype)
{
    if (mSkype != skype) {
        mSkype = skype;
        setModified("skype");
    }
}

void DataContact::setLoyalty(const int &loyalty)
{
    if (mLoyalty != loyalty) {
        mLoyalty = loyalty;
        setModified("loyalty");
    }
}

void DataContact::setCompanyName(const QString &companyName)
{
    if (mCompanyName != companyName) {
        mCompanyName = companyName;
        setModified("companyName");
    }
}

void DataContact::setCompanyDirector(const QString &companyDirector)
{
    if (mCompanyDirector != companyDirector) {
        mCompanyDirector = companyDirector;
        setModified("companyDirector");
    }
}

void DataContact::setCompanyPhone(const QString &companyPhone)
{
    if (mCompanyPhone != companyPhone) {
        mCompanyPhone = companyPhone;
        setModified("companyPhone");
    }
}

void DataContact::setCompanyFax(const QString &companyFax)
{
    if (mCompanyFax != companyFax) {
        mCompanyFax = companyFax;
        setModified("companyFax");
    }
}

void DataContact::setCompanyMailAddress(const QString &companyMailAddress)
{
    if (mCompanyMailAddress != companyMailAddress) {
        mCompanyMailAddress = companyMailAddress;
        setModified("companyMailAddress");
    }
}

void DataContact::setCompanyOfficialAddress(const QString &companyOfficialAddress)
{
    if (mCompanyOfficialAddress != companyOfficialAddress) {
        mCompanyOfficialAddress = companyOfficialAddress;
        setModified("companyOfficialAddress");
    }
}

void DataContact::setIsAdmin(const bool &isAdmin)
{
    if (mIsAdmin != isAdmin) {
        mIsAdmin = isAdmin;
        setModified("isAdmin");
    }
}

void DataContact::setPriceType(const int priceType)
{
    if (mPriceType != priceType) {
        mPriceType = priceType;
        setModified("priceType");
    }
}

const QString DataContact::display() const
{
    return displayName();
}

const QString DataContact::regDateTitle() const
{
    return mRegDate.toString("dd.MM.yyyy");
}

const QString DataContact::countOrdersTitle() const
{
    return QString::number(mCountOrders);
}

const QString DataContact::priceTypeDisplay() const
{
    switch (mPriceType) {
    case 0:
        return "Розничная";
    case 1:
        return "Мелкооптовая";
    case 2:
        return "Оптовая";
    default:
        break;
    }
    return "Розничная";
}

const QString DataContact::displayName() const
{
    QString res = "";
    if (!property("lastName").isNull())
        res = property("lastName").toString();
    if (!property("firstName").isNull()) {
        if (!res.isEmpty())
            res += " ";
        res += property("firstName").toString();
    }
    if (!property("secondName").isNull()) {
        if (!res.isEmpty())
            res += " ";
        res += property("secondName").toString();
    }
    return res;
}

const QString DataContact::getLogin() const
{
    return mLogin;
}

const QString DataContact::passwordHash() const
{
    return mPasswordHash;
}

const QString DataContact::email() const
{
    return mEmail;
}

const QString DataContact::phone() const
{
    return mPhone;
}

const QString DataContact::postIndex() const
{
    return mPostIndex;
}

const QString DataContact::address() const
{
    return mAddress;
}

const QDate DataContact::regDate() const
{
    return mRegDate;
}

const QDate DataContact::birthDate() const
{
    return mBirthDate;
}

qreal DataContact::discount() const
{
    return mDiscount;
}

int DataContact::countOrders() const
{
    return mCountOrders;
}

int DataContact::priceType() const
{
    return mPriceType;
}

const QString DataContact::docSer() const
{
    return mDocSer;
}

const QString DataContact::docNum() const
{
    return mDocNum;
}

const QString DataContact::docRegistr() const
{
    return mDocRegistr;
}

const QString DataContact::skype() const
{
    return mSkype;
}

const QString DataContact::companyName() const
{
    return mCompanyName;
}

const QString DataContact::companyDirector()
{
    return mCompanyDirector;
}

const QString DataContact::companyPhone() const
{
    return mCompanyPhone;
}

const QString DataContact::companyFax() const
{
    return mCompanyFax;
}

const QString DataContact::companyMailAddress() const
{
    return mCompanyMailAddress;
}

const QString DataContact::companyOfficialAddress() const
{
    return mCompanyOfficialAddress;
}

bool DataContact::isAdmin() const
{
    return mIsAdmin;
}

int DataContact::loyalty() const
{
    return mLoyalty;
}

DataContact::Gender DataContact::gender() const
{    
    return mGender;
}

ListPersonalsAccounts *DataContact::personalAccount() const
{
    return mListAccount;
}

ListCompanyRequisites *DataContact::companyRequisites() const
{
    return mCompanyRequisites;
}

void DataContact::recalcAccountsBalance()
{
    double balance = 0;
    for (int i = 0; i < mListAccount->size(); ++i) {
        DataPersonalAccount *account = mListAccount->value(i);
        balance += account->inPayee();
        balance -= account->outPayee();
        account->setBalance(balance);
    }
}

double DataContact::balance() const
{
    if (mListAccount->size())
        return mListAccount->at(mListAccount->size() - 1)->balance();
    return 0;
}

const QString DataContact::balanceTitle() const
{
    if (mListAccount->size())
        return mListAccount->at(mListAccount->size() - 1)->balanceTitle();
    return QString();
}

void DataContact::clearData()
{
    DataImages::clearData();

    mGender = Unknown;
    mEmail = QString();
    mPhone = QString();
    mPostIndex = QString();
    mAddress = QString();
    mSkype = QString();
    mDiscount = 0;
    mCountOrders = 0;
    mPriceType = 0;
    mDocSer = QString();
    mDocNum = QString();
    mDocRegistr = QString();
    mCompanyName = QString();
    mCompanyDirector = QString();
    mCompanyPhone = QString();
    mCompanyFax = QString();
    mCompanyMailAddress = QString();
    mCompanyOfficialAddress = QString();

    mCompanyRequisites->deleteItems();    
}

bool DataContact::isModified() const
{
    return DataImages::isModified() || mCompanyRequisites->isModified() ||
            mListAccount->isModified();
}

QJsonObject DataContact::getAsJsonObject() const
{
    QJsonObject obj = DataImages::getAsJsonObject();

    if (isSetAllFields || modifiedFields.contains("regDate"))
        obj.insert("regDate", QJsonValue(mRegDate.toString("yyyy-MM-dd")));
    if (isSetAllFields || modifiedFields.contains("priceType"))
        obj.insert("priceType", QJsonValue(mPriceType));
    if (isSetAllFields || modifiedFields.contains("login"))
        obj.insert("login", QJsonValue(mLogin));
    if (isSetAllFields || modifiedFields.contains("passwordHash"))
        obj.insert("passwordHash", QJsonValue(mPasswordHash));
    if (isSetAllFields || modifiedFields.contains("gender"))
        obj.insert("gender", QJsonValue(QString(mGender)));
    if (isSetAllFields || modifiedFields.contains("email"))
        obj.insert("email", QJsonValue(mEmail));
    if (isSetAllFields || modifiedFields.contains("skype"))
        obj.insert("skype", QJsonValue(mSkype));
    if (isSetAllFields || modifiedFields.contains("phone"))
        obj.insert("phone", QJsonValue(mPhone));
    if (isSetAllFields || modifiedFields.contains("postIndex"))
        obj.insert("postIndex", QJsonValue(mPostIndex));
    if (isSetAllFields || modifiedFields.contains("address"))
        obj.insert("address", QJsonValue(mAddress));
    if (isSetAllFields || modifiedFields.contains("birthDate"))
        obj.insert("birthDate", QJsonValue(mBirthDate.toString("yyyy-MM-dd")));
    if (isSetAllFields || modifiedFields.contains("docSer"))
        obj.insert("docSer", QJsonValue(mDocSer));
    if (isSetAllFields || modifiedFields.contains("docNum"))
        obj.insert("docNum", QJsonValue(mDocNum));
    if (isSetAllFields || modifiedFields.contains("docRegistr"))
        obj.insert("docRegistr", QJsonValue(mDocRegistr));
    if (isSetAllFields || modifiedFields.contains("discount"))
        obj.insert("discount", QJsonValue(mDiscount));
    if (isSetAllFields || modifiedFields.contains("loyalty"))
        obj.insert("loyalty", QJsonValue(mLoyalty));
    if (isSetAllFields || modifiedFields.contains("companyName"))
        obj.insert("companyName", QJsonValue(mCompanyName));
    if (isSetAllFields || modifiedFields.contains("companyDirector"))
        obj.insert("companyDirector", QJsonValue(mCompanyDirector));
    if (isSetAllFields || modifiedFields.contains("companyPhone"))
        obj.insert("companyPhone", QJsonValue(mCompanyPhone));
    if (isSetAllFields || modifiedFields.contains("companyFax"))
        obj.insert("companyFax", QJsonValue(mCompanyFax));
    if (isSetAllFields || modifiedFields.contains("companyMailAddress"))
        obj.insert("companyMailAddress", QJsonValue(mCompanyMailAddress));
    if (isSetAllFields || modifiedFields.contains("companyOfficialAddress"))
        obj.insert("companyOfficialAddress", QJsonValue(mCompanyOfficialAddress));
    if (isSetAllFields || modifiedFields.contains("isAdmin"))
        obj.insert("isAdmin", QJsonValue(mIsAdmin));
    if (mCompanyRequisites->isModified()) {
        mCompanyRequisites->setIsSetAllFields(true);
        obj.insert("companyRequisites", QJsonValue(mCompanyRequisites->getAsJsonArray()));
    }
    if (mListAccount->isModified()) {
        mListAccount->setIsSetAllFields(true);
        obj.insert("personalAccount", QJsonValue(mListAccount->getAsJsonArray()));
    }    

    return obj;
}

void DataContact::initialization()
{
    DataImages::initialization();
    mCompanyRequisites = new ListCompanyRequisites;
    mListAccount = new ListPersonalsAccounts;
    mGender = Unknown;
    mDiscount = 0;
    mGender = Unknown;
    mCountOrders = 0;
    mPriceType = 0;
}
