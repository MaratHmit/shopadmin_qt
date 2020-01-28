#include "integrationsdelegate.h"

#include <QCheckBox>
#include <QComboBox>
#include <QApplication>
#include <QPainter>
#include <QStylePainter>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QHBoxLayout>
#include <QSpacerItem>

#include "requestfeature.h"
#include "apiadapter.h"

IntegrationsDelegate::IntegrationsDelegate(QObject *parent, const IntegrationsModel *model) :
    QItemDelegate(parent)
{
    mIsParametersOnOffMode = false;
    mIsFeaturesMode = false;
    mModel = model;
    connect(this, SIGNAL(modified()), mModel, SIGNAL(modified()));
}

IntegrationsDelegate::~IntegrationsDelegate()
{

}

void IntegrationsDelegate::setIsParametersOnOffMode()
{
    mIsParametersOnOffMode = true;
}

void IntegrationsDelegate::setIsFeaturesMode()
{
    mIsFeaturesMode = true;
}

QWidget *IntegrationsDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                            const QModelIndex &index) const
{
    QWidget *widget = QItemDelegate::createEditor(parent, option, index);
    if (!index.isValid())
        return widget;

    DataItem *item = mModel->dataItem(index);
    int indexColumn = index.column();
    if (mIsParametersOnOffMode) {
        switch (indexColumn) {
        case 1:
            indexColumn = 2;
            break;
        case 2:
            indexColumn = 1;
            break;
        default:
            break;
        }
    }

    if (indexColumn == 1) {        
        if (!item->getValues().size() && (item->getValueType() == "L" || item->getValueType() == "CL")) {
            parent->setCursor(Qt::WaitCursor);
            ListFeatures *features = new ListFeatures;
            RequestFeature request;
            request.setIdFeature(qobject_cast <DataFeature *> (item)->idFeature());            
            if (ApiAdapter::getApi()->getListFeaturesValues(request, features)) {
                for (int i = 0; i < features->size(); ++i)
                    item->addInListValues(features->at(i)->getName());
            }
            parent->setCursor(Qt::ArrowCursor);
        }

        if (item && item->getValueType() == "B") {
            widget = new QWidget(parent);
            widget->setMaximumWidth(50);
            QCheckBox *checkBox = new QCheckBox(widget);
            checkBox->setMaximumWidth(50);
            QHBoxLayout *layout = new QHBoxLayout(widget);
            layout->addWidget(checkBox);
            layout->setContentsMargins(5, 0, 0, 0);
            widget->setLayout(layout);
            checkBox->setProperty("item", (qintptr) item);
            connect(checkBox, SIGNAL(clicked(bool)), this, SLOT(onChecked(bool)));            

            return widget;
        }

        if (item && (item->getValueType() == "L" || item->getValueType() == "CL")) {
            QComboBox *widget = new QComboBox(parent);
            if (item->getValues().size())
                widget->addItems(item->getValues());
            widget->setProperty("item", (qintptr) item);
            widget->setEditable(mIsFeaturesMode);
            if (!mIsFeaturesMode)
                widget->setMaximumWidth(200);
            return widget;
        }

        if (QDoubleSpinBox *box = qobject_cast <QDoubleSpinBox *> (widget))
            box->setMaximumWidth(70);
        if (QSpinBox *box = qobject_cast <QSpinBox *> (widget))
            box->setMaximumWidth(70);
    }

    return widget;
}

void IntegrationsDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    DataItem *item = mModel->dataItem(index);
    int indexColumn = index.column();
    if (mIsParametersOnOffMode) {
        switch (indexColumn) {
        case 1:
            indexColumn = 2;
            break;
        case 2:
            indexColumn = 1;
            break;
        default:
            break;
        }
    }

    if (indexColumn == 1) {
        if (item && item->getValueType() == "B") {
            if (editor->children().count()) {
                if (QCheckBox *box = qobject_cast <QCheckBox *> (editor->children()[0])) {
                    if (item->getValue().isEmpty())
                        item->setValue("1");
                    else item->setValue(QString());                    
                    box->setChecked(!item->getValue().isEmpty());
                }
                return;
            }
        }
        if (item && (item->getValueType() == "L" || item->getValueType() == "CL")) {
            QComboBox *box = qobject_cast <QComboBox *> (editor);
            box->setCurrentText(item->getValue());
            connect(box, SIGNAL(currentTextChanged(QString)), SLOT(onChangeComboBox(QString)));
            return;
        }        
    }

    QItemDelegate::setEditorData(editor, index);
}

void IntegrationsDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                        const QModelIndex &index) const
{
    DataItem *item = mModel->dataItem(index);
    int indexColumn = index.column();
    if (mIsParametersOnOffMode) {
        switch (indexColumn) {
        case 1:
            indexColumn = 2;
            break;
        case 2:
            indexColumn = 1;
            break;
        default:
            break;
        }
    }

     if (indexColumn == 1) {
        if (item && item->getValueType() == "B") {
            if (editor->children().count()) {
                if (QCheckBox *box = qobject_cast <QCheckBox *> (editor->children()[0]))
                    model->setData(index, box->isChecked(), Qt::EditRole);                
                return;
            }
        }
        if (item && (item->getValueType() == "CL" || item->getValueType() == "L") && mIsFeaturesMode) {
            return;
        }
     }

    QItemDelegate::setModelData(editor, model, index);
}

void IntegrationsDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                 const QModelIndex &index) const
{
    if (!index.isValid())
        return;

    DataItem *item = mModel->dataItem(index);
    if (!item || option.state & QStyle::State_Editing) {
        QItemDelegate::paint( painter, option, index);
        return;
    }

    int indexColumn = index.column();
    if (mIsParametersOnOffMode) {
        switch (indexColumn) {
        case 1:
            indexColumn = 2;
            break;
        case 2:
            indexColumn = 1;
            break;
        default:
            break;
        }
    }

    if (item->getValueType() == "B" && indexColumn == 1) {
        QWidget *widget = dynamic_cast<QWidget *> (painter->device());
        if (widget) {
            painter->fillRect(option.rect, QBrush(QColor(Qt::white)));
            QStyleOptionButton *buttonOption = new QStyleOptionButton;
            buttonOption->initFrom(widget);
            QRect centeredRect = option.rect;
            buttonOption->rect = centeredRect.adjusted(5, 0, 0, 0);
            buttonOption->state = QStyle::State_Enabled | QStyle::State_On;
            if (item->getValue().isEmpty() || ! (bool) item->getValue().toInt())
                buttonOption->state = QStyle::State_Enabled | QStyle::State_Off;
            QApplication::style()->drawControl(QStyle::CE_CheckBox, buttonOption, painter, widget);
            return;
        }
    }

    if (indexColumn == 2) {
        QWidget *widget = dynamic_cast<QWidget *> (painter->device());
        if (widget) {
            painter->fillRect(option.rect, QBrush(QColor(Qt::white)));
            QStyleOptionButton *buttonOption = new QStyleOptionButton;
            buttonOption->initFrom(widget);
            QRect centeredRect = option.rect;
            buttonOption->rect = centeredRect.adjusted(
                        centeredRect.width() / 2
                        - QApplication::style()->pixelMetric(QStyle::PM_IndicatorWidth) / 2, 0, 0, 0);
            buttonOption->state = QStyle::State_Enabled | QStyle::State_On;
            if (!item->getIsActive())
                buttonOption->state = QStyle::State_Enabled | QStyle::State_Off;
            QApplication::style()->drawControl(QStyle::CE_CheckBox, buttonOption, painter, widget);
            return;
        }
    }

    QItemDelegate::paint( painter, option, index);
}

void IntegrationsDelegate::onChecked(bool checked)
{
    if (QCheckBox *box = qobject_cast <QCheckBox *> (sender())) {
        DataItem *item = reinterpret_cast <DataItem *> (box->property("item").toInt());
        if (item) {
            if (checked)
                item->setValue("1");
            else item->setValue(QString());
            emit modified();
        }
    }
}

void IntegrationsDelegate::onChangeComboBox(const QString &value)
{
    if (QComboBox *box = qobject_cast <QComboBox *> (sender())) {        
        DataItem *item = reinterpret_cast <DataItem *> (box->property("item").toInt());
        if (item) {
            item->setValue(value);
            emit modified();
        }
        if (item && mIsFeaturesMode)
            item->setIdValue(QString());
    }
}
