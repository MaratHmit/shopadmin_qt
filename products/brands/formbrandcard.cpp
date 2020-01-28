#include "formbrandcard.h"
#include "ui_formbrandcard.h"
#include "seutils.h"
#include "apiadapter.h"


FormBrandCard::FormBrandCard(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormBrandCard)
{    
    ui->setupUi(this);
    setWindowFlags(Qt::Window);
    setAttribute(Qt::WA_DeleteOnClose);

    widgetFullDescription = nullptr;
    ui->widgetDescription->setSection(SEConsts::Section_Brands);

    ui->stackedWidget->setCurrentIndex(0);
    widgetImage()->hideImageAlt();
    widgetImage()->setSection(SEConsts::Section_Brands);
    ui->pageSEO->hidePageTitle();

    initRegValidators();
    initSignals();
    brand = new DataItem();
}

FormBrandCard::~FormBrandCard()
{
    delete ui;
    delete brand;
}

void FormBrandCard::setId(const QString &id)
{
    if (!id.isEmpty())
        brand->setId(id);
}

void FormBrandCard::showEvent(QShowEvent *)
{    
    ui->lineEditName->setFocus();
    getData();
}

void FormBrandCard::on_buttonCancel_clicked()
{
    close();
}

void FormBrandCard::on_pushButtonMainInfo_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->lineEditName->setFocus();
}

void FormBrandCard::on_pushButtonImage_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}

void FormBrandCard::on_pushButtonDocInfo_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
}

FrameImage *FormBrandCard::widgetImage()
{
    return qobject_cast<FrameImage *>
             (ui->stackedWidget->widget(2));
}

void FormBrandCard::on_buttonSave_clicked()
{
    setData();
    if (save())
        ui->buttonSave->setEnabled(false);
    else ui->buttonSave->setEnabled(true);
}

void FormBrandCard::setData()
{
    ui->widgetDescription->refreshSourceText();
    if (widgetFullDescription != nullptr) {
        widgetFullDescription->refreshSourceText();
        brand->setFullDescription(widgetFullDescription->textHTML());
    }

    brand->setName(ui->lineEditName->text());
    brand->setCode(ui->lineEditCode->text());
    brand->setDescription(ui->widgetDescription->textHTML());
    brand->setImageFile(widgetImage()->fileImageName());
    brand->setSEOHeader(ui->pageSEO->seoHeader());
    brand->setSEOKeywords(ui->pageSEO->seoKeywords());
    brand->setSEODescription(ui->pageSEO->seoDescription());
}

void FormBrandCard::getData()
{
    if (brand->getId().isEmpty())
        return;

    if (ApiAdapter::getApi()->getInfoBrand(brand))
        fillControls();
}

void FormBrandCard::fillControls()
{
    ui->lineEditName->setText(brand->getName());
    ui->lineEditCode->setText(brand->getCode());
    ui->widgetDescription->setHTMLText(brand->property("description").toString());    
    widgetImage()->setImage(brand->getImageFile());
    ui->pageSEO->setSEOHeader(brand->property("seoHeader").toString());
    ui->pageSEO->setSEOKeywords(brand->property("seoKeywords").toString());
    ui->pageSEO->setSEODescription(brand->property("seoDescription").toString());
    ui->buttonSave->setEnabled(false);
}

bool FormBrandCard::save()
{    
    ui->buttonCancel->setEnabled(false);
    ui->buttonOK->setEnabled(false);
    ui->buttonSave->setEnabled(false);
    brand->setIsSetAllFields(true);

    if (ApiAdapter::getApi()->saveBrand(brand)) {
        widgetImage()->saveImage();
        emit saved(brand->getId());
        ui->buttonCancel->setEnabled(true);
        ui->buttonOK->setEnabled(true);
        return true;
    }
    ui->buttonCancel->setEnabled(true);
    ui->buttonOK->setEnabled(true);
    return false;
}

void FormBrandCard::initRegValidators()
{
    QRegExp regExpr("^\\S[A-я0-9-_!\\s]*");
    QRegExpValidator *regCodeText = new QRegExpValidator(regExpr, this);
    ui->lineEditCode->setValidator(regCodeText);
}

void FormBrandCard::initSignals()
{
    connect(widgetImage(), SIGNAL(modified()), SLOT(onModified()));
    connect(ui->pageSEO, SIGNAL(modified()), SLOT(onModified()));
    connect(ui->lineEditName, SIGNAL(textEdited(QString)), SLOT(onModified()));
    connect(ui->lineEditCode, SIGNAL(textEdited(QString)), SLOT(onModified()));
    connect(ui->widgetDescription, SIGNAL(modified()), SLOT(onModified()));    
}

void FormBrandCard::on_pushButtonTranslit_clicked()
{
    setCursor(Qt::WaitCursor);
    QStringList list;
    list.append(ui->lineEditName->text());
    list = ApiAdapter::getApi()->translit(list);
    if (list.size())
        ui->lineEditCode->setText(list.at(0));
    setCursor(Qt::ArrowCursor);

}

void FormBrandCard::on_buttonOK_clicked()
{
    setData();
    if (save())
        accept();
}

void FormBrandCard::onModified()
{
    ui->buttonSave->setEnabled(true);
}

void FormBrandCard::on_pushButtonDescription_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    if (widgetFullDescription == nullptr) {
        widgetFullDescription = new FrameWYSIWYG(ui->pageDescription);
        ui->pageDescription->layout()->addWidget(widgetFullDescription);
        widgetFullDescription->setHTMLText(brand->property("fullDescription").toString());
        connect(widgetFullDescription, SIGNAL(modified()), SLOT(onModified()));
    }
}
