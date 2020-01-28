#include "dialoghostname.h"
#include "ui_dialoghostname.h"

DialogHostName::DialogHostName(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogHostName)
{
    ui->setupUi(this);
    connect(ui->pushButtonOK, SIGNAL(clicked()), SLOT(accept()));
    connect(ui->pushButtonCancel, SIGNAL(clicked()), SLOT(reject()));
}

DialogHostName::~DialogHostName()
{
    delete ui;
}

void DialogHostName::setHostName(const QString &hostName)
{
    ui->lineEditName->setText(hostName);
}

QString DialogHostName::hostName() const
{
    return ui->lineEditName->text().trimmed();
}
