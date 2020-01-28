#include "listimagesdelegate.h"
#include "frameimagedescription.h"

#include <QCheckBox>
#include <QApplication>
#include <QPainter>
#include <QStylePainter>

ListImagesDelegate::ListImagesDelegate(NameTableModel *model) :
    QItemDelegate(model)
{
    mModel = model;
}

QWidget *ListImagesDelegate::createEditor(
        QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &index) const
{
    if (index.column() == 2) {        
        FrameImageDescription *widget = new FrameImageDescription(parent);
        widget->setImage(mModel->dataItem(index));
        connect(widget, &FrameImageDescription::modified, this, &ListImagesDelegate::modified);
        return widget;
    }
    return 0;
}

void ListImagesDelegate::setEditorData(
        QWidget *, const QModelIndex &) const
{

}

void ListImagesDelegate::setModelData(
        QWidget *, QAbstractItemModel *, const QModelIndex &) const
{

}

void ListImagesDelegate::paint(
        QPainter *painter, const QStyleOptionViewItem &option,
        const QModelIndex &index) const
{
    if (index.isValid() && index.column() == 0) {
              QItemDelegate::paint( painter, option, index);
              return;
          }

     if (index.isValid() && index.column() > 0) {
         QWidget *widget = dynamic_cast<QWidget *> (painter->device());
         if (widget) {
             if (index.column() == 1) {
                 if (option.state & QStyle::State_Selected)
                      painter->fillRect(option.rect, QBrush(QColor(0, 191, 255)));
                 else painter->fillRect(option.rect, QBrush(QColor(245, 245, 245)));
                 QStyleOptionButton *buttonOption = new QStyleOptionButton;
                 buttonOption->initFrom(widget);
                 QRect centeredRect = option.rect;
                 buttonOption->rect = centeredRect.adjusted(
                             centeredRect.width() / 2
                             - QApplication::style()->pixelMetric(QStyle::PM_IndicatorWidth) / 2, 0, 0, 0);
                 buttonOption->state = QStyle::State_Enabled | QStyle::State_On;
                 if (!mModel->dataItem(index)->getIsMain())
                    buttonOption->state = QStyle::State_Enabled | QStyle::State_Off;
                 QApplication::style()->drawControl(QStyle::CE_CheckBox, buttonOption, painter, widget);
             } else {
                QPoint point;
                QRect rect = option.rect;
                rect.setHeight(rect.height() / 2);
                if (option.state & QStyle::State_Selected)
                     painter->fillRect(rect, QBrush(QColor(0, 191, 255)));
                else painter->fillRect(rect, QBrush(QColor(245, 245, 245)));
                rect = option.rect;
                rect.setTop(option.rect.top() + rect.height() / 2);
                rect.setBottom(rect.bottom());
                if (option.state & QStyle::State_Selected)
                    painter->fillRect(rect, QBrush(QColor(0, 191, 255)));
                else painter->fillRect(rect, QBrush(QColor(250, 250, 210)));
                point.setX(option.rect.left() + 4);
                point.setY(option.rect.top() + 20);
                painter->drawText(point, "Файл фото:");
                point.setX(option.rect.left() + 80);
                painter->drawText(point, mModel->dataItem(index)->getImageFile());
                point.setX(option.rect.left() + 4);
                point.setY(option.rect.top() + 50);
                painter->drawText(point, "Текст к фото:");
                point.setX(option.rect.left() + 80);
                point.setY(option.rect.top() + 50);
                painter->drawText(point, mModel->dataItem(index)->getImageAlt());
             }
         }
     }
}
