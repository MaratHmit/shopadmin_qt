#include "dataorder.h"
#include "seconfig.h"

DataOrder::DataOrder()
{
    initialization();
}

DataOrder::DataOrder(const QJsonObject &jsonobject)
{
    initialization();
    setFromJSONObject(jsonobject);
}

DataOrder::DataOrder(const DataOrder *d)
{
    initialization();
    copy(d);
}

DataOrder::~DataOrder()
{
    delete mListItems;
    delete mListPayments;    
}

const QDate DataOrder::dateOrder() const
{
    return mDateOrder;
}

const QString DataOrder::customer() const
{
    return mCustomer;
}

const QString DataOrder::customerPhone() const
{
    return mCustomerPhone;
}

const QString DataOrder::customerEmail() const
{
    return mCustomerEmail;
}

const QString DataOrder::idCustomer() const
{
    return mIdCustomer;
}


qreal DataOrder::sum() const
{
    return mSum;
}

const QString DataOrder::statusOrder() const
{
    return mStatusOrder;
}

const QString DataOrder::statusDelivery() const
{
    return mStatusDelivery;
}

const QDate DataOrder::deliveryDate() const
{
    return mDeliveryDate;
}

const QString DataOrder::deliveryId() const
{
    return mDeliveryId;
}

const QString DataOrder::deliveryName() const
{
    return mDeliveryName;
}

const QString DataOrder::deliveryNameRecipient() const
{
    return mDeliveryNameRecipient;
}

const QString DataOrder::deliveryPhone() const
{
    return mDeliveryPhone;
}

const QString DataOrder::deliveryEmail() const
{
    return mDeliveryEmail;
}

const QString DataOrder::deliveryCallTime() const
{
    return mDeliveryCallTime;
}

const QString DataOrder::deliveryPostIndex() const
{
    return mDeliveryPostIndex;
}

const QString DataOrder::deliveryAddress() const
{
    return mDeliveryAddress;
}

const QString DataOrder::deliveryDocInfo() const
{
    return mDeliveryDocInfo;
}

const QString DataOrder::productName() const
{
    return mProductName;
}

const QString DataOrder::isSentTitle() const
{
    if (property("isSent").toBool())
        return "Отправлено";

    return "Не отправлено";
}

const QDate DataOrder::deliveryDocDate() const
{
    return mDeliveryDocDate;
}

const QString DataOrder::deliveryDocNum() const
{
    return mDeliveryDocNum;
}

const QString DataOrder::deliveryServiceName() const
{
    return mDeliveryServiceName;
}

const QString DataOrder::currency() const
{    
    return mCurrency;
}

const QDate DataOrder::dateCredit() const
{
    return mDateCredit;
}

const QString DataOrder::paymentTypePrimary() const
{
    return mPaymentTypePrimary;
}

qreal DataOrder::deliverySum() const
{
    return mDeliverySum;
}

qreal DataOrder::productsSum() const
{
    return 0;
}

qreal DataOrder::paidSum() const
{
    return mPaidSum;
}

qreal DataOrder::surchargeSum() const
{
    return mSum - mPaidSum;
}

qreal DataOrder::discountSum() const
{
    return mDiscountSum;
}

ListOrderItems *DataOrder::listOrderItems() const
{
    return mListItems;
}

ListPayments *DataOrder::listPayments() const
{
    return mListPayments;
}

const QString DataOrder::sumTitle() const
{
    return SEConfig::priceCurrency(mSum, mCurrency);
}

const QString DataOrder::deliveryDateTitle() const
{
    return mDeliveryDate.toString("dd.MM.yyyy");
}

const QString DataOrder::statusOrderTitle() const
{
    for (int i = 0; i < SEConfig::getOrdersStatuses()->size(); i++)
        if (SEConfig::getOrdersStatuses()->at(i)->getId() == mStatusOrder)
            return SEConfig::getOrdersStatuses()->at(i)->getName();
    return QString();
}

const QString DataOrder::statusDeliveryTitle() const
{
    for (int i = 0; i < SEConfig::getDeliveriesStatuses()->size(); i++)
        if (SEConfig::getDeliveriesStatuses()->at(i)->getId() == mStatusDelivery)
            return SEConfig::getDeliveriesStatuses()->at(i)->getName();
    return QString();
}

