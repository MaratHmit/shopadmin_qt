#include "field.h"

#include <qnamespace.h>

Field::Field()
{

}

Field::Field(const QString name, const QString title, const int &align, const int &role) :
    mName(name), mTitle(title), mAlign(align), mRole(role)
{
    if (!mAlign)
        mAlign = Qt::AlignLeft | Qt::AlignVCenter;
}

const QString Field::name() const
{
    return mName;
}

const QString Field::title() const
{
    return mTitle;
}

int Field::align() const
{
    return mAlign;
}

int Field::role() const
{
    return mRole;
}

void Field::setTitle(const QString &title)
{
    mTitle = title;
}

