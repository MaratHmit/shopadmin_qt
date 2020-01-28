#include "dialogcardrole.h"
#include "ui_dialogcardrole.h"
#include "seconfig.h"
#include "seconsts.h"
#include "apiadapter.h"

DialogCardRole::DialogCardRole(QWidget *parent) :
    DialogCard(parent),
    ui(new Ui::DialogCardRole)
{
    ui->setupUi(this);

}

DialogCardRole::~DialogCardRole()
{
    delete ui;
}

void DialogCardRole::initControls()
{
    if (item->getId().isEmpty())
        setWindowTitle("Новая роль");
    else setWindowTitle(QString("Редактирование роли: " + item->getName()));

    ui->lineEdit->clear();
    ui->lineEdit->setFocus();
}

void DialogCardRole::fillControls()
{
    if (item->getId().isEmpty())
        return;

    ui->lineEdit->setText(item->getName());
}

bool DialogCardRole::setData()
{
    if (ui->lineEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Сохранение роли", "Не заполнено обязателное поле: <Наименование>!");
        ui->lineEdit->setFocus();
        return false;
    }

    item->setName(ui->lineEdit->text().trimmed());

    return true;
}

bool DialogCardRole::save()
{
    return ApiAdapter::getApi()->save(SEConsts::API_PERMISSION_ROLES, item);
}
