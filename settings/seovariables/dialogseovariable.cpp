#include "dialogseovariable.h"
#include "ui_dialogseovariable.h"
#include "apiadapter.h"

#include <QMessageBox>

DialogSEOVariable::DialogSEOVariable(QWidget *parent, const DataItem *item) :
    QDialog(parent),
    ui(new Ui::DialogSEOVariable)
{
    ui->setupUi(this);
    mItem = new DataItem;
    if (item)
        mItem->copy(item);
    fillControls();
    initSignals();
}

DialogSEOVariable::~DialogSEOVariable()
{
    delete ui;
    delete mItem;
}

const DataItem *DialogSEOVariable::item() const
{
    return mItem;
}

void DialogSEOVariable::accept()
{
    if (ui->lineEditName->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Сохранение данных", "Необходимо заполнить поле \"наименование\"!");
        ui->lineEditName->setFocus();
        return;
    }
    if (ui->lineEditValue->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Сохранение данных", "Необходимо заполнить поле \"значение\"!");
        ui->lineEditValue->setFocus();
        return;
    }

    mItem->setName(ui->lineEditName->text().trimmed());
    mItem->setValue(ui->lineEditValue->text().trimmed());
    if (ApiAdapter::getApi()->saveSEODynVar(mItem))
        QDialog::accept();
}

void DialogSEOVariable::initSignals()
{
    connect(ui->pushButtonCancel, SIGNAL(clicked(bool)), SLOT(reject()));
    connect(ui->pushButtonOK, SIGNAL(clicked(bool)), SLOT(accept()));
}

void DialogSEOVariable::fillControls()
{
    if (mItem->getId().isEmpty())
        return;

    ui->lineEditName->setText(mItem->getName());
    ui->lineEditValue->setText(mItem->getValue());
}