const QString DataOrder::discountSumTitle() const
{
    return SEConfig::priceCurrency(mDiscountSum, mCurrency);
}

const QString DataOrder::deliverySumTitle() const
{
    return SEConfig::priceCurrency(mDeliverySum, mCurrency);
}

const QString DataOrder::paidSumTitle() const
{
    return SEConfig::priceCurrency(mPaidSum, mCurrency);
}

const QString DataOrder::surchargeSumTitle() const
{
    return SEConfig::priceCurrency(surchargeSum(), mCurrency);
}

const QString DataOrder::display() const
{
    return "№ " + getId() + " от " + mDateOrder.toString("dd.MM.yyyy");
}

const QString DataOrder::idProduct() const
{
    return mIdProduct;
}

void DataOrder::setFromJSONObject(const QJsonObject &jsonobject)
{    
    DataImages::setFromJSONObject(jsonobject);

    if (!jsonobject["idCustomer"].isNull())
        mIdCustomer = jsonobject["idCustomer"].toString();
    if (!jsonobject["customer"].isNull())
        mCustomer = jsonobject["customer"].toString();    
    if (!jsonobject["dateOrder"].isNull())
        mDateOrder = QDate::fromString(jsonobject["dateOrder"].toString(), "yyyy-MM-dd");
    if (!jsonobject["customerPhone"].isNull())
        mCustomerPhone = jsonobject["customerPhone"].toString();
    if (!jsonobject["customerEmail"].isNull())
        mCustomerEmail = jsonobject["customerEmail"].toString();
    if (!jsonobject["sum"].isNull())
        mSum = jsonobject["sum"].toDouble();
    if (!jsonobject["deliveryDate"].isNull())
        mDeliveryDate = QDate::fromString(jsonobject["deliveryDate"].toString(), "yyyy-MM-dd");
    if (!jsonobject["statusOrder"].isNull())
        mStatusOrder = jsonobject["statusOrder"].toString();
    if (!jsonobject["statusDelivery"].isNull())
        mStatusDelivery = jsonobject["statusDelivery"].toString();
    mCurrency = jsonobject["currency"].toString();
    if (mCurrency.isEmpty())
        mCurrency = SEConfig::getMainInfo()->baseCurrency();
    if (!jsonobject["deliveryId"].isNull())
        mDeliveryId = jsonobject["deliveryId"].toString();
    if (!jsonobject["deliveryName"].isNull())
        mDeliveryName = jsonobject["deliveryName"].toString();
    if (!jsonobject["deliveryNameRecipient"].isNull())
        mDeliveryNameRecipient = jsonobject["deliveryNameRecipient"].toString();
    if (!jsonobject["deliveryPhone"].isNull())
        mDeliveryPhone = jsonobject["deliveryPhone"].toString();
    if (!jsonobject["deliveryEmail"].isNull())
        mDeliveryEmail = jsonobject["deliveryEmail"].toString();
    if (!jsonobject["deliveryCallTime"].isNull())
        mDeliveryCallTime = jsonobject["deliveryCallTime"].toString();
    if (!jsonobject["deliveryAddress"].isNull())
        mDeliveryAddress = jsonobject["deliveryAddress"].toString();
    if (!jsonobject["deliveryPostIndex"].isNull())
        mDeliveryPostIndex = jsonobject["deliveryPostIndex"].toString();
    if (!jsonobject["deliveryDocNum"].isNull())
        mDeliveryDocNum = jsonobject["deliveryDocNum"].toString();
    if (!jsonobject["deliveryDocDate"].isNull())
        mDeliveryDocDate = QDate::fromString(jsonobject["deliveryDocDate"].toString(), "yyyy-MM-dd");
    if (!jsonobject["deliveryServiceName"].isNull())
        mDeliveryServiceName = jsonobject["deliveryServiceName"].toString();
    if (!jsonobject["deliveryDocInfo"].isNull())
        mDeliveryDocInfo = jsonobject["deliveryDocInfo"].toString();
    if (!jsonobject["deliverySum"].isNull())
        mDeliverySum = jsonobject["deliverySum"].toDouble();
    if (!jsonobject["discountSum"].isNull())
        mDiscountSum = jsonobject["discountSum"].toDouble();
    if (!jsonobject["paidSum"].isNull())
        mPaidSum = jsonobject["paidSum"].toDouble();
    if (!jsonobject["isCanceled"].isNull())
        mIsCanceled = jsonobject["isCanceled"].toBool();
    if (!jsonobject["dateCredit"].isNull())
        mDateCredit = QDate::fromString(jsonobject["dateCredit"].toString(), "yyyy-MM-dd");
    if (!jsonobject["paymentTypePrimary"].isNull())
        mPaymentTypePrimary = jsonobject["paymentTypePrimary"].toString();
    if (!jsonobject["productName"].isNull())
        mProductName = jsonobject["productName"].toString();
    if (!jsonobject["idProduct"].isNull())
        mIdProduct = jsonobject["idProduct"].toString();

    if (!jsonobject["items"].isNull()) {
        mListItems->deleteItems();
        QJsonArray items = jsonobject["items"].toArray();
        foreach (const QJsonValue& value, items)
            mListItems->append(new DataOrderItem(value.toObject()));
        mListItems->setUnModified();
    }

    if (!jsonobject["payments"].isNull()) {
        mListPayments->deleteItems();
        QJsonArray items = jsonobject["payments"].toArray();
        foreach (const QJsonValue& value, items)
            mListPayments->append(new DataPayment(value.toObject()));
        mListPayments->setUnModified();
    }

}

