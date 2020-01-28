#include "framesearchtable.h"
#include "ui_framesearchtable.h"
#include "seconfig.h"

FrameSearchTable::FrameSearchTable(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FrameSearchTable)
{
    ui->setupUi(this);

    mIsHotSearchMode = false;    
    connect(ui->lineEditSearch, SIGNAL(returnPressed()), ui->toolButtonFind, SLOT(click()));
}

FrameSearchTable::~FrameSearchTable()
{
    delete ui;
}

QString FrameSearchTable::searchText() const
{
    return ui->lineEditSearch->text().trimmed();
}

void FrameSearchTable::setSearchText(const QString &searchText)
{
    ui->lineEditSearch->setText(searchText);
}

void FrameSearchTable::setIsHotSearchMode()
{
    mIsHotSearchMode = true;
    ui->toolButtonClear->hide();
    ui->toolButtonFind->hide();
    connect(ui->lineEditSearch, SIGNAL(textEdited(QString)), SLOT(onChangeText(QString)));
}

void FrameSearchTable::setFocus()
{
    ui->lineEditSearch->setFocus();
}

void FrameSearchTable::on_toolButtonFind_clicked()
{
    emit runSearch();
}

void FrameSearchTable::on_toolButtonClear_clicked()
{
    ui->lineEditSearch->clear();
    emit runSearch();
}

void FrameSearchTable::onChangeText(const QString &search)
{
    if (mLastText.isNull() || (mLastText.compare(search) != 0)) {
        mLastText = ui->lineEditSearch->text().trimmed();
        emit runSearch();
    }
}
