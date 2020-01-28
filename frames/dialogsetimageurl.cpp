#include "dialogsetimageurl.h"
#include "ui_dialogsetimageurl.h"

DialogSetImageUrl::DialogSetImageUrl(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSetImageUrl)
{
    ui->setupUi(this);
    connect(ui->pushButtonCancel, &QAbstractButton::clicked, this, &DialogSetImageUrl::reject);
    connect(ui->pushButtonOK, &QAbstractButton::clicked, this, &DialogSetImageUrl::accept);
    connect(ui->lineEdit, &QLineEdit::textEdited, this, &DialogSetImageUrl::checkEnabled);
}

DialogSetImageUrl::~DialogSetImageUrl()
{
    delete ui;
}

const QString DialogSetImageUrl::imageURL() const
{
    return ui->lineEdit->text();
}

void DialogSetImageUrl::checkEnabled()
{
    ui->pushButtonOK->setEnabled(ui->lineEdit->text().indexOf("http") == 0);
}