QJsonObject DataOrder::getAsJsonObject() const
{
    QJsonObject obj = DataImages::getAsJsonObject();

    if (mIsSendToEmail)
        obj.insert("isSendToEmail", QJsonValue(true));
    if (!mDateOrder.isNull() && (isSetAllFields || modifiedFields.contains("dateOrder")))
        obj.insert("dateOrder", QJsonValue(mDateOrder.toString("yyyy-MM-dd")));
    if (isSetAllFields || modifiedFields.contains("customer"))
        obj.insert("customer", QJsonValue(mCustomer));
    if (isSetAllFields || modifiedFields.contains("customerPhone"))
        obj.insert("customerPhone", QJsonValue(mCustomerPhone));
    if (isSetAllFields || modifiedFields.contains("customerEmail"))
        obj.insert("customerEmail", QJsonValue(mCustomerEmail));
    if (isSetAllFields || modifiedFields.contains("idCustomer"))
        obj.insert("idCustomer", QJsonValue(mIdCustomer));    
    if (!mDeliveryDate.isNull() && (isSetAllFields || modifiedFields.contains("deliveryDate")))
        obj.insert("deliveryDate", QJsonValue(mDeliveryDate.toString("yyyy-MM-dd")));
    obj.insert("currency", QJsonValue(mCurrency));
    if (mDiscountSum >= 0 && (isSetAllFields || modifiedFields.contains("discountSum")))
        obj.insert("discountSum", QJsonValue(mDiscountSum));
    if (mDeliverySum >= 0 && (isSetAllFields || modifiedFields.contains("deliverySum")))
        obj.insert("deliverySum", QJsonValue(mDeliverySum));
    if (isSetAllFields || modifiedFields.contains("statusOrder"))
        obj.insert("statusOrder", QJsonValue(mStatusOrder));
    if (isSetAllFields || modifiedFields.contains("statusDelivery"))
        obj.insert("statusDelivery", QJsonValue(mStatusDelivery));
    if (isSetAllFields || modifiedFields.contains("deliveryId"))
        obj.insert("deliveryId", QJsonValue(mDeliveryId));
    if (isSetAllFields || modifiedFields.contains("deliveryEmail"))
        obj.insert("deliveryEmail", QJsonValue(mDeliveryEmail));
    if (isSetAllFields || modifiedFields.contains("deliveryPhone"))
        obj.insert("deliveryPhone", QJsonValue(mDeliveryPhone));
    if (isSetAllFields || modifiedFields.contains("deliveryCallTime"))
        obj.insert("deliveryCallTime", QJsonValue(mDeliveryCallTime));
    if (isSetAllFields || modifiedFields.contains("deliveryPostIndex"))
        obj.insert("deliveryPostIndex", QJsonValue(mDeliveryPostIndex));
    if (isSetAllFields || modifiedFields.contains("deliveryAddress"))
        obj.insert("deliveryAddress", QJsonValue(mDeliveryAddress));
    if (isSetAllFields || modifiedFields.contains("isCanceled"))
        obj.insert("isCanceled", QJsonValue(mIsCanceled));
    if (isSetAllFields || modifiedFields.contains("deliveryDocNum"))
        obj.insert("deliveryDocNum", QJsonValue(mDeliveryDocNum));
    if (isSetAllFields || modifiedFields.contains("deliveryDocDate"))
        obj.insert("deliveryDocDate", QJsonValue(mDeliveryDocDate.toString("yyyy-MM-dd")));
    if (isSetAllFields || modifiedFields.contains("deliveryServiceName"))
        obj.insert("deliveryServiceName", QJsonValue(mDeliveryServiceName));
    if (!mDateCredit.isNull() && (isSetAllFields || modifiedFields.contains("dateCredit")))
        obj.insert("dateCredit", QJsonValue(mDateCredit.toString("yyyy-MM-dd")));
    if (isSetAllFields || modifiedFields.contains("idProduct"))
        obj.insert("idProduct", QJsonValue(mIdProduct));

    if (isSetAllFields || mListItems->isModified()) {
        mListItems->setIsSetAllFields(true);
        obj.insert("items", QJsonValue(mListItems->getAsJsonArray()));
    }
    if (isSetAllFields || mListPayments->isModified()) {
        mListPayments->setIsSetAllFields(true);
        obj.insert("payments", QJsonValue(mListPayments->getAsJsonArray()));
    }

    return obj;
}

