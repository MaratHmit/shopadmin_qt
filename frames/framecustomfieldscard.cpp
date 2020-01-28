#include "framecustomfieldscard.h"
#include "ui_framecustomfieldscard.h"
#include "request.h"
#include "apiadapter.h"

#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QRadioButton>
#include <QDateEdit>
#include <QVBoxLayout>


FrameCustomFieldsCard::FrameCustomFieldsCard(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FrameCustomFieldsCard)
{
    ui->setupUi(this);
    initVariables();
}

FrameCustomFieldsCard::~FrameCustomFieldsCard()
{
    delete ui;
}

void FrameCustomFieldsCard::setItem(const DataImages *item)
{
    this->item = item;
}

void FrameCustomFieldsCard::setDataType(const QString &dataType)
{
    this->dataType = dataType;
}

int FrameCustomFieldsCard::countFields() const
{
    return mListDynFields->size();
}

void FrameCustomFieldsCard::onChangeDynFields()
{
    QString id = sender()->property("id").toString();
    if (id.isEmpty())
        return;  

    for (int i = 0; i < item->listCustomFields()->size(); ++i) {        
        if (item->listCustomFields()->at(i)->getIdMain() == id) {
            const DataItem *dynField = getDynFieldById(id);
            QString value = item->listCustomFields()->at(i)->getValue();
            if (QString(sender()->metaObject()->className()) == "QLineEdit")
                value = qobject_cast <QLineEdit *> (sender())->text();
            if (QString(sender()->metaObject()->className()) == "QPlainTextEdit")
                value = qobject_cast <QPlainTextEdit *> (sender())->toPlainText();
            if (QString(sender()->metaObject()->className()) == "QDoubleSpinBox")
                value = QString::number(qobject_cast <QDoubleSpinBox *> (sender())->value(), 10, 2);
            if (QString(sender()->metaObject()->className()) == "QDateEdit")
                value = qobject_cast <QDateEdit *> (sender())->date().toString("yyyy-MM-dd");
            if (QString(sender()->metaObject()->className()) == "QComboBox")
                value = qobject_cast <QComboBox *> (sender())->currentText();
            if (QString(sender()->metaObject()->className()) == "QCheckBox") {
                QCheckBox *box = qobject_cast <QCheckBox *> (sender());
                if (dynField->getValues().size()) {
                    QString valueBox = box->text();
                    if (box->isChecked()) {
                        if (value.isEmpty())
                            value = valueBox;
                        else if (!value.contains(valueBox))
                            value += "," + valueBox;
                    } else {
                        if (value.contains(valueBox)) {
                            QStringList listValues = value.split(",");
                            listValues.removeAll(valueBox);
                            value = listValues.join(",");
                        }
                    }
                } else {
                    if (box->isChecked())
                        value = "1";
                    else value = "0";
                }
            }
            if (QString(sender()->metaObject()->className()) == "QRadioButton") {
                QRadioButton *box = qobject_cast <QRadioButton *> (sender());
                QString valueBox = box->text();
                if (box->isChecked())
                    value = valueBox;
            }
            item->listCustomFields()->at(i)->setValue(value);            
            break;
        }
    }

    emit modified();
}

const DataItem *FrameCustomFieldsCard::getDynFieldById(const QString &id) const
{
    for (int i = 0; i < mListDynFields->size(); ++i)
        if (mListDynFields->at(i)->getId() == id)
            return mListDynFields->at(i);
    return 0;
}

void FrameCustomFieldsCard::showEvent(QShowEvent *)
{
    if (isShowed)
        return;

    initDynFields();
    isShowed = true;
}

