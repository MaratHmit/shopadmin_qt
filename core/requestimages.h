#ifndef REQUESTIMAGES_H
#define REQUESTIMAGES_H

#include "request.h"

class RequestImages : public Request
{
public:
    RequestImages();

    void setSectionImage(const QString &section);
    void addFileName(const QString &fileName);

    QJsonObject getAsJsonObject() const;
    void clear();

private:
    QString mSection;
    QList<QString> mFiles;

};

#endif // REQUESTIMAGES_H
