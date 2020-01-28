#include "formreviewcard.h"
#include "ui_formreviewcard.h"
#include "formproducts.h"
#include "formcontacts.h"
#include "apiadapter.h"
#include "userpermission.h"

FormReviewCard::FormReviewCard(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormReviewCard)
{
    ui->setupUi(this);
    initSignals();
    initVariables();
    initPermissions();
}

FormReviewCard::~FormReviewCard()
{
    delete ui;
    delete mReview;
}

void FormReviewCard::setReview(const DataReview *review)
{
    mReview->copy(review);    
    ui->dateTimeEdit->setDateTime(review->dateTime());
    ui->comboBoxMark->setCurrentIndex(review->mark() - 1);
    ui->comboBoxUseTime->setCurrentIndex(review->useTime() - 1);
    ui->lineEditProduct->setText(review->nameProduct());
    ui->lineEditUser->setText(review->nameUser());
    ui->plainTextEditMerits->setPlainText(review->merits());
    ui->plainTextEditDemerits->setPlainText(review->demerits());
    ui->plainTextEditComment->setPlainText(review->comment());
    ui->checkBoxIsActive->setChecked(review->getIsActive());
    ui->spinBoxLikes->setValue(review->countLikes());
    ui->spinBoxDislikes->setValue(review->countDislikes());
    ui->labelMark->setPixmap(mReview->markIco());
}

void FormReviewCard::onSave()
{
    if (!mReview->isModified()) {
        reject();
        return;
    }

    if (!mReview->mark()) {
        QMessageBox::warning(this, tr("Сохранение отзыва"),
                             tr("Не заполнено обязателное поле: <Оценка>!"));
        ui->comboBoxMark->setFocus();
        return;
    }
    if (mReview->idProduct().isEmpty()) {
        QMessageBox::warning(this, tr("Сохранение отзыва"),
                             tr("Не заполнено обязателное поле: <Товар>!"));
        ui->lineEditProduct->setFocus();
        return;
    }
    if (mReview->idUser().isEmpty()) {
        QMessageBox::warning(this, tr("Сохранение отзыва"),
                             tr("Не заполнено обязателное поле: <Покупатель>!"));
        ui->lineEditUser->setFocus();
        return;
    }

    if (save())
        accept();
}

void FormReviewCard::onChangeMark(int index)
{
    mReview->setMark(index + 1);
    ui->labelMark->setPixmap(mReview->markIco());
}

void FormReviewCard::onChangeUseTime(int index)
{
    mReview->setUseTime(index + 1);
}

void FormReviewCard::onChangePlainText()
{
    if (sender() == ui->plainTextEditMerits)
        mReview->setMerits(ui->plainTextEditMerits->document()->toPlainText());
    if (sender() == ui->plainTextEditDemerits)
        mReview->setDemerits(ui->plainTextEditDemerits->document()->toPlainText());
    if (sender() == ui->plainTextEditComment)
        mReview->setComment(ui->plainTextEditComment->document()->toPlainText());
}

void FormReviewCard::showEvent(QShowEvent *)
{
    emit afterShow();
}

void FormReviewCard::initVariables()
{
    mReview = new DataReview();
    ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime());
}

void FormReviewCard::initSignals()
{
    connect(this, SIGNAL(afterShow()), SLOT(onAfterShow()));
    connect(ui->pushButtonCancel, SIGNAL(clicked()), SLOT(reject()));
    connect(ui->pushButtonOK, SIGNAL(clicked()), SLOT(onSave()));
    connect(ui->toolButtonProduct, SIGNAL(clicked()), SLOT(selectProduct()));
    connect(ui->toolButtonUser, SIGNAL(clicked()), SLOT(selectUser()));
}

void FormReviewCard::initSignalsChangedData()
{
    connect(ui->comboBoxMark, SIGNAL(currentIndexChanged(int)), SLOT(onChangeMark(int)));
    connect(ui->dateTimeEdit, SIGNAL(dateTimeChanged(QDateTime)), mReview, SLOT(setDateTime(QDateTime)));
    connect(ui->comboBoxUseTime, SIGNAL(currentIndexChanged(int)), SLOT(onChangeUseTime(int)));
    connect(ui->plainTextEditMerits, SIGNAL(textChanged()), SLOT(onChangePlainText()));
    connect(ui->plainTextEditDemerits, SIGNAL(textChanged()), SLOT(onChangePlainText()));
    connect(ui->plainTextEditComment, SIGNAL(textChanged()), SLOT(onChangePlainText()));
    connect(ui->checkBoxIsActive, SIGNAL(clicked(bool)), mReview, SLOT(setIsActive(bool)));
    connect(ui->spinBoxLikes, SIGNAL(valueChanged(int)), mReview, SLOT(setCountLikes(int)));
    connect(ui->spinBoxDislikes, SIGNAL(valueChanged(int)), mReview, SLOT(setCountDislikes(int)));
}

void FormReviewCard::initPermissions()
{
    ui->toolButtonProduct->setEnabled(UserPermission::isRead(UserPermission::PRODUCTS));
    ui->toolButtonUser->setEnabled(UserPermission::isRead(UserPermission::CONTACTS));
}

bool FormReviewCard::save()
{    
    if (mReview->getId().isEmpty())
        mReview->setIsSetAllFields(true);
    return ApiAdapter::getApi()->saveReview(mReview);
}

void FormReviewCard::onAfterShow()
{
    initSignalsChangedData();
    ui->lineEditProduct->setFocus();
}

void FormReviewCard::selectProduct()
{
    FormProducts formProducts(this, false);
    if (formProducts.exec() ==  QDialog::Accepted) {
        mReview->setIdProduct(formProducts.getSelectedProducts()->at(0)->getId());
        ui->lineEditProduct->setText(formProducts.getSelectedProducts()->at(0)->getName());
    }
}

void FormReviewCard::selectUser()
{
    FormContacts formContacts(this, false);
    if (formContacts.exec() ==  QDialog::Accepted) {
        mReview->setIdUser(formContacts.getSelectedContacts()->at(0)->getId());
        ui->lineEditUser->setText(formContacts.getSelectedContacts()->at(0)->displayName());
    }

}