void DataOrder::setDateOrder(const QDate &dateOrder)
{
    if (mDateOrder != dateOrder) {
        mDateOrder = dateOrder;
        setModified("dateOrder");
    }
}

void DataOrder::setCustomer(const QString &customer)
{
    if (mCustomer != customer) {
        mCustomer = customer;
        setModified("customer");
    }
}

void DataOrder::setCustomerPhone(const QString &customerPhone)
{
    if (mCustomerPhone != customerPhone) {
        mCustomerPhone = customerPhone;
        setModified("customerPhone");
    }
}

void DataOrder::setCustomerEmail(const QString &customerEmail)
{
    if (mCustomerEmail != customerEmail) {
        mCustomerEmail = customerEmail;
        setModified("customerEmail");
    }
}

void DataOrder::setIdProduct(const QString &idProduct)
{
    if (mIdProduct != idProduct) {
        mIdProduct = idProduct;
        setModified("idProduct");
    }
}

void DataOrder::setSum(const qreal &sum)
{
    if (mSum != sum) {
        mSum = sum;
        setModified("sum");
    }
}

void DataOrder::setDeliveryDate(const QDate &deliveryDate)
{
    if (mDeliveryDate != deliveryDate) {
        mDeliveryDate = deliveryDate;
        setModified("deliveryDate");
    }
}

void DataOrder::setStatusOrder(const QString &statusOrder)
{
    if (mStatusOrder != statusOrder) {
        mStatusOrder = statusOrder;
        setModified("statusOrder");
    }
}

void DataOrder::setStatusDelivery(const QString &statusDelivery)
{
    if (mStatusDelivery != statusDelivery) {
        mStatusDelivery = statusDelivery;
        setModified("statusDelivery");
    }
}

void DataOrder::setDeliveryId(const QString deliveryId)
{
    if (mDeliveryId != deliveryId) {
        mDeliveryId = deliveryId;
        setModified("deliveryId");
    }
}

void DataOrder::setDeliveryName(const QString deliveryName)
{
    if (mDeliveryName != deliveryName) {
        mDeliveryName = deliveryName;
        setModified("deliveryName");
    }
}

void DataOrder::setDeliveryNameRecipient(const QString deliveryNameRecipient)
{
    if (mDeliveryNameRecipient != deliveryNameRecipient) {
        mDeliveryNameRecipient = deliveryNameRecipient;
        setModified("deliveryNameRecipient");
    }
}

