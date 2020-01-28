#include "dialogauthcheck.h"
#include "ui_dialogauthcheck.h"

#include "apiadapter.h"
#include "seutils.h"

#include <QMessageBox>

DialogAuthCheck::DialogAuthCheck(QWidget *parent, const QString &project, const QString &serial) :
    QDialog(parent),
    ui(new Ui::DialogAuthCheck)
{
    ui->setupUi(this);
    ui->lineEditProjectName->setText(project);
    ui->lineEditLogin->setText(serial);

}

DialogAuthCheck::~DialogAuthCheck()
{
    delete ui;
}

void DialogAuthCheck::accept()
{
    if (ui->lineEditLogin->text().trimmed().isEmpty()) {
        QMessageBox::critical(this, "Авторизация", "Заполните поле: [Серийный номер]");
        return;
    }
    if (ui->lineEditPassword->text().isEmpty()) {
        QMessageBox::critical(this, "Авторизация", "Заполните поле: [Серийный ключ]");
        return;
    }

    if (ApiAdapter::getApi()->registration(ui->lineEditLogin->text().trimmed().toLower(),
                                           QMD5(ui->lineEditPassword->text().trimmed()), QString(), true))
        QDialog::accept();
}
