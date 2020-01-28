#ifndef DATAPAYMENT_H
#define DATAPAYMENT_H

#include "dataitem.h"

class DataPayment : public DataItem
{
    Q_OBJECT
    Q_PROPERTY(QString docNum READ docNumTitle)
    Q_PROPERTY(QString paymentTarget READ paymentTargetTitle)
    Q_PROPERTY(QString docDate READ docDateTitle)
    Q_PROPERTY(QString payerName READ payerName)
    Q_PROPERTY(QString amount READ amountTitle)
    Q_PROPERTY(QString idOrder READ idOrderTitle)

public:
    DataPayment();
    DataPayment(const QJsonObject &jsonobject);
    DataPayment(const DataPayment *d);
    ~DataPayment();

    void setFromJSONObject(const QJsonObject &jsonobject);
    QJsonObject getAsJsonObject() const;
    void clearData();
    void copy(const DataPayment *d);
    void copy(const DataItem *d);
    void initialization();

    const QString idOrder() const;
    const QString idPayer() const;
    const QString docNum() const;
    const QString payerName() const;
    const QDate docDate() const;
    int docYear() const;
    const QString idPaymentType() const;
    const QString idManager() const;
    double amount() const;
    double orderAmount() const;
    const QString idUserAccountIn() const;
    const QString idUserAccountOut() const;    
    int paymentTarget() const;
    const QString currency() const;

    const QString docNumTitle() const;
    const QString docDateTitle() const;
    const QString amountTitle() const;
    const QString idOrderTitle() const;    
    const QString paymentTargetTitle() const;

    const QString display() const;

    void setIdOrder(const QString &idOrder);
    void setIdPayer(const QString &idPayer);
    void setDocNum(const QString &docNum);
    void setPayerName(const QString &payerName);
    void setDocDate(const QDate &docDate);
    void setDocYear(const int &docYear);
    void setIdPaymentType(const QString &idPaymentType);
    void setIdManager(const QString &idManager);
    void setAmount(const double &amount);
    void setOrderAmount(const double &orderAmount);
    void setIdUserAccountIn(const QString &idUserAccountIn);
    void setIdUserAccountOut(const QString &idUserAccountOut);    
    void setPaymentTarget(const int paymentTarget);
    void setCurrency(const QString &currency);

private:
  QString mIdOrder;
  QString mIdPayer;
  QString mDocNum;
  QString mPayerName;
  QDate mDocDate;
  int mDocYear;
  QString mIdPaymentType;
  QString mIdManager;
  double mAmount;  
  double mOrderAmount;
  QString mIdUserAccountIn;
  QString mIdUserAccountOut;
  int mPaymentTarget;
  QString mCurrency;

};

typedef SEVector<DataPayment *> ListPayments;

#endif // DATAPAYMENT_H
