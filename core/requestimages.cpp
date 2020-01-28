#include "requestimages.h"

RequestImages::RequestImages()
{
    clear();
}

void RequestImages::setSectionImage(const QString &section)
{
    mSection = section;
}

void RequestImages::addFileName(const QString &fileName)
{
    mFiles.append(fileName);
}

QJsonObject RequestImages::getAsJsonObject() const
{
    QJsonObject obj = Request::getAsJsonObject();

    obj.insert("section", QJsonValue(mSection));

    QJsonArray items;
    for (int i = 0; i < mFiles.size(); i++)
        items.append(QJsonValue(mFiles.at(i)));
    obj.insert("files", QJsonValue(items));

    return obj;
}

void RequestImages::clear()
{
    mSection.clear();
    mFiles.clear();
}
