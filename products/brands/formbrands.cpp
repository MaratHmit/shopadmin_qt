#include "formbrands.h"
#include "ui_formbrands.h"
#include "seconfig.h"
#include "formbrandcard.h"
#include "apiadapter.h"
#include "userpermission.h"

#include <QMessageBox>
#include <memory>

FormBrands::FormBrands(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormBrands)
{
    ui->setupUi(this);
    ui->toolButtonSelect->setHidden(true);
    ui->actionSelect->setVisible(false);

    brands  = new ListDataItems();
    model = new NameTableModel(this, brands);
    initFields();
    model->clearFields();
    foreach (Field *field, mFields)
        model->appendField(field);
    mIsShowed = false;
    mIsLoadedLayouts = false;
    mIsSelectMode = false;
    mSelectedBrands = new ListDataItems();

    timerSort = new QTimer(this);
    timerSort->setSingleShot(true);

    initTreeBrands();
    initActions();    
    initPermission();
}

FormBrands::~FormBrands()
{
    if (mIsShowed)
        SEConfig::saveTableState(ui->treeViewBrands->header(),
                                        ui->treeViewBrands->objectName());
    delete ui;    
    delete mSelectedBrands;
}

void FormBrands::setSelectMode(const bool &isMultiSelect)
{
    if (!isMultiSelect)
         ui->treeViewBrands->setSelectionMode(QAbstractItemView::SingleSelection);
    mIsSelectMode = true;
    ui->actionRemove->setVisible(false);
    ui->actionSelect->setVisible(true);
    ui->toolButtonDelete->hide();
    ui->toolButtonSelect->show();
}

ListDataItems *FormBrands::selectedBrands() const
{
    QModelIndexList list = ui->treeViewBrands->selectionModel()->selectedRows();
    mSelectedBrands->clear();
    for (int i = 0; i < list.size(); i++)
        mSelectedBrands->append(brands->at(list.at(i).row()));
    return mSelectedBrands;
}

void FormBrands::showEvent(QShowEvent *)
{
    if (!mIsShowed) {
        mIsLoadedLayouts = SEConfig::restoreTableState(ui->treeViewBrands->header(),
                                                           ui->treeViewBrands->objectName());
        initActionsSignals();
        refreshData();
        mIsShowed = true;
    }
}

void FormBrands::refreshData()
{
    if (!UserPermission::isRead(UserPermission::PRODUCTS))
        return;

    resetRequest();

    QString id;

    disconnect(ui->treeViewBrands->selectionModel(),
               SIGNAL(selectionChanged(QItemSelection,QItemSelection)));

    QModelIndex index = ui->treeViewBrands->currentIndex();
    if (index.isValid())
        id = model->idData(ui->treeViewBrands->currentIndex());

    model->beginReset();
    ApiAdapter::getApi()->getListBrands(mRequest, model->getItems());
    model->endReset();
    if (!mIsLoadedLayouts)
        for (int i = 0; i < model->columnCount() - 1; i++)
            ui->treeViewBrands->resizeColumnToContents(i);

    connect(ui->treeViewBrands->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(onChangeBrand()));

    goToRowById(id);
}

void FormBrands::onSaveBrand(const QString &id)
{
    refreshData();
    goToRowById(id);
}

void FormBrands::onChangeBrand()
{
    QString id = currentId();

    ui->actionAdd->setEnabled(isEditPermission);
    ui->actionEdit->setEnabled(!id.isEmpty() && isEditPermission);
    ui->actionRemove->setEnabled(!id.isEmpty() && isDeletePermission);
    ui->actionSelect->setEnabled(!id.isEmpty());
    ui->actionUp->setEnabled(ui->treeViewBrands->currentIndex().isValid() &&
                             model->dataItem(ui->treeViewBrands->currentIndex())->row() && isEditPermission);
    ui->actionDown->setEnabled(ui->treeViewBrands->currentIndex().isValid() &&
                               model->dataItem(ui->treeViewBrands->currentIndex())->parent() &&
                               model->dataItem(ui->treeViewBrands->currentIndex())->parent()->children().count() - 1 !=
            model->dataItem(ui->treeViewBrands->currentIndex())->row() && isEditPermission);
}

void FormBrands::resetRequest()
{
    mRequest.clear();
    mRequest.clearFilter();

    QString sortField = mFields.at(ui->treeViewBrands->header()->sortIndicatorSection())->name();
    mRequest.setSortBy(sortField);
    mRequest.setSearchText(ui->widgetSearch->searchText().trimmed());
    mRequest.setAscendingOrder(ui->treeViewBrands->header()->sortIndicatorOrder() ==
            Qt::AscendingOrder);
}

void FormBrands::initFields()
{    
    mFields.append(new Field("id", "№"));
    mFields.append(new Field("image", "Фото"));
    mFields.append(new Field("code", "Код"));
    mFields.append(new Field("name", "Наименование"));
    mFields.append(new Field("description", "Краткое описание"));
    mFields.append(new Field("fullDescription", "Полное описание"));
}

void FormBrands::initTreeBrands()
{
    ui->treeViewBrands->setModel(model);
    ui->treeViewBrands->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->treeViewBrands->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->treeViewBrands->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeViewBrands, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(showContextMenu(QPoint)));
    connect(ui->treeViewBrands->header(), SIGNAL(sectionClicked(int)), SLOT(refreshData()));
    connect(ui->widgetSearch, SIGNAL(runSearch()), SLOT(refreshData()));
}

