#ifndef FIELD_H
#define FIELD_H

#include <QString>

class Field
{
public:
    Field();
    Field(const QString name, const QString title, const int &align = 0, const int &role = Qt::DisplayRole);

    const QString name() const;
    const QString title() const;
    int align() const;    
    int role() const;

    void setTitle(const QString &title);

private:
    QString mName;
    QString mTitle;
    int mAlign;    
    int mRole;
};

#endif // FIELD_H
