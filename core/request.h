#ifndef REQUEST_H
#define REQUEST_H

#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QList>
#include <QMap>
#include <QDate>

class Request
{

public:
    explicit Request();

    void addId(const QString &id);
    void addId(const QStringList &idList);
    void setIdParent(const QString &idParent);
    void setIdCountry(const QString &idCountry);    
    void setLimit(const int &limit);
    void setOffset(const int &offset);
    void setSortBy(const QString &sortBy);
    void setAscendingOrder(const bool &isAscending);
    void setSearchText(const QString &searchText);    
    void setIdPayment(const QString &idPayment);
    void addSortIndex(const QString &id, const int &index);
    void setFilter(const QString &filter);
    void clearFilter();
    void addStringValue(const QString &key, const QString &value);
    void clearStringValues();

    const QString filter() const;
    const QString search() const;

    virtual QJsonObject getAsJsonObject() const;
    virtual QString getAsJsonString() const;
    virtual void clear();

private:
    QList<QString> mIds;
    QList<QString> mIdsGroups;
    QString mIdMain;
    QString mIdParent;
    int mLimit;
    int mOffset;
    bool mAscendingOrder;
    QString mSortBy;
    QString mSearchText;
    QString mLanguage;
    QString mIdPayment;
    QString mFilter;    
    QString mIdCountry;    
    QMap<QString, int> mSortPostions;
    QMap<QString, QString> mDynValues;

};

#endif // REQUEST_H
