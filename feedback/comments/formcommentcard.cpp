#include "formcommentcard.h"
#include "ui_formcommentcard.h"
#include "apiadapter.h"
#include "seconfig.h"
#include "formproducts.h"
#include "userpermission.h"

#include <QPalette>
#include <QMessageBox>

FormCommentCard::FormCommentCard(QWidget *parent) :
    DialogCard(parent),
    ui(new Ui::FormCommentCard)
{
    ui->setupUi(this);        

    initVariables();
    initSignals();
}

FormCommentCard::~FormCommentCard()
{    
    delete ui;
}

void FormCommentCard::setLocalMode()
{
    DialogCard::setLocalMode();

    ui->lineEditProduct->hide();
    ui->toolButtonProduct->hide();
    ui->labelProduct->hide();    
}

void FormCommentCard::onSelectProduct()
{
    FormProducts *form = new FormProducts(this, false);
    if (form->exec() == QDialog::Accepted &&
            form->getSelectedProducts()->size()) {
        ui->lineEditProduct->setText(form->getSelectedProducts()->at(0)->getName());
        item->setProperty("idProduct", form->getSelectedProducts()->at(0)->getId());
        item->setProperty("nameProduct", form->getSelectedProducts()->at(0)->getName());
    }
}

void FormCommentCard::initControls()
{    
    ui->lineEditNameContact->clear();
    ui->lineEditEmailContact->clear();
    ui->dateEdit->setDate(QDate::currentDate());
    ui->textEditCommentary->clear();
    ui->textEditResponse->clear();
    ui->checkBoxIsActive->setChecked(false);
    ui->checkBoxIsShowing->setChecked(false);
    ui->lineEditProduct->clear();
}

void FormCommentCard::showEvent(QShowEvent *)
{
    initControls();
    fillControls();
}

void FormCommentCard::initVariables()
{
    isLocalMode = false;
    ui->checkBoxIsActive->hide();
    ui->checkBoxIsShowing->hide();
    QPalette pal;
    SEConfig::setPaletteReadOnly(&pal);
    ui->lineEditProduct->setPalette(pal);
    ui->lineEditProduct->setReadOnly(true);
    ui->toolButtonProduct->setEnabled(UserPermission::isRead(UserPermission::PRODUCTS));
}

void FormCommentCard::fillControls()
{
    if (item->getId().isEmpty())
        return;

    ui->lineEditNameContact->setText(item->property("contactTitle").toString());
    ui->lineEditEmailContact->setText(item->property("contactEmail").toString());
    ui->dateEdit->setDate(QDate::fromString(item->property("date").toString(), "dd.MM.yyyy"));
    ui->textEditCommentary->setText(item->property("commentary").toString());
    ui->textEditResponse->setText(item->property("response").toString());
    ui->lineEditProduct->setText(item->property("nameProduct").toString());
}

bool FormCommentCard::save()
{    
    return isLocalMode || ApiAdapter::getApi()->saveCommentary(item);
}

bool FormCommentCard::setData()
{
    if (ui->lineEditNameContact->text().isEmpty() ||
            ui->textEditCommentary->document()->isEmpty() ||
            (item->property("idProduct").toString().isEmpty() && !isLocalMode)) {
        QMessageBox::warning(this, tr("Сохранение комментария"),
                             tr("Поля отмеченные [*] обязательны к заполнению!"));
        return false;
    }

    item->setProperty("contactTitle", ui->lineEditNameContact->text().trimmed());
    item->setProperty("contactEmail", ui->lineEditEmailContact->text().trimmed());
    item->setProperty("date", ui->dateEdit->date());
    item->setProperty("commentary", ui->textEditCommentary->document()->toPlainText());
    item->setProperty("response", ui->textEditResponse->document()->toPlainText());

    return true;
}

void FormCommentCard::initSignals()
{
    connect(ui->toolButtonProduct, SIGNAL(clicked()), SLOT(onSelectProduct()));
    connect(ui->pushButtonCancel, SIGNAL(clicked(bool)), SLOT(reject()));
    connect(ui->pushButtonOK, SIGNAL(clicked(bool)), SLOT(accept()));
}
