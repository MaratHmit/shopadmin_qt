#include "dialogcomplaint.h"
#include "ui_dialogcomplaint.h"

DialogComplaint::DialogComplaint(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogComplaint)
{
    ui->setupUi(this);
}

DialogComplaint::~DialogComplaint()
{
    delete ui;
}

void DialogComplaint::setInfo(const QString &info)
{
    ui->label->setText(info);
}

void DialogComplaint::on_pushButton_clicked()
{
    reject();
}
