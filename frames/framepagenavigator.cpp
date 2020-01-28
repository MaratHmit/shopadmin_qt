#include "framepagenavigator.h"
#include "ui_framepagenavigator.h"
#include "seconfig.h"

FramePageNavigator::FramePageNavigator(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FramePageNavigator)
{
    ui->setupUi(this);
    setVisibleAllSelectButton(false);
    connectSelected();
}

FramePageNavigator::~FramePageNavigator()
{
    delete ui;
}

void FramePageNavigator::setNumRecord(int num, int count)
{
    ui->lineEdit->setText(tr("Запись") + " " + QString::number(num) + " " +
                          tr("из") + " " + QString::number(count));
}

int FramePageNavigator::getLimit()
{
    return ui->spinBox->value();
}

void FramePageNavigator::setLimit(const int &limit)
{
    ui->spinBox->setValue(limit);
}

void FramePageNavigator::setEnabledLimit(const bool &enabled)
{
    ui->spinBox->setEnabled(enabled);
}

void FramePageNavigator::setVisibleButtons(const bool &first, const bool &prev, const bool &next, const bool &last)
{
    ui->btnFirst->setVisible(first);
    ui->btnPrevios->setVisible(prev);
    ui->btnNext->setVisible(next);
    ui->btnLast->setVisible(last);
}

void FramePageNavigator::setVisibleAllSelectButton(const bool &isVisible)
{
    ui->toolButtonSelectAll->setVisible(isVisible);
    ui->labelAllSelectInfo->setVisible(isVisible);
    ui->labelAllSelectCount->setVisible(isVisible);
}

bool FramePageNavigator::isCheckedAllSelected()
{
    return ui->toolButtonSelectAll->isChecked();
}

void FramePageNavigator::disconnectSelected()
{
    ui->toolButtonSelectAll->disconnect();
}

void FramePageNavigator::connectSelected()
{
    connect(ui->toolButtonSelectAll, SIGNAL(clicked(bool)), SIGNAL(selectAll(bool)));
}

void FramePageNavigator::setUnchekedSelectButton()
{
    ui->toolButtonSelectAll->setChecked(false);
}

void FramePageNavigator::setSelectedCount(int count)
{
    ui->labelAllSelectCount->setText(QString::number(count));
}

void FramePageNavigator::on_btnNext_clicked()
{
    emit goNextPage();
}

void FramePageNavigator::on_btnPrevios_clicked()
{
    emit goPreviosPage();
}

void FramePageNavigator::on_btnLast_clicked()
{
    emit goLastPage();    
}

void FramePageNavigator::on_btnFirst_clicked()
{
    emit goFirstPage();    
}

void FramePageNavigator::on_spinBox_editingFinished()
{
    emit refreshDataStart();
}
