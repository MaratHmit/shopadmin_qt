#include "formparametersfilters.h"
#include "ui_formparametersfilters.h"
#include "apiadapter.h"

FormParametersFilters::FormParametersFilters(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormParametersFilters)
{
    ui->setupUi(this);
    initVariables();
    initSignals();
}

FormParametersFilters::~FormParametersFilters()
{
    delete ui;
    delete mItems;
    if (mSelectedItems)
        delete mSelectedItems;
}

void FormParametersFilters::setExistingFilters(ListDataItems *items)
{
    mExistingItems = items;
}

const ListDataItems *FormParametersFilters::selectedItems()
{
    if (mSelectedItems)
        return mSelectedItems;

    mSelectedItems = new ListDataItems();
    QModelIndexList list = ui->listView->selectionModel()->selectedRows();
    for (int i = 0; i < list.size(); i++) {
        mItems->at(list.at(i).row())->setIsChecked(true);
        mSelectedItems->append(mItems->at(list.at(i).row()));
    }
    return mSelectedItems;
}

void FormParametersFilters::showEvent(QShowEvent *)
{
    startTimer(1);
}

void FormParametersFilters::timerEvent(QTimerEvent *event)
{
    killTimer(event->timerId());
    refreshData();
}

void FormParametersFilters::initVariables()
{
    mSelectedItems = 0;
    mItems = new ListDataItems();
    mModel = new NameTableModel(this, mItems, true);
    ui->listView->setModel(mModel);
}

void FormParametersFilters::initSignals()
{
    connect(ui->pushButtonOk, &QAbstractButton::clicked, this, &FormParametersFilters::accept);
}

void FormParametersFilters::removeTheExisting()
{
    for (int i = 0; i < mExistingItems->size(); i++)
        for (int j = 0; j < mItems->size(); j++)
            if (mExistingItems->at(i)->getName() == mItems->at(j)->getName()) {
                delete mItems->takeAt(j);
                break;
            }
}

void FormParametersFilters::refreshData()
{
    setCursor(QCursor(Qt::WaitCursor));
    if (ApiAdapter::getApi()->getListParametersFilters(mItems)) {
        removeTheExisting();
        mModel->resetModel();
    }
    setCursor(QCursor(Qt::ArrowCursor));
}