void DataOrder::setDeliveryPhone(const QString deliveryPhone)
{
    if (mDeliveryPhone != deliveryPhone) {
        mDeliveryPhone = deliveryPhone;
        setModified("deliveryPhone");
    }
}

void DataOrder::setDeliveryEmail(const QString deliveryEmail)
{
    if (mDeliveryEmail != deliveryEmail) {
        mDeliveryEmail = deliveryEmail;
        setModified("deliveryEmail");
    }
}

void DataOrder::setDeliveryCallTime(const QString deliveryCallTime)
{
    if (mDeliveryCallTime != deliveryCallTime) {
        mDeliveryCallTime = deliveryCallTime;
        setModified("deliveryCallTime");
    }
}

void DataOrder::setDeliveryPostIndex(const QString deliveryPostIndex)
{
    if (mDeliveryPostIndex != deliveryPostIndex) {
        mDeliveryPostIndex = deliveryPostIndex;
        setModified("deliveryPostIndex");
    }
}

void DataOrder::setDeliveryAddress(const QString deliveryAddress)
{
    if (mDeliveryAddress != deliveryAddress) {
        mDeliveryAddress = deliveryAddress;
        setModified("deliveryAddress");
    }
}

void DataOrder::setDeliveryDocNum(const QString deliveryDocNum)
{
    if (mDeliveryDocNum != deliveryDocNum) {
        mDeliveryDocNum = deliveryDocNum;
        setModified("deliveryDocNum");
    }
}

void DataOrder::setDeliveryDocDate(const QDate &deliveryDocDate)
{
    if (mDeliveryDocDate != deliveryDocDate) {
        mDeliveryDocDate = deliveryDocDate;
        setModified("deliveryDocDate");
    }
}

void DataOrder::setDeliveryServiceName(const QString &deliveryServiceName)
{
    if (mDeliveryServiceName != deliveryServiceName) {
        mDeliveryServiceName = deliveryServiceName;
        setModified("deliveryServiceName");
    }
}

void DataOrder::setDeliverySum(const qreal deliverySum)
{
    if (mDeliverySum != deliverySum) {
        mDeliverySum = deliverySum;
        setModified("deliverySum");
    }
}

void DataOrder::setDiscountSum(const qreal discountSum)
{
    if (mDiscountSum != discountSum) {
        mDiscountSum = discountSum;
        setModified("discountSum");
    }
}

void DataOrder::setPaidSum(const qreal paidSum)
{
    if (mPaidSum != paidSum) {
        mPaidSum = paidSum;
        setModified("paidSum");
    }
}

void DataOrder::setIsSendToEmail(const bool &isSendToEmail)
{
    mIsSendToEmail = isSendToEmail;
}

void DataOrder::setCanceled(const bool isCanceled)
{
    if (mIsCanceled != isCanceled) {
        mIsCanceled = isCanceled;
        setModified("isCanceled");
    }
}

void DataOrder::setDateCredit(const QDate &dateCredit)
{
    if (dateCredit != mDateCredit) {
        mDateCredit = dateCredit;
        setModified("dateCredit");
    }
}

void DataOrder::clearData()
{
    DataImages::clearData();

    mDateOrder = QDate::currentDate();
    mCurrency = SEConfig::getMainInfo()->baseCurrency();
    mDeliveryPhone = QString();
    mDeliveryEmail = QString();
    mDeliveryCallTime = QString();
    mDeliveryPostIndex = QString();
    mDeliveryAddress = QString();
    mDeliveryId = QString();
    mDeliveryDocNum = QString();
    mDeliveryServiceName = QString();
    mDeliverySum = 0;
    mDiscountSum = 0;
    mStatusOrder = "N";
    mSum = 0;    
    mPaidSum = 0;
    mListItems->deleteItems();      
}

