#ifndef DATAPERSONALACCOUNT_H
#define DATAPERSONALACCOUNT_H

#include "dataitem.h"

class DataPersonalAccount : public DataItem
{
    Q_OBJECT
    Q_PROPERTY(QDate datePayee READ datePayee)
    Q_PROPERTY(QString inPayeeTitle READ inPayeeTitle)
    Q_PROPERTY(QString outPayeeTitle READ outPayeeTitle)
    Q_PROPERTY(QString balanceTitle READ balanceTitle)

public:
    DataPersonalAccount();    
    DataPersonalAccount(const DataPersonalAccount *d);
    DataPersonalAccount(const QJsonObject &jsonobject);

    virtual void copy(const DataItem *d);
    void copy(const DataPersonalAccount *d);
    virtual void setFromJSONObject(const QJsonObject &jsonobject);
    virtual QJsonObject getAsJsonObject() const; 

    QString orderId() const;
    QString account() const;
    QDate datePayee() const;
    qreal inPayee() const;
    qreal outPayee() const;
    qreal balance() const;
    QString currency() const;
    int typeOperation() const;
    QString inPayeeTitle() const;
    QString outPayeeTitle() const;
    QString balanceTitle() const;    

public slots:
    void setOrderId(const QString &orderId);
    void setAccount(const QString &account);
    void setDatePayee(const QDate &datePayee);
    void setInPayee(const qreal &inPayee);
    void setOutPayee(const qreal &outPayee);
    void setCurrency(const QString &currency);
    void setTypeOperation(const int &typeOperation);
    void setBalance(const qreal &balance);

protected:
    void initialization();

private:
    QString mOrderId;
    QString mAccount;
    QDate mDatePayee;
    qreal mInPayee;
    qreal mOutPayee;
    qreal mBalance;
    QString mCurrency;
    int mTypeOperation;

};

typedef SEVector<DataPersonalAccount *> ListPersonalsAccounts;

#endif // DATAPERSONALACCOUNT_H
