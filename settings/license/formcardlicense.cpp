#include "formcardlicense.h"
#include "ui_formcardlicense.h"
#include "apiadapter.h"
#include "seconfig.h"

FormCardLicense::FormCardLicense(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormCardLicense)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window);
    setAttribute(Qt::WA_DeleteOnClose);

    mLicense = new DataLicense;
}

FormCardLicense::~FormCardLicense()
{
    delete ui;
    delete mLicense;
}


bool FormCardLicense::saveContact()
{    
    return ApiAdapter::getApi()->saveLicense(mLicense);
}

void FormCardLicense::setIdLicense(const QString &idLicense)
{
    mLicense->setId(idLicense);
}

QString FormCardLicense::idLicense()const
{
    return mLicense->getId();
}

void FormCardLicense::fillControls()
{
    if (idLicense()== 0)
        return;

    if (!ApiAdapter::getApi()->getInfoLicense(mLicense))
        return;

    ui->lineEditSerial->setText(mLicense->getSerial());
    ui->textEditRegKey->setText(mLicense->getRegkey());

}

void FormCardLicense::getParam()
{
    QDate dat = QDate::currentDate();

    mLicense->setSerial(ui->lineEditSerial->text());
    mLicense->setRegkey(ui->textEditRegKey->toPlainText());
    mLicense->setDataReg(dat);
}

void FormCardLicense::showEvent(QShowEvent *event)
{
      Q_UNUSED(event);
      fillControls();
}

void FormCardLicense::on_pushButtonCancel_clicked()
{
    reject();
}

void FormCardLicense::on_pushButtonOk_clicked()
{
    if (ui->lineEditSerial->text().trimmed().isEmpty())
        return;

    getParam();
    if (saveContact())
        accept();

}
