#include "frameseo.h"
#include "ui_frameseo.h"

#include <QToolButton>

FrameSEO::FrameSEO(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FrameSEO)
{
    ui->setupUi(this);
    mDataItem = 0;
    mLayoutSEOButtons = new FlowLayout;
    mTagsVars = new ListDataItems;
    mIsFilled = false;    
    mTypeSEO = SEOGoods;
}

FrameSEO::FrameSEO(QWidget *parent, DataItem *dataItem) :
    QWidget(parent),
    ui(new Ui::FrameSEO)
{
    ui->setupUi(this);
    mIsFilled = false;
    mDataItem = dataItem;
    mLayoutSEOButtons = new FlowLayout;
    mTagsVars = new ListDataItems;
    mTypeSEO = SEOGoods;
}

FrameSEO::~FrameSEO()
{
    delete ui;
    delete mTagsVars;
}

void FrameSEO::setSEOHeader(const QString &seoHeader)
{
    ui->lineEditSEOHeader->setText(seoHeader);
    connect(ui->lineEditSEOHeader, SIGNAL(textEdited(QString)), SIGNAL(modified()));
}

void FrameSEO::setSEOKeywords(const QString &seoKeywords)
{
    ui->lineEditSEOKeywords->setText(seoKeywords);
    connect(ui->lineEditSEOKeywords, SIGNAL(textEdited(QString)), SIGNAL(modified()));
}

void FrameSEO::setSEODescription(const QString &seoDescription)
{
    ui->plainTextEditSEODescription->document()->setPlainText(seoDescription);    
    connect(ui->plainTextEditSEODescription->document(), SIGNAL(contentsChanged()), SIGNAL(modified()));
}

void FrameSEO::setPageTitle(const QString &pageTitle)
{
    ui->lineEditPageTitle->setText(pageTitle);
    connect(ui->lineEditPageTitle, SIGNAL(textEdited(QString)), SIGNAL(modified()));
}

void FrameSEO::hidePageTitle()
{
    ui->lineEditPageTitle->hide();
    ui->label_4->hide();
}

void FrameSEO::setSEOType(const FrameSEO::SEOType seoType)
{
    mTypeSEO = seoType;
}

QString FrameSEO::seoHeader()
{
    return ui->lineEditSEOHeader->text();
}

QString FrameSEO::seoKeywords()
{
    return ui->lineEditSEOKeywords->text();
}

QString FrameSEO::seoDescription()
{
    return ui->plainTextEditSEODescription->document()->toPlainText();
}

QString FrameSEO::pageTitle()
{
    return ui->lineEditPageTitle->text();
}

void FrameSEO::fillControls()
{
    initSEOTagsVars();
    disconnectSignals();
    if (mDataItem && !mIsFilled) {
        ui->lineEditSEOHeader->setText(mDataItem->property("seoHeader").toString());
        ui->lineEditSEOKeywords->setText(mDataItem->property("seoKeywords").toString());
        ui->plainTextEditSEODescription->document()->setPlainText(mDataItem->property("seoDescription").toString());
        ui->lineEditPageTitle->setText(mDataItem->property("pageTitle").toString());
        mIsFilled = true;
    }
    connectSignals();
    ui->lineEditSEOHeader->setFocus();
}

void FrameSEO::clearInfo()
{
    ui->lineEditSEOHeader->clear();
    ui->lineEditSEOKeywords->clear();
    ui->plainTextEditSEODescription->clear();
    ui->lineEditPageTitle->clear();
    mIsFilled = false;
}

void FrameSEO::onChangeData()
{    
    if (mDataItem) {
        if (sender() == ui->lineEditSEOHeader)
            mDataItem->setSEOHeader(ui->lineEditSEOHeader->text());
        if (sender() == ui->lineEditSEOKeywords)
            mDataItem->setSEOKeywords(ui->lineEditSEOKeywords->text());
        if (sender() == ui->plainTextEditSEODescription->document())
            mDataItem->setSEODescription(ui->plainTextEditSEODescription->document()->toPlainText());
        if (sender() == ui->lineEditPageTitle)
            mDataItem->setPageTitle(ui->lineEditPageTitle->text());
    }
    emit modified();
}

void FrameSEO::onButtonClickInsertSEOTagVar()
{
     QToolButton *button = qobject_cast <QToolButton *> (sender());
     if (!button)
         return;

     if (ui->lineEditSEOHeader == focusWidget()) {
         ui->lineEditSEOHeader->insert(button->text());
         return;
     }
     if (ui->lineEditSEOKeywords == focusWidget()) {
         ui->lineEditSEOKeywords->insert(button->text());
         return;
     }
     ui->plainTextEditSEODescription->insertPlainText(button->text());
}

void FrameSEO::connectSignals()
{
    connect(ui->lineEditSEOHeader, SIGNAL(textEdited(QString)), SLOT(onChangeData()));
    connect(ui->lineEditSEOKeywords, SIGNAL(textEdited(QString)), SLOT(onChangeData()));
    connect(ui->lineEditPageTitle, SIGNAL(textEdited(QString)), SLOT(onChangeData()));
    connect(ui->plainTextEditSEODescription->document(), SIGNAL(contentsChanged()), SLOT(onChangeData()));
}

void FrameSEO::disconnectSignals()
{
    ui->lineEditSEOHeader->disconnect();
    ui->lineEditSEOKeywords->disconnect();
    ui->plainTextEditSEODescription->disconnect();
    ui->lineEditPageTitle->disconnect();
}

void FrameSEO::initSEOTagsVars()
{
    if (mTagsVars->size())
        return;

    Request request;
    if (mTypeSEO == SEOGoodsGroups)
        request.addStringValue("type", "goodsGroups");
    else request.addStringValue("type", "goods");
    if (ApiAdapter::getApi()->getListSEOTagsVars(request, mTagsVars))
        createTagsButtons();
}

void FrameSEO::createTagsButtons()
{
    for (int i = 0; i < mTagsVars->size(); ++i) {
        QToolButton *button = new QToolButton;
        button->setText(mTagsVars->at(i)->getName());
        button->setToolTip(mTagsVars->at(i)->property("note").toString());
        button->setMinimumHeight(25);
        if (mTagsVars->at(i)->property("isDynamic").toBool())
            button->setStyleSheet("background-color: #55aaff");
        else button->setStyleSheet("background-color: #55aa7f");
        connect(button, SIGNAL(clicked(bool)), SLOT(onButtonClickInsertSEOTagVar()));
        mLayoutSEOButtons->addWidget(button);
    }
    mLayoutSEOButtons->setMargin(2);
    ui->groupBoxSEOTagsVars->setLayout(mLayoutSEOButtons);
}
