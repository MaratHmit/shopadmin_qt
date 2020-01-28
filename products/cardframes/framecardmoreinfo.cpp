#include "framecardmoreinfo.h"
#include "ui_framecardmoreinfo.h"
#include "apiadapter.h"

#include <QTextEdit>
#include <QDateEdit>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QDebug>

FrameCardMoreInfo::FrameCardMoreInfo(QWidget *parent, DataProduct *product) :
    QWidget(parent),
    ui(new Ui::FrameCardMoreInfo)
{
    ui->setupUi(this);
    mIsShowed = false;
    mIsFilled = false;
    mProduct = product;
    mListManufacturers = new ListDataItems();
    ui->widgetFullDescription->setSection(SEConsts::Section_Products);
}

FrameCardMoreInfo::~FrameCardMoreInfo()
{
    delete ui;
    delete mListManufacturers;
}

void FrameCardMoreInfo::showEvent(QShowEvent *)
{
    if (!mIsShowed)
        mIsShowed = true;    
}

void FrameCardMoreInfo::fillControls()
{
    disconnectSignalsChanged();
    if (mProduct && !mIsFilled) {
        ui->widgetFullDescription->setHTMLText(mProduct->property("fullDescription").toString());
        mIsFilled = true;
    }
    startTimer(10);
}

void FrameCardMoreInfo::refreshData()
{
    mIsFilled = false;
    fillControls();
}

void FrameCardMoreInfo::correctDataBeforeSave()
{
    if (mIsFilled) {
        ui->widgetFullDescription->refreshSourceText();        
        mProduct->setFullDescription(ui->widgetFullDescription->textHTML());
    }
}

void FrameCardMoreInfo::onChangeData()
{    
    if (sender() == ui->widgetFullDescription)
        mProduct->setFullDescription(ui->widgetFullDescription->textHTML());    
    emit modified();
}

void FrameCardMoreInfo::timerEvent(QTimerEvent *e)
{
    killTimer(e->timerId());
    connectSignalsChanged();
}

void FrameCardMoreInfo::disconnectSignalsChanged()
{    
    ui->widgetFullDescription->disconnect();    
}

void FrameCardMoreInfo::connectSignalsChanged(const QWidget *widget)
{
    if (!widget) {
        widget = this;
        connect(ui->widgetFullDescription, SIGNAL(documentChanged()), SLOT(onChangeData()));
    }   
}