void FrameCustomFieldsCard::addDynFieldGroup(const DataItem *fieldGroup)
{
    QVBoxLayout *layoutDynFieldGroups = qobject_cast<QVBoxLayout *> (this->layout());
    if (!layoutDynFieldGroups) {
        layoutDynFieldGroups = new QVBoxLayout();
        this->setLayout(layoutDynFieldGroups);
    }
    QGroupBox *box = new QGroupBox(this);
    box->setTitle(fieldGroup->getName());
    box->setProperty("id", fieldGroup->getId());
    QGridLayout *layoutGrid = new QGridLayout();
    box->setLayout(layoutGrid);
    layoutDynFieldGroups->addWidget(box);
}

void FrameCustomFieldsCard::addDynField(const DataItem *field)
{
    QGroupBox *box = 0;
    for (int i = 0; i < this->children().size(); ++i) {
        box = qobject_cast <QGroupBox *> (this->children().at(i));
        if (box && box->property("id").toString() == field->getIdGroup())
            break;
    }
    if (box) {
        QGridLayout *layout = qobject_cast <QGridLayout *> (box->layout());
        if (!layout)
            return;
        int row = layout->rowCount();
        QLabel *label = new QLabel(field->getName() + ":  ");
        layout->addWidget(label, row, 0, Qt::AlignRight);
        label->setAlignment(Qt::AlignRight);
        label->setMaximumWidth(200);
        label->setMinimumWidth(200);
        if (field->getValueType() == "S") {
            QLineEdit *edit = new QLineEdit();
            edit->setText(field->getValue());
            edit->setPlaceholderText(field->property("placeholder").toString());
            edit->setProperty("id", field->getId());
            connect(edit, SIGNAL(textEdited(QString)), SLOT(onChangeDynFields()));
            layout->addWidget(edit, row, 1);
            return;
        }
        if (field->getValueType() == "T") {
            QPlainTextEdit *edit = new QPlainTextEdit();
            edit->setPlainText(field->getValue());
            edit->setProperty("id", field->getId());
            edit->setPlaceholderText(field->property("placeholder").toString());
            connect(edit, SIGNAL(textChanged()), SLOT(onChangeDynFields()));
            layout->addWidget(edit, row, 1);
            return;
        }
        if (field->getValueType() == "D") {
            QDoubleSpinBox *edit = new QDoubleSpinBox();
            edit->setToolTip(field->property("placeholder").toString());
            edit->setMaximum(16777215);
            edit->setMinimum(-16777214);
            edit->setProperty("id", field->getId());
            edit->setValue(field->getValue().toDouble());
            connect(edit, SIGNAL(valueChanged(double)), SLOT(onChangeDynFields()));
            layout->addWidget(edit, row, 1);
        }
        if (field->getValueType() == "L") {
            QComboBox *edit = new QComboBox();
            edit->setToolTip(field->property("placeholder").toString());
            for (int i = 0; i < field->getValues().size(); i++)
                edit->addItem(field->getValues().at(i));
            edit->setProperty("id", field->getId());
            edit->setCurrentIndex(-1);
            edit->setCurrentText(field->getValue());
            connect(edit, SIGNAL(currentIndexChanged(int)), SLOT(onChangeDynFields()));
            layout->addWidget(edit, row, 1);
        }
        if (field->getValueType() == "DT") {
            QDateEdit *edit = new QDateEdit();
            edit->setCalendarPopup(true);
            edit->setToolTip(field->property("placeholder").toString());
            edit->setProperty("id", field->getId());
            edit->setDate(QDate::fromString("01-01-2000", "yyyy-MM-dd"));
            edit->setDate(QDate::fromString(field->getValue(), "yyyy-MM-dd"));
            connect(edit, SIGNAL(dateChanged(QDate)), SLOT(onChangeDynFields()));
            layout->addWidget(edit, row, 1);
        }
        if (field->getValueType() == "R") {
            QWidget *widget = new QWidget();
            widget->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
            widget->setToolTip(field->property("placeholder").toString());
            QVBoxLayout *layoutW = new QVBoxLayout();
            for (int i = 0; i < field->getValues().size(); i++) {
                QRadioButton *edit = new QRadioButton();
                edit->setText(field->getValues().at(i));
                edit->setChecked(field->getValues().at(i) == field->getValue());
                edit->setProperty("id", field->getId());
                connect(edit, SIGNAL(toggled(bool)), SLOT(onChangeDynFields()));
                layoutW->addWidget(edit);
            }
            widget->setLayout(layoutW);
            layout->addWidget(widget, row, 1);
        }
        if (field->getValueType() == "CB") {
            if (field->getValues().size()) {
                QWidget *widget = new QWidget();
                widget->setToolTip(field->property("placeholder").toString());
                QVBoxLayout *layoutW = new QVBoxLayout();
                for (int i = 0; i < field->getValues().size(); i++) {
                    QCheckBox *edit = new QCheckBox();
                    edit->setChecked(field->getValue().contains(field->getValues().at(i)));
                    edit->setText(field->getValues().at(i));
                    edit->setProperty("id", field->getId());
                    connect(edit, SIGNAL(clicked(bool)), SLOT(onChangeDynFields()));
                    layoutW->addWidget(edit);
                }
                widget->setLayout(layoutW);
                layout->addWidget(widget, row, 1);
            } else {
                QCheckBox *edit = new QCheckBox();
                edit->setToolTip(field->property("placeholder").toString());
                edit->setProperty("id", field->getId());
                connect(edit, SIGNAL(clicked(bool)), SLOT(onChangeDynFields()));
                if (!field->getValue().isEmpty() && field->getValue() != "0")
                    edit->setChecked(true);
                layout->addWidget(edit, row, 1);
            }
        }
    }
}

