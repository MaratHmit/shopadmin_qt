#ifndef DATAPAYSYSTEM_H
#define DATAPAYSYSTEM_H

#include "dataitem.h"
#include "datapayrequisite.h"
#include "dataproduct.h"

class DataPaySystem : public DataItem
{
public:
    DataPaySystem();
    DataPaySystem(const DataPaySystem *d);
    DataPaySystem(const QJsonObject &jsonobject);    
    ~DataPaySystem();

    void setFromJSONObject(const QJsonObject &jsonobject);
    QJsonObject getAsJsonObject() const;
    void clearData();
    void copy(const DataPaySystem *d);
    void initialization();

    void setIsExtBlank(const bool &isExtBlank);
    void setIsAuthorize(const bool &isAuthorize);
    void setIsAdvance(const bool &isAdvance);
    void setIsTestMode(const bool &isTestMode);
    void setUrlHelp(const QString &urlHelp);
    void setIdentifier(const QString &identifier);
    void setPageSuccess(const QString &pageSuccess);
    void setPageFail(const QString &pageFail);
    void setPageBlank(const QString &pageBlank);
    void setPageResult(const QString &pageResult);
    void setPageMainInfo(const QString &pageMainInfo);

    bool isExtBlank() const;
    bool isAuthorize() const;
    bool isAdvance() const;
    bool isTestMode() const;
    QString urlHelp() const;
    QString identifier() const;
    QString pageSuccess() const;
    QString pageFail() const;
    QString pageBlank() const;
    QString pageResult() const;
    QString pageMainInfo() const;
    ListDataItems *payReqs() const;
    ListProducts *filters() const;
    ListDataItems *hosts() const;    

    bool isModified() const;
    void setUnModifiedAll();

private:
    bool mIsExtBlank; // внешний бланк
    bool mIsAuthorize; // авторизация
    bool mIsAdvance; // предоплата
    bool mIsTestMode; // тестовый режим
    QString mUrlHelp;
    QString mIdentifier; // идентификатор плагина
    QString mPageSuccess; // страница успеха
    QString mPageFail;  // страница неудачи
    QString mPageBlank; // страница бланка
    QString mPageResult; // страница результата
    QString mPageMainInfo; // страница с пояснениями    
    ListDataItems *mPaysReqs; // платежные реквизиты
    ListProducts *mFilters;  // фильтры товаров
    ListDataItems *mHosts; // хосты
};

typedef SEVector<DataPaySystem *> ListDataPaySystems;

#endif // DATAPAYSYSTEM_H
