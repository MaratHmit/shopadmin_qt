#include "dialoglettercard.h"
#include "ui_dialoglettercard.h"
#include "apiadapter.h"

DialogLetterCard::DialogLetterCard(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogLetterCard)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window);
    setAttribute(Qt::WA_DeleteOnClose);
    initVariables();
    initSignals();
}

DialogLetterCard::~DialogLetterCard()
{
    delete ui;
    delete mLetter;
}

void DialogLetterCard::setGroup(const QString &idGroup, const QString &nameGroup)
{
    mLetter->setIdGroup(idGroup);
    ui->lineEditGroupName->setText(nameGroup);
}

void DialogLetterCard::setLetter(const DataItem *letter)
{
    mLetter->copy(letter);
}

void DialogLetterCard::showEvent(QShowEvent *)
{
    if (!mLetter->getId().isEmpty())
        fillControls();
    connectSignalsOnChanged();
}

void DialogLetterCard::onOk()
{
    ui->pushButtonOK->setEnabled(false);
    ui->pushButtonCancel->setEnabled(false);

    bool isNew = mLetter->getId().isEmpty();
    ui->widget->refreshSourceText();
    mLetter->setProperty("letter", ui->widget->textHTML());
    if (ApiAdapter::getApi()->saveTemplateLetter(mLetter)) {
        emit save(isNew, mLetter);
        accept();
    }

    ui->pushButtonOK->setEnabled(true);
    ui->pushButtonCancel->setEnabled(true);
}

void DialogLetterCard::onChangeData()
{
    ui->pushButtonOK->setEnabled(!ui->lineEditName->text().trimmed().isEmpty());
    if (sender() == ui->lineEditName)
        mLetter->setName(ui->lineEditName->text().trimmed());
    if (sender() == ui->lineEditCode)
        mLetter->setCode(ui->lineEditCode->text().trimmed());
    if (sender() == ui->lineEditSubject)
        mLetter->setProperty("subject", ui->lineEditSubject->text().trimmed());
}

void DialogLetterCard::initVariables()
{
    mLetter = new DataItem();
    ui->widget->setSection(SEConsts::Section_News, false);
}

void DialogLetterCard::initSignals()
{
    connect(ui->pushButtonCancel, &QAbstractButton::clicked, this, &DialogLetterCard::reject);
    connect(ui->pushButtonOK, &QAbstractButton::clicked, this, &DialogLetterCard::onOk);
}

void DialogLetterCard::connectSignalsOnChanged()
{
    connect(ui->lineEditName, &QLineEdit::textEdited, this, &DialogLetterCard::onChangeData);
    connect(ui->lineEditSubject, &QLineEdit::textEdited, this, &DialogLetterCard::onChangeData);
    connect(ui->lineEditCode, &QLineEdit::textEdited, this, &DialogLetterCard::onChangeData);
    connect(ui->widget, &FrameWYSIWYG::modified, this, &DialogLetterCard::onChangeData);
}

void DialogLetterCard::fillControls()
{
    ui->lineEditCode->setText(mLetter->getCode());
    ui->lineEditName->setText(mLetter->getName());
    ui->lineEditSubject->setText(mLetter->property("subject").toString());
    ui->widget->setHTMLText(mLetter->property("letter").toString());
}