void DataOrder::copy(const DataOrder *d)
{
    DataImages::copy(d);
    setDateOrder(d->dateOrder());
    setIdCustomer(d->idCustomer());
    setCustomer(d->customer());
    setCustomerPhone(d->customerPhone());
    setCustomerEmail(d->customerEmail());
    mCurrency = d->mCurrency;
    if (mCurrency.isEmpty())
        mCurrency = SEConfig::getMainInfo()->baseCurrency();
    setSum(d->sum());
    setStatusOrder(d->statusOrder());
    setStatusDelivery(d->statusDelivery());
    setDeliveryId(d->deliveryId());
    setDeliveryName(d->deliveryName());
    setDeliveryDate(d->deliveryDate());
    setDeliveryNameRecipient(d->deliveryNameRecipient());
    setDeliveryPhone(d->deliveryPhone());
    setDeliveryEmail(d->deliveryEmail());
    setDeliveryCallTime(d->deliveryCallTime());
    setDeliveryPostIndex(d->deliveryPostIndex());
    setDeliveryAddress(d->deliveryAddress());
    setDiscountSum(d->discountSum());
    setDeliverySum(d->deliverySum());
    setPaidSum(d->paidSum());
    setDeliveryDocNum(d->deliveryDocNum());
    setDeliveryDocDate(d->deliveryDocDate());
    setDeliveryServiceName(d->deliveryServiceName());
    setIdProduct(d->idProduct());
    mProductName = d->mProductName;

    mListItems->deleteItems();
    for (int i = 0; i < d->listOrderItems()->size(); i++)
        mListItems->append(new DataOrderItem(d->listOrderItems()->at(i)));
    mListItems->setUnModified();

    mListPayments->deleteItems();
    for (int i = 0; i < d->listPayments()->size(); i++)
        mListPayments->append(new DataPayment(d->listPayments()->at(i)));
    mListPayments->setUnModified();

    setUnModified();
}

void DataOrder::copy(const DataItem *d)
{
    copy(dynamic_cast <const DataOrder *> (d));
}

const QColor DataOrder::getColorByOrderStatus() const
{
    for (int i = 0; i < SEConfig::getOrdersStatuses()->size(); i++)
        if (SEConfig::getOrdersStatuses()->at(i)->getId() == mStatusOrder &&
                !SEConfig::getOrdersStatuses()->at(i)->getColorHexCode().isEmpty())
            return QColor(SEConfig::getOrdersStatuses()->at(i)->getColorHexCode());

    return QColor();
}

const QColor DataOrder::getColorByDeliveryStatus() const
{
    for (int i = 0; i < SEConfig::getDeliveriesStatuses()->size(); i++)
        if (SEConfig::getDeliveriesStatuses()->at(i)->getId() == mStatusDelivery &&
                !SEConfig::getDeliveriesStatuses()->at(i)->getColorHexCode().isEmpty())
            return QColor(SEConfig::getDeliveriesStatuses()->at(i)->getColorHexCode());

    return QColor();
}

const QColor DataOrder::getColorByPropertyName(const QString &propertyName) const
{
    if (mIsCanceled)
        return QColor(Qt::red);
    if (propertyName == "statusOrder")
        return getColorByOrderStatus();
    if (propertyName == "statusDelivery")
        return getColorByDeliveryStatus();
    if (mDateOrder == QDate::currentDate())
        return QColor("#FFF8DC");
    if (propertyName == "isSentTitle") {
        if (property("isSent").toBool())
            return Qt::green;
        return Qt::red;
    }

    return QColor();
}

bool DataOrder::isModified() const
{
    return DataImages::isModified() || mListItems->isModified() || mListPayments->isModified();
}

void DataOrder::initialization()
{
    DataImages::initialization();

    mListItems = new ListOrderItems();
    mListPayments = new ListPayments();
    mDateOrder = QDate::currentDate();    
    mCurrency = SEConfig::getMainInfo()->baseCurrency();
    mDeliverySum = 0;
    mDiscountSum = 0;
    mStatusOrder = "N";
    mSum = 0;
    mPaidSum = 0;
    mIsSendToEmail = true;
    mIsCanceled = false;
    mDeliveryDocNum = QString();
    mDeliveryServiceName = QString();    
}

void DataOrder::setIdCustomer(const QString &idCustomer)
{
    if (mIdCustomer != idCustomer) {
        mIdCustomer = idCustomer;
        setModified("idCustomer");
    }
}
