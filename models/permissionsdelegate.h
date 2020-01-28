#ifndef PERMISSIONSDELEGATE_H
#define PERMISSIONSDELEGATE_H

#include <QStyledItemDelegate>

class PermissionsDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit PermissionsDelegate(QObject *parent = 0);

    void paint(QPainter *, const QStyleOptionViewItem &, const QModelIndex &) const;


};

#endif // PERMISSIONSDELEGATE_H
