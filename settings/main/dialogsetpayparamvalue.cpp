#include "dialogsetpayparamvalue.h"
#include "ui_dialogsetpayparamvalue.h"
#include "apiadapter.h"

DialogSetPayParamValue::DialogSetPayParamValue(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSetPayParamValue)
{
    ui->setupUi(this);
    mParam = new DataItem();
}

DialogSetPayParamValue::~DialogSetPayParamValue()
{
    delete mParam;
    delete ui;
}

void DialogSetPayParamValue::setParam(DataItem *param)
{
    mParam->copy(param);    
    ui->labelName->setText(mParam->getName());
    ui->lineEditValue->setText(mParam->getValue());
}

QString DialogSetPayParamValue::value() const
{
    return ui->lineEditValue->text();
}

void DialogSetPayParamValue::on_buttonBox_accepted()
{
    if (ui->lineEditValue->text().trimmed().isEmpty()) {
        reject();
        return;
    }

    setData();
    //mParam->setIsSetAllFields(1);
    if (ApiAdapter::getApi()->saveParamRequisites(mParam))
        accept();
}

void DialogSetPayParamValue::setData()
{
    mParam->setValue(ui->lineEditValue->text());
}
