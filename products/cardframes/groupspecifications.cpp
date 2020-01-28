#include "groupspecifications.h"
#include "apiadapter.h"
#include "requestfeature.h"

#include <QDebug>
#include <QHeaderView>

GroupSpecifications::GroupSpecifications(QWidget *parent) :
    QWidget(parent)
{    
    createTreeView();
}  

void GroupSpecifications::setId(const QString &id)
{
    mId = id;
}

QString GroupSpecifications::id() const
{
    return mId;
}

void GroupSpecifications::addSpecification(DataFeature *specification, const bool &isResetModel)
{    
    DataFeature *newItem = new DataFeature(specification);
    mListItems->append(newItem);
    connect(newItem, SIGNAL(modified()), SIGNAL(modified()));
    if (isResetModel)
        mModelItems->resetModel();
}

void GroupSpecifications::deleteSpecification()
{
    if (!mTableSpecifications)
        return;

    if (!mTableSpecifications->currentIndex().isValid()) {
        QMessageBox::information(this, tr("Удаление характеристики"),
                                 tr("Выделите удаляемую характеристику!"), QMessageBox::Ok);
        mTableSpecifications->setFocus();
        return;
    }

    QModelIndexList list = mTableSpecifications->selectionModel()->selectedRows();
    if (!list.size())
        return;

    for (int i = 0; i < list.size(); ++i) {
        DataItem *item = mModelItems->dataItem(list.at(i));
        if (!item)
            continue;
        item->setIsSelected(true);
    }
    for (int i = mListItems->size() - 1; i >= 0; --i) {
        if (mListItems->at(i)->getIsSelected())
            delete mListItems->takeAt(i);
    }
    mModelItems->resetModel();
    emit modified();
    if (!mListItems->size())
        emit deleteGroupSpecification(this);
}

void GroupSpecifications::resetModel()
{
    mModelItems->resetModel();
}

void GroupSpecifications::clear()
{
    mListItems->deleteItems();
    resetModel();
}

const ListFeatures *GroupSpecifications::listItems() const
{
    return mListItems;
}

void GroupSpecifications::createTreeView()
{
    QHBoxLayout *layout = new QHBoxLayout;
    mTableSpecifications = new QTableView(this);
    mTableSpecifications->horizontalHeader()->setStretchLastSection(true);
    mTableSpecifications->horizontalHeader()->setHighlightSections(false);
    mTableSpecifications->setEditTriggers(QAbstractItemView::AllEditTriggers);
    mTableSpecifications->setContextMenuPolicy(Qt::CustomContextMenu);
    mTableSpecifications->setSelectionMode(QAbstractItemView::ExtendedSelection);
    mTableSpecifications->setSelectionBehavior(QAbstractItemView::SelectRows);
    connect(mTableSpecifications, SIGNAL(customContextMenuRequested(QPoint)), SIGNAL(showContextMenu(QPoint)));
    layout->addWidget(mTableSpecifications);
    layout->setMargin(0);
    mListItems = new ListFeatures;
    mModelItems = new IntegrationsModel(this, reinterpret_cast <ListDataItems *> (mListItems));
    mModelItems->setIsFeaturesMode();
    connect(mModelItems, SIGNAL(modified()), SLOT(onModified()));
    mTableSpecifications->setModel(mModelItems);
    mTableSpecifications->resizeColumnsToContents();
    mDelegateItems = new IntegrationsDelegate(this, mModelItems);
    connect(mDelegateItems, SIGNAL(modified()), SLOT(onModified()));
    mDelegateItems->setIsFeaturesMode();
    mTableSpecifications->setItemDelegate(mDelegateItems);
    setLayout(layout);
}

void GroupSpecifications::onModified()
{
    emit modified();
}
