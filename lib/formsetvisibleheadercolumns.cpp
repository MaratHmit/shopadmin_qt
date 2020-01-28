#include "formsetvisibleheadercolumns.h"
#include "ui_formsetvisibleheadercolumns.h"

FormSetVisibleHeaderColumns::FormSetVisibleHeaderColumns(
        QWidget *parent, QHeaderView *header) :
    QDialog(parent),
    ui(new Ui::FormSetVisibleHeaderColumns)
{
    ui->setupUi(this);
    mHeader = header;
    initSignals();
}

FormSetVisibleHeaderColumns::~FormSetVisibleHeaderColumns()
{
    delete ui;
}

void FormSetVisibleHeaderColumns::showEvent(QShowEvent *)
{
    initList();
}

void FormSetVisibleHeaderColumns::setColumns()
{
    for (int i = 0; i < ui->listWidget->count(); i++) {
        if (ui->listWidget->item(i)->checkState() == Qt::Checked)
            mHeader->showSection(i);
        else mHeader->hideSection(i);
    }
    accept();
}

void FormSetVisibleHeaderColumns::initList()
{    
    ui->listWidget->clear();
    QAbstractItemModel *model = mHeader->model();

    for (int i = 0; i < mHeader->count(); i++) {
        QListWidgetItem *item = new QListWidgetItem;
        item->setData(Qt::DisplayRole, model->headerData(
                          i, Qt::Horizontal, Qt::DisplayRole).toString());
        if (mHeader->isSectionHidden(i))
            item->setData(Qt::CheckStateRole, Qt::Unchecked);
        else item->setData(Qt::CheckStateRole, Qt::Checked);
        ui->listWidget->addItem(item);
    }
}

void FormSetVisibleHeaderColumns::initSignals()
{
    connect(ui->pushButtonOK, &QPushButton::clicked, this,
            &FormSetVisibleHeaderColumns::setColumns);
    connect(ui->pushButtonCancel, &QPushButton::clicked, this,
            &FormSetVisibleHeaderColumns::reject);
}
