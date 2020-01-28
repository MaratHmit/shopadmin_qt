#ifndef DATAORDER_H
#define DATAORDER_H

#include <QDate>
#include <QMap>

#include "dataorderitem.h"
#include "datapayment.h"

class DataOrder : public DataImages
{
    Q_OBJECT
    Q_PROPERTY(QDate dateOrder READ dateOrder)
    Q_PROPERTY(QString customer READ customer)
    Q_PROPERTY(QString customerEmail READ customerEmail)
    Q_PROPERTY(QString customerPhone READ customerPhone)
    Q_PROPERTY(QString sum READ sumTitle)
    Q_PROPERTY(QString dateDelivery READ deliveryDateTitle)
    Q_PROPERTY(QString statusOrder READ statusOrderTitle)
    Q_PROPERTY(QString statusDelivery READ statusDeliveryTitle)
    Q_PROPERTY(QString discount READ discountSumTitle)
    Q_PROPERTY(QString paymentTypePrimary READ paymentTypePrimary)    
    Q_PROPERTY(QString deliveryDocInfo READ deliveryDocInfo)
    Q_PROPERTY(QString productName READ productName)
    Q_PROPERTY(QString isSentTitle READ isSentTitle)

public:
    DataOrder();
    DataOrder(const QJsonObject &jsonobject);
    DataOrder(const DataOrder *d);
    ~DataOrder();

    const QDate dateOrder() const;
    const QString customer() const;
    const QString customerPhone() const;    
    const QString customerEmail() const;
    const QString idCustomer() const;    
    qreal sum() const;    
    const QString statusOrder() const;
    const QString statusDelivery() const;
    const QDate deliveryDate() const;
    const QString deliveryId() const;
    const QString deliveryName() const;
    const QString deliveryNameRecipient() const;
    const QString deliveryPhone() const;
    const QString deliveryEmail() const;
    const QString deliveryCallTime() const;
    const QString deliveryPostIndex() const;
    const QString deliveryAddress() const;
    const QString deliveryDocInfo() const;
    const QString productName() const;
    const QString isSentTitle() const;

    const QDate deliveryDocDate() const;
    const QString deliveryDocNum() const;
    const QString deliveryServiceName() const;

    const QString currency() const;
    const QDate dateCredit() const;
    const QString paymentTypePrimary() const;
    qreal discountSum() const;
    qreal deliverySum() const;
    qreal productsSum() const;
    qreal paidSum() const;
    qreal surchargeSum() const;

    ListOrderItems *listOrderItems() const;
    ListPayments *listPayments() const;    

    const QString sumTitle() const;
    const QString deliveryDateTitle() const;
    const QString statusOrderTitle() const;
    const QString statusDeliveryTitle() const;
    const QString discountSumTitle() const;
    const QString deliverySumTitle() const;
    const QString paidSumTitle() const;
    const QString surchargeSumTitle() const;
    const QString display() const;
    const QString idProduct() const;

    void setFromJSONObject(const QJsonObject &jsonobject);
    QJsonObject getAsJsonObject() const;

    void setDateOrder(const QDate &dateOrder);
    void setIdCustomer(const QString &idCustomer);    
    void setCustomer(const QString &customer);
    void setCustomerPhone(const QString &customerPhone);        
    void setCustomerEmail(const QString &customerEmail);
    void setIdProduct(const QString &idProduct);
    void setSum(const qreal &sum);
    void setDeliveryDate(const QDate &deliveryDate);
    void setStatusOrder(const QString &statusOrder);
    void setStatusDelivery(const QString &statusDelivery);
    void setDeliveryId(const QString deliveryId);
    void setDeliveryName(const QString deliveryName);
    void setDeliveryNameRecipient(const QString deliveryNameRecipient);
    void setDeliveryPhone(const QString deliveryPhone);
    void setDeliveryEmail(const QString deliveryEmail);
    void setDeliveryCallTime(const QString deliveryCallTime);
    void setDeliveryPostIndex(const QString deliveryPostIndex);
    void setDeliveryAddress(const QString deliveryAddress);

    void setDeliveryDocNum(const QString deliveryDocNum);
    void setDeliveryDocDate(const QDate &deliveryDocDate);
    void setDeliveryServiceName(const QString &deliveryServiceName);

    void setDeliverySum(const qreal deliverySum);
    void setDiscountSum(const qreal discountSum);
    void setPaidSum(const qreal paidSum);
    void setIsSendToEmail(const bool &isSendToEmail);
    void setCanceled(const bool isCanceled);
    void setDateCredit(const QDate &dateCredit);

    void clearData();
    void copy(const DataOrder *d);
    void copy(const DataItem *d);

    const QColor getColorByOrderStatus() const;
    const QColor getColorByDeliveryStatus() const;
    const QColor getColorByPropertyName(const QString &propertyName) const;

    bool isModified() const;

protected:
    void initialization();

private:
    QDate mDateOrder;    
    QString mIdCustomer;
    QString mCustomer;
    QString mCustomerPhone;    
    QString mCustomerEmail;
    QString mCurrency;
    qreal mSum;    
    qreal mDeliverySum;
    qreal mDiscountSum;
    QString mStatusOrder;
    QString mStatusDelivery;
    QString mDeliveryId;
    QString mDeliveryName;
    QDate mDeliveryDate;
    QString mDeliveryNameRecipient;
    QString mDeliveryPhone;
    QString mDeliveryEmail;
    QString mDeliveryCallTime;
    QString mDeliveryPostIndex;
    QString mDeliveryAddress;    
    QString mDeliveryDocInfo;
    QString mProductName;
    qreal mPaidSum;
    bool mIsSendToEmail;
    bool mIsCanceled;
    QDate mDateCredit;
    QString mPaymentTypePrimary;
    QString mIdProduct;

    QDate mDeliveryDocDate;
    QString mDeliveryDocNum;
    QString mDeliveryServiceName;

    ListOrderItems *mListItems;
    ListPayments *mListPayments;    

};

typedef SEVector<DataOrder *> ListOrders;

#endif // DATAORDER_H
