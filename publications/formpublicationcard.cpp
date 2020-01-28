#include "formpublicationcard.h"
#include "ui_formpublicationcard.h"
#include "seconfig.h"
#include "seconsts.h"
#include "apiadapter.h"
#include "formnewsgroups.h"

FormPublicationCard::FormPublicationCard(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormPublicationCard)
{
    ui->setupUi(this);    
    setWindowFlags(Qt::Window);
    initVariables();
}

FormPublicationCard::~FormPublicationCard()
{
    delete ui;    
    delete mNews;
}

void FormPublicationCard::setNew(const DataNew *news)
{
    mNews->copy(news);
}

void FormPublicationCard::showEvent(QShowEvent *)
{
    getData();
    fillControls();
    initSignals();
}

void FormPublicationCard::onClickMenu()
{
    if (sender() == ui->pushButtonMainInfo)
        ui->stackedWidget->setCurrentIndex(0);
    if (sender() == ui->pushButtonImage)
        ui->stackedWidget->setCurrentIndex(1);
    if (sender() == ui->pushButtonImages) {
        if (!mFrameImages) {
            mFrameImages = new FrameListImages(this, SEConsts::Section_News, mNews->listImages());
            mFrameImages->hideCheckedsColumn();
            connect(mFrameImages, &FrameListImages::modified, this, &FormPublicationCard::setModified);
            ui->stackedWidget->addWidget(mFrameImages);
        }
        ui->stackedWidget->setCurrentWidget(mFrameImages);
    }
}

void FormPublicationCard::initVariables()
{
    mFrameImages = 0;    
    mNews = new DataNew();
    ui->pageImage->setSection(SEConsts::Section_News);
    ui->widgetFullDescription->setSection(SEConsts::Section_News);
    ui->pageImage->hideImageAlt();
    ui->stackedWidget->setCurrentIndex(0);
}

void FormPublicationCard::fillControls()
{
    ui->dateEditNew->setDate(QDate::currentDate());    
    ui->dateEditPublication->setDate(QDate::currentDate());    
    mNews->setUnModified();
    if (mNews->getId().isEmpty()) {
        mNews->setNewsDate(ui->dateEditNew->date());
        mNews->setPublicationDate(ui->dateEditPublication->date());
        return;    
    }

    ui->lineEditName->setText(mNews->getName());
    ui->lineEditUrl->setText(mNews->getUrl());
    if (mNews->newsDate().isValid())
        ui->dateEditNew->setDate(mNews->newsDate());
    if (mNews->publicationDate().isValid())
        ui->dateEditPublication->setDate(mNews->publicationDate());
    ui->widgetFullDescription->setHTMLText(mNews->property("fullDescription").toString());
    if (!mNews->getImageFile().isEmpty())
        ui->pageImage->setImage(mNews->getImageFile());
    ui->checkBoxIsActive->setChecked(mNews->getIsActive());
    ui->lineEditGroupName->setText(mNews->getNameGroup());
}

void FormPublicationCard::onOk()
{
    if (ui->lineEditName->text().trimmed().isEmpty()) {
        reject();
        return;
    }

    ui->widgetFullDescription->refreshSourceText();
    mNews->setFullDescription(ui->widgetFullDescription->textHTML());

    ui->buttonOK->setEnabled(false);
    ui->buttonCancel->setEnabled(false);    
    if (save())
        accept();
    ui->buttonOK->setEnabled(true);
    ui->buttonCancel->setEnabled(true);    
}

void FormPublicationCard::onSelectGroup()
{
    FormNewsGroups *formGroups = new FormNewsGroups(this);
    formGroups->setSelectMode(false);
    if (formGroups->exec() == QDialog::Accepted) {
        mNews->setIdGroup(formGroups->listSelected().at(0)->getId());
        ui->lineEditGroupName->setText(formGroups->listSelected().at(0)->getName());
    }
}

bool FormPublicationCard::save()
{
    mNews->setIsSetAllFields(true);    
    ui->pageImage->saveImage();
    return ApiAdapter::getApi()->saveNews(mNews);
}

void FormPublicationCard::getData()
{
    if (!mNews->getId().isEmpty()) {
        mNews->setId(mNews->getId());
        ApiAdapter::getApi()->getInfoNews(mNews);
        mNews->setUnModified();
    }
}

void FormPublicationCard::initSignals()
{
    connect(ui->toolButtonGroup, &QToolButton::clicked, this, &FormPublicationCard::onSelectGroup);
    connect(ui->buttonCancel, SIGNAL(clicked()), SLOT(reject()));
    connect(ui->buttonOK, &QPushButton::clicked, this, &FormPublicationCard::onOk);    
    connect(ui->pushButtonMainInfo, &QPushButton::clicked, this, &FormPublicationCard::onClickMenu);
    connect(ui->pushButtonImage, &QPushButton::clicked, this, &FormPublicationCard::onClickMenu);
    connect(ui->pushButtonImages, &QPushButton::clicked, this, &FormPublicationCard::onClickMenu);
    connect(ui->lineEditName, &QLineEdit::textEdited, mNews, &DataNew::setName);    
    connect(ui->lineEditUrl, &QLineEdit::textEdited, mNews, &DataNew::setUrl);
    connect(ui->dateEditNew, &QDateEdit::userDateChanged, mNews, &DataNew::setNewsDate);
    connect(ui->dateEditPublication, &QDateEdit::userDateChanged, mNews, &DataNew::setPublicationDate);
    connect(ui->checkBoxIsActive, SIGNAL(clicked(bool)), mNews, SLOT(setIsActive(bool)));    
    connect(ui->pageImage, SIGNAL(imageChanged(QString)), mNews, SLOT(setImageFile(QString)));
    connect(ui->pageImage, &FrameImage::modified,  this, &FormPublicationCard::setModified);
    connect(mNews, &DataNew::modified, this, &FormPublicationCard::setModified);
    connect(ui->widgetFullDescription, &FrameWYSIWYG::modified, this, &FormPublicationCard::setModified);
}

void FormPublicationCard::setModified()
{
    ui->buttonOK->setEnabled(!ui->lineEditName->text().trimmed().isEmpty());
}
