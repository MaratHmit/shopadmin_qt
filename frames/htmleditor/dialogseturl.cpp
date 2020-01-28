#include "dialogseturl.h"
#include "ui_dialogseturl.h"

DialogSetURL::DialogSetURL(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSetURL)
{
    ui->setupUi(this);    

    mIsHasName = false;
    connect(ui->lineEditName, SIGNAL(textEdited(QString)),
            SLOT(onNameEdited(QString)));
    connect(ui->lineEditURL, SIGNAL(textEdited(QString)),
            SLOT(onURLEdited(QString)));
}

DialogSetURL::~DialogSetURL()
{
    delete ui;
}

void DialogSetURL::setURLName(const QString &str)
{
    ui->lineEditName->setText(str);
    mIsHasName = true;
}

QString DialogSetURL::URL() const
{
    QString s = "<a href='" + ui->lineEditURL->text() + "'>" + ui->lineEditName->text();
    if (ui->checkBoxIsOpenInNewWindow->isChecked())
      s += "target=\"_blank\"</a>";
    else s += "</a>";

    return s;
}

void DialogSetURL::onNameEdited(const QString &arg1)
{
    mIsHasName = !arg1.isEmpty();
}

void DialogSetURL::onURLEdited(const QString &arg1)
{
    if (!mIsHasName)
        ui->lineEditName->setText(arg1);
}
