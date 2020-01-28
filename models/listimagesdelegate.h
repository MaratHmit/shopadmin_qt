#ifndef LISTIMAGESDELEGATE_H
#define LISTIMAGESDELEGATE_H

#include "nametablemodel.h"

#include <QItemDelegate>

class ListImagesDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit ListImagesDelegate(NameTableModel *model);

    QWidget *createEditor(QWidget *parent,
                          const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;    

signals:
    void modified();

private:    
    NameTableModel *mModel;
    int mImageSection;

};

#endif // LISTIMAGESDELEGATE_H