void FrameCustomFieldsCard::createDynFields()
{
    for (int i = 0; i < mListDynFields->size(); ++i)
        addDynField(mListDynFields->at(i));
    QGroupBox *box = 0;
    for (int i = 0; i < this->children().size(); ++i) {
        box = qobject_cast <QGroupBox *> (this->children().at(i));
        if (box) {
            QGridLayout *layout = qobject_cast <QGridLayout *> (box->layout());
            if (layout) {
                QSpacerItem *itemSpacer = new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding);
                layout->addItem(itemSpacer, layout->rowCount(), 0);
            }
        }
    }
}

void FrameCustomFieldsCard::createDynFieldsGroups()
{
    for (int i = 0; i < mListDynFields->size(); ++i)
        if (mListDynFields->at(i)->getIdGroup().isEmpty()) {
            addDynFieldGroup(new DataItem());
            break;
        }
    for (int i = 0; i < mListDynFieldGroups->size(); ++i) {
        for (int j = 0; j < mListDynFields->size(); ++j)
            if (mListDynFields->at(j)->getIdGroup() == mListDynFieldGroups->at(i)->getId()) {
                addDynFieldGroup(mListDynFieldGroups->at(i));
                break;
            }
    }
}

void FrameCustomFieldsCard::initVariables()
{
    item = nullptr;
    isShowed = false;
    mListDynFields = new ListDataItems();
    mListDynFieldGroups = new ListDataItems();
    dataType = "order";
}

void FrameCustomFieldsCard::initDynFields()
{
    if (item == nullptr)
        return;

    Request request;
    request.setFilter("[data] = '" + dataType + "'");
    setCursor(Qt::WaitCursor);
    if (ApiAdapter::getApi()->getListDynFieldsGroups(request, mListDynFieldGroups) &&
            ApiAdapter::getApi()->getListDynFields(request, mListDynFields)) {
        for (int i = 0; i < item->listCustomFields()->size(); ++i) {
            for (int j = 0; j < mListDynFields->size(); ++j) {
                if (item->listCustomFields()->at(i)->getIdMain() == mListDynFields->at(j)->getId()) {
                    mListDynFields->at(j)->setValue(item->listCustomFields()->at(i)->getValue());
                    break;
                }
            }
        }
        createDynFieldsGroups();
        createDynFields();
    }
    setCursor(Qt::ArrowCursor);
}