void FormBrands::initActions()
{
    ui->toolButtonAdd->setDefaultAction(ui->actionAdd);
    ui->toolButtonEdit->setDefaultAction(ui->actionEdit);
    ui->toolButtonDelete->setDefaultAction(ui->actionRemove);
    ui->toolButtonRefresh->setDefaultAction(ui->actionRefresh);
    ui->toolButtonSelect->setDefaultAction(ui->actionSelect);
    ui->toolButtonDown->setDefaultAction(ui->actionDown);
    ui->toolButtonUp->setDefaultAction(ui->actionUp);
}

void FormBrands::initActionsSignals()
{
    connect(ui->actionAdd, SIGNAL(triggered()), SLOT(onAddEditBrand()));
    connect(ui->actionEdit, SIGNAL(triggered()), SLOT(onAddEditBrand()));
    connect(ui->actionSelect, SIGNAL(triggered()), SLOT(accept()));
    if (mIsSelectMode)
        connect(ui->treeViewBrands, SIGNAL(doubleClicked(QModelIndex)),
                ui->actionSelect, SLOT(trigger()));
    else connect(ui->treeViewBrands, SIGNAL(doubleClicked(QModelIndex)),
                 ui->actionEdit, SLOT(trigger()));
    connect(ui->actionRefresh, SIGNAL(triggered()), SLOT(refreshData()));
    connect(ui->actionRemove, SIGNAL(triggered()), SLOT(onRemoveBrand()));
    connect(ui->actionDown, SIGNAL(triggered()), SLOT(onSort()));
    connect(ui->actionUp, SIGNAL(triggered()), SLOT(onSort()));
    connect(timerSort, SIGNAL(timeout()), SLOT(saveSort()));
}

void FormBrands::initPermission()
{
    isAddPermission = UserPermission::isAdd(UserPermission::PRODUCTS);
    isEditPermission = UserPermission::isEdit(UserPermission::PRODUCTS);
    isDeletePermission = UserPermission::isDelete(UserPermission::PRODUCTS);

    ui->actionAdd->setEnabled(isAddPermission);
    ui->actionEdit->setEnabled(isEditPermission);
    ui->actionRemove->setEnabled(isDeletePermission);
}


void FormBrands::goToRowById(QString id)
{
    if (!id.isEmpty()) {
        ui->treeViewBrands->selectionModel()->setCurrentIndex(
                    model->getModelIndexById(id), QItemSelectionModel::SelectCurrent |
                    QItemSelectionModel::Rows);
        ui->treeViewBrands->setFocus();
        return;
    }

    if (model->getItems()->size())
        ui->treeViewBrands->selectionModel()->setCurrentIndex(
                    model->index(0, 0), QItemSelectionModel::SelectCurrent |
                    QItemSelectionModel::Rows);
    else onChangeBrand();
    ui->treeViewBrands->setFocus();
}

QString FormBrands::currentId()
{
    return model->idData(ui->treeViewBrands->currentIndex());
}

void FormBrands::onAddEditBrand()
{
    int isNew = false;

    isNew = sender() == ui->actionAdd;
    ui->actionAdd->setEnabled(false);
    ui->actionEdit->setEnabled(false);

    FormBrandCard *card = new FormBrandCard(this);
    QString id = currentId();
    if (!id.isEmpty() && !isNew)
        card->setId(id);
    connect(card, SIGNAL(saved(QString)), SLOT(onSaveBrand(QString)));
    card->exec();

    onChangeBrand();
}

void FormBrands::showContextMenu(QPoint point)
{
    onChangeBrand();

    QPoint globalPos;

    if (sender()->inherits("QAbstractScrollArea"))
        globalPos = ((QAbstractScrollArea*)sender())->viewport()->mapToGlobal(point);
    else globalPos = ((QWidget*)sender())->mapToGlobal(point);

    QMenu menu;

    menu.addAction(ui->actionAdd);
    menu.addAction(ui->actionEdit);    
    menu.addAction(ui->actionRemove);
    if (ui->actionSelect->isVisible()) {
      menu.addSeparator();
      menu.addAction(ui->actionSelect);
    }
    menu.exec(globalPos);
}

void FormBrands::onSort()
{
    if (!ui->treeViewBrands->currentIndex().isValid())
        return;

    bool isMoveUp = sender() == ui->actionUp;

    QString id = model->dataItem(ui->treeViewBrands->currentIndex())->getId();
    model->beginReset();
    if (isMoveUp)
        brands->moveUp(model->dataItem(ui->treeViewBrands->selectionModel()->currentIndex()));
    else brands->moveDown(model->dataItem(ui->treeViewBrands->selectionModel()->currentIndex()));
    model->endReset();
    goToRowById(id);

    if (timerSort->isActive())
        timerSort->stop();
    timerSort->start(300);
}

void FormBrands::saveSort()
{
    ApiAdapter::getApi()->sortBrands(model->getSortParamsForApi());
}

void FormBrands::onRemoveBrand()
{
    QString msg;
    if (ui->treeViewBrands->selectionModel()->selectedRows().count() > 1)
        msg = tr("Удалить выбранные бренды?");
    else msg = tr("Удалить бренд") + ": " +
            model->nameData(ui->treeViewBrands->currentIndex()) + "?";

    QMessageBox::StandardButton mesRepl;

    mesRepl = QMessageBox::question(this, tr("Подтверждение удаления"),
                                    msg, QMessageBox::Yes | QMessageBox::No);
    if(mesRepl == QMessageBox::Yes)    {
        Request request;
        QModelIndexList list = ui->treeViewBrands->selectionModel()->selectedRows();

        for (int i = 0; i < list.count(); i++)
            request.addId(model->data(list.at(i), Qt::DisplayRole).toString());
        ApiAdapter::getApi()->deleteBrands(request);
        refreshData();
    }
}
