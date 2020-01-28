#include "frameimportcolumn.h"
#include "ui_frameimportcolumn.h"

FrameImportColumn::FrameImportColumn(QWidget *parent, DataItem *item, QStringList fields) :
    QWidget(parent),
    ui(new Ui::FrameImportColumn)
{
    ui->setupUi(this);
    this->item = item;

    ui->labelColumnName->setText(item->property("title").toString());
    ui->labelColumnSample->setText(item->property("sample").toString());
    ui->comboBox->addItems(fields);
}

FrameImportColumn::~FrameImportColumn()
{
    delete ui;
}

const QString FrameImportColumn::getFieldName()
{
    return ui->comboBox->currentText();
}

void FrameImportColumn::setCurrentValue(const QString &value)
{
    if (!value.isEmpty())
        ui->comboBox->setCurrentText(value);
}
