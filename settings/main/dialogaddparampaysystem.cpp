#include "dialogaddparampaysystem.h"
#include "ui_dialogaddparampaysystem.h"
#include "apiadapter.h"

DialogAddParamPaySystem::DialogAddParamPaySystem(QWidget *parent, const bool &isSave) :
    QDialog(parent),
    ui(new Ui::DialogAddParamPaySystem)
{
    ui->setupUi(this);
    mParam = new DataPayRequisite();
    mIsSave = isSave;
    setValidators();
}

DialogAddParamPaySystem::~DialogAddParamPaySystem()
{
    delete mParam;
    delete ui;
}

QString DialogAddParamPaySystem::code() const
{
    return ui->lineEditCode->text();
}

QString DialogAddParamPaySystem::name() const
{
    return ui->lineEditName->text();
}

void DialogAddParamPaySystem::setIdPayment(const QString &idPayment)
{
    mParam->setIdPayment(idPayment);
}

void DialogAddParamPaySystem::setPayReq(const DataPayRequisite *payReq)
{
    ui->lineEditName->setEnabled(0);
    ui->lineEditCode->setEnabled(0);
    mParam->copy(payReq);
}

const DataPayRequisite *DialogAddParamPaySystem::payReq() const
{    
    return mParam;
}

void DialogAddParamPaySystem::setValidators()
{
    QRegExp regExpr("[A-я0-9-]*");    
    QRegExpValidator *regLatinText = new QRegExpValidator(regExpr, this);
    ui->lineEditCode->setValidator(regLatinText);
}

void DialogAddParamPaySystem::fillControls()
{
    if (mParam->getId().isEmpty())
        return;

    ui->lineEditCode->setText(mParam->getCode());
    ui->lineEditName->setText(mParam->getName());
    ui->lineEditValue->setText(mParam->getValue());
}

void DialogAddParamPaySystem::on_buttonBox_accepted()
{
    if (!ui->lineEditCode->text().isEmpty() && !ui->lineEditName->text().isEmpty()) {
        setData();
        if (!mIsSave || ApiAdapter::getApi()->saveParamRequisites(mParam))
                accept();
    }
}

void DialogAddParamPaySystem::showEvent(QShowEvent *)
{
    fillControls();
}

void DialogAddParamPaySystem::setData()
{
    mParam->setCode(ui->lineEditCode->text().toUpper());
    mParam->setName(ui->lineEditName->text());
    mParam->setValue(ui->lineEditValue->text());
}
