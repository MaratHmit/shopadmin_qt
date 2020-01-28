#include "dialogdelivcondition.h"
#include "ui_dialogdelivcondition.h"

#include <QRegExp>
#include <QRegExpValidator>

DialogDelivCondition::DialogDelivCondition(QWidget *parent,
                                           DataDeliveryConditions *condition) :
    QDialog(parent),
    ui(new Ui::DialogDelivCondition)
{
    ui->setupUi(this);
    mCondition = condition;
    initControls();
}

DialogDelivCondition::~DialogDelivCondition()
{
    delete ui;
}

void DialogDelivCondition::showEvent(QShowEvent *)
{
    fillControls();
    initSignals();
}

void DialogDelivCondition::initSignals()
{
    connect(ui->pushButtonCancel, &QPushButton::clicked, this,
            &DialogDelivCondition::reject);
    connect(ui->pushButtonOK, &QPushButton::clicked, this,
            &DialogDelivCondition::accept);
    connect(ui->comboBoxTypeParam, &QComboBox::currentTextChanged, this,
            &DialogDelivCondition::onChangeData);
    connect(ui->comboBoxTypeOperation, &QComboBox::currentTextChanged, this,
            &DialogDelivCondition::onChangeData);
    connect(ui->comboBoxTypePrice, &QComboBox::currentTextChanged, this,
            &DialogDelivCondition::onChangeData);
    connect(ui->lineEditMaxValue, &QLineEdit::textEdited, this,
            &DialogDelivCondition::onChangeData);
    connect(ui->lineEditMinValue, &QLineEdit::textEdited, this,
            &DialogDelivCondition::onChangeData);
    connect(ui->lineEditPricePercent, &QLineEdit::textEdited, this,
            &DialogDelivCondition::onChangeData);
    connect(ui->spinBoxPriotity, SIGNAL(valueChanged(int)),
            SLOT(onChangeData()));
}

void DialogDelivCondition::initControls()
{
    setValidators();

    ui->comboBoxTypeParam->addItem("сумма", "sum");
    ui->comboBoxTypeParam->addItem("вес", "weight");
    ui->comboBoxTypeParam->addItem("объем", "volume");
    ui->comboBoxTypeParam->setCurrentIndex(0);

    ui->comboBoxTypeOperation->addItem("приравнивать", "=");
    ui->comboBoxTypeOperation->addItem("добавлять", "+");
    ui->comboBoxTypeOperation->addItem("вычитать", "-");
    ui->comboBoxTypeParam->setCurrentIndex(0);

    ui->comboBoxTypePrice->addItem("абсолютное значение", "a");
    ui->comboBoxTypePrice->addItem("процент от суммы заказа", "s");
    ui->comboBoxTypePrice->addItem("процент от цены доставки", "d");
    ui->comboBoxTypePrice->setCurrentIndex(0);
}

void DialogDelivCondition::fillControls()
{
    ui->lineEditMaxValue->setText(
                QString::number(mCondition->maxValue()));
    ui->lineEditMinValue->setText(
                QString::number(mCondition->minValue()));

    for (int i = 0; i < ui->comboBoxTypeParam->count(); i++)
        if (ui->comboBoxTypeParam->itemData(i).toString() ==
                mCondition->typeParam()) {
            ui->comboBoxTypeParam->setCurrentIndex(i);
            break;
        }
    for (int i = 0; i < ui->comboBoxTypeOperation->count(); i++)
        if (ui->comboBoxTypeOperation->itemData(i).toString() ==
                mCondition->operation()) {
            ui->comboBoxTypeOperation->setCurrentIndex(i);
            break;
        }
    for (int i = 0; i < ui->comboBoxTypePrice->count(); i++)
        if (ui->comboBoxTypePrice->itemData(i).toString() ==
                mCondition->typePrice()) {
            ui->comboBoxTypePrice->setCurrentIndex(i);
            break;
        }

    ui->spinBoxPriotity->setValue(mCondition->priority());
    ui->lineEditPricePercent->setText(QString::number(mCondition->price()));
    if (mCondition->typePrice() == "a")
        ui->labelPricePercent->setText("Цена");
    else ui->labelPricePercent->setText("Процент");
    ui->lineEditDescriptionPrice->setText(mCondition->titleDescriptionPrice());
}

void DialogDelivCondition::setValidators()
{
    QRegExp regExprFloat("\\d*\\.{1}\\d*$|^\\d*$");
    QRegExpValidator *regFloats = new QRegExpValidator(regExprFloat, this);
    ui->lineEditPricePercent->setValidator(regFloats);
    ui->lineEditMaxValue->setValidator(regFloats);
    ui->lineEditMinValue->setValidator(regFloats);
}

void DialogDelivCondition::onChangeData()
{
    ui->pushButtonOK->setEnabled(true);
    if (sender() == ui->comboBoxTypeParam)
        mCondition->setTypeParam(ui->comboBoxTypeParam->currentData().toString());
    if (sender() == ui->comboBoxTypeOperation)
        mCondition->setOperation(ui->comboBoxTypeOperation->currentData().toString());
    if (sender() == ui->comboBoxTypePrice) {
        mCondition->setTypePrice(ui->comboBoxTypePrice->currentData().toString());
        if (mCondition->typePrice() == "a")
            ui->labelPricePercent->setText("Цена");
        else ui->labelPricePercent->setText("Процент");
    }    
    if (sender() == ui->lineEditMaxValue)
        mCondition->setMaxValue(ui->lineEditMaxValue->text().toFloat());
    if (sender() == ui->lineEditMinValue)
        mCondition->setMinValue(ui->lineEditMinValue->text().toFloat());
    if (sender() == ui->lineEditPricePercent)
        mCondition->setPrice(ui->lineEditPricePercent->text().toFloat());
    if (sender() == ui->spinBoxPriotity)
        mCondition->setPriority(ui->spinBoxPriotity->value());
    ui->lineEditDescriptionPrice->setText(mCondition->titleDescriptionPrice());
}
