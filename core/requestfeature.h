#ifndef REQUESTFEATURE_H
#define REQUESTFEATURE_H

#include "request.h"

class RequestFeature : public Request
{
public:
    RequestFeature();

    void setIsList(const bool &isList);
    void setIdFeature(const QString &idFeature);
    void setIsSEO(const QString &id, const bool &isSEO);

    virtual QJsonObject getAsJsonObject() const;
    virtual void clear();

private:
    bool mIsList;
    QString mIdFeature;
    QMap<QString, bool> mSEOParams;

};

#endif // REQUESTFEATURE_H
