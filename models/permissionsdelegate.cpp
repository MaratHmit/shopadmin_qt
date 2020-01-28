#include "permissionsdelegate.h"

#include <QStyleOptionViewItem>

PermissionsDelegate::PermissionsDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{

}

void PermissionsDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                const QModelIndex &index) const
{

    if (!index.column()) {
        QStyledItemDelegate::paint( painter, option, index);
        return;
    }

    QStyleOptionViewItem optionNew(option);
    optionNew.decorationAlignment = Qt::AlignCenter;
    optionNew.decorationPosition = QStyleOptionViewItem::Top;    
    QStyledItemDelegate::paint(painter, optionNew, index);
}
