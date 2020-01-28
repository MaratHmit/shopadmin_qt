#include "formsesections.h"
#include "ui_formsesections.h"

#include <QDebug>

FormSESections::FormSESections(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormSESections)
{
    ui->setupUi(this);

    initVariables();
    initSignals();
}

FormSESections::~FormSESections()
{
    delete ui;
}

void FormSESections::reject()
{

}

void FormSESections::hideEvent(QHideEvent *)
{
    SEConfig::saveSplitterLayout(ui->splitterSections);
    storeStateTables();
}

void FormSESections::showEvent(QShowEvent *)
{
    SEConfig::loadSplitterLayout(ui->splitterSections);
    if (!mSections->size()) {
        restoreStateTables();
        refreshSections();
    }
}

void FormSESections::timerEvent(QTimerEvent *e)
{
    killTimer(e->timerId());
    saveSort();
}

void FormSESections::goToRowById(const QString &id)
{
    QModelIndex modelIndex = mModelSectionsItems->getModelIndexById(id);

    if (modelIndex.isValid()) {
        ui->treeViewItems->selectionModel()->setCurrentIndex(
                    modelIndex, QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);
        return;
    }

    if (mModelSectionsItems->rowCount() > 0)
        ui->treeViewItems->selectionModel()->
                setCurrentIndex(mModelSectionsItems->index(0, 0),
                                QItemSelectionModel::SelectCurrent |
                                QItemSelectionModel::Rows);
    else onChangeCurrentItem();
}

void FormSESections::initVariables()
{

    preloader = new Preloader(this);
    mIdTimer = 0;
    mSections = new ListDataItems;
    mModelSections = new NameTableModel(this, mSections);    
    mModelSections->appendField(new Field("isActiveIco", "Вид", Qt::AlignCenter | Qt::AlignVCenter, Qt::DecorationRole));
    mModelSections->appendField(new Field("note", "Страница"));
    mModelSections->appendField(new Field("nameGroup", "Раздел"));
    ui->treeViewSections->setModel(mModelSections);

    mSectionsItems = new ListDataItems;
    mModelSectionsItems = new NameTableModel(this, mSectionsItems);    
    mModelSectionsItems->clearFields();    
    mModelSectionsItems->appendField(new Field("image", "Фото"));
    mModelSectionsItems->appendField(new Field("id", "Ид.", Qt::AlignRight | Qt::AlignVCenter));
    mModelSectionsItems->appendField(new Field("isActiveIco", "Вид", Qt::AlignCenter | Qt::AlignVCenter, Qt::DecorationRole));
    mModelSectionsItems->appendField(new Field("sortIndex", "Порядок", Qt::AlignRight | Qt::AlignVCenter));
    mModelSectionsItems->appendField(new Field("name", "Наименование"));
    mModelSectionsItems->appendField(new Field("note", "Примечание"));    
    ui->treeViewItems->setModel(mModelSectionsItems);
}

void FormSESections::initSignals()
{
    connect(ui->toolButtonRefreshSections, &QToolButton::clicked, this, &FormSESections::refreshSections);
    connect(ui->treeViewSections->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            SLOT(onChangeCurrentSection()));
    connect(ui->treeViewItems->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            SLOT(onChangeCurrentItem()));
    connect(ui->toolButtonEditSection, SIGNAL(clicked(bool)), SLOT(editSection()));
    connect(ui->treeViewSections, SIGNAL(doubleClicked(QModelIndex)),
            ui->toolButtonEditSection, SLOT(click()));
    connect(ui->toolButtonAdd, SIGNAL(clicked(bool)), SLOT(addEditSectionItem()));
    connect(ui->toolButtonEdit, SIGNAL(clicked(bool)), SLOT(addEditSectionItem()));
    connect(ui->treeViewItems, SIGNAL(doubleClicked(QModelIndex)), SLOT(addEditSectionItem()));
    connect(ui->toolButtonDelete, SIGNAL(clicked(bool)), SLOT(onDeleteSectionItem()));
    connect(ui->toolButtonUp, SIGNAL(clicked(bool)), SLOT(onSort()));
    connect(ui->toolButtonDown, SIGNAL(clicked(bool)), SLOT(onSort()));
}

void FormSESections::storeStateTables()
{
    SEConfig::saveTableState(ui->treeViewSections->header(), ui->treeViewSections->objectName());
    SEConfig::saveTableState(ui->treeViewItems->header(), ui->treeViewItems->objectName());
}

void FormSESections::restoreStateTables()
{
    mIsRestoreStates = SEConfig::restoreTableState(ui->treeViewSections->header(), ui->treeViewSections->objectName()) &&
            SEConfig::restoreTableState(ui->treeViewItems->header(), ui->treeViewItems->objectName());
}

void FormSESections::saveSort()
{
    ApiAdapter::getApi()->sortSESectionItem(mModelSectionsItems->getSortParamsForApi());
}

void FormSESections::onChangeCurrentSection()
{
    ui->toolButtonEditSection->setEnabled(ui->treeViewSections->currentIndex().isValid() &&
                                          !mModelSections->dataItem(ui->treeViewSections->currentIndex())->getIdParent().isEmpty());
    refreshSectionsItems();
}

void FormSESections::onChangeCurrentItem()
{
    ui->toolButtonAdd->setEnabled(true);
    ui->toolButtonEdit->setEnabled(ui->treeViewItems->currentIndex().isValid());
    ui->toolButtonDelete->setEnabled(ui->treeViewItems->currentIndex().isValid());
    ui->toolButtonUp->setEnabled(ui->treeViewItems->currentIndex().isValid() &&
                                        ui->treeViewItems->selectionModel()->currentIndex().row());
    ui->toolButtonDown->setEnabled(ui->treeViewItems->currentIndex().isValid() &&
                                          mModelSectionsItems->rowCount() - 1 -
                                          ui->treeViewItems->selectionModel()->currentIndex().row());
}

void FormSESections::onSort()
{
    if (!ui->treeViewItems->currentIndex().isValid())
        return;
    if (mIdTimer)
        killTimer(mIdTimer);

    bool isMoveUp = sender() == ui->toolButtonUp;

    QString id = mModelSectionsItems->idData(ui->treeViewItems->currentIndex());
    mModelSectionsItems->beginReset();
    if (isMoveUp)
        mSectionsItems->moveUp(mModelSectionsItems->dataItem(ui->treeViewItems->selectionModel()->currentIndex()));
    else mSectionsItems->moveDown(mModelSectionsItems->dataItem(ui->treeViewItems->selectionModel()->currentIndex()));
    mModelSectionsItems->endReset();
    goToRowById(id);
    mIdTimer = startTimer(1000);
}

void FormSESections::addEditSectionItem()
{    
    bool isNew = sender() == ui->toolButtonAdd;

    DataItem *item;
    if (isNew) {
        item = new DataItem;
        item->setIdGroup(mLastIdSection);
    } else {
        if (!ui->treeViewItems->currentIndex().isValid())
            return;

        item = mModelSectionsItems->dataItem(ui->treeViewItems->currentIndex());        
    }    
    DialogSESectionItem dialog(this, item);    
    if (dialog.exec() == QDialog::Accepted) {
        item->copy(dialog.sectionItem());
        if (isNew) {
            mSectionsItems->append(item);
            mModelSectionsItems->resetModel();
        } else
            mModelSections->setData(mModelSections->index(ui->treeViewItems->currentIndex().row(), 0), item->getId(), Qt::EditRole);
    } else {
        if (isNew)
            delete item;
    }
    onChangeCurrentItem();
}

void FormSESections::onDeleteSectionItem()
{
    if (!ui->treeViewItems->currentIndex().isValid())
        return;

    QString msg;
    if (ui->treeViewItems->selectionModel()->selectedRows().count() > 1)
        msg = tr("Удалить выбранные элементы раздела?");
    else msg = tr("Удалить элемент раздела") + ": " +
            mModelSectionsItems->nameData(ui->treeViewItems->currentIndex()) + "?";

    QMessageBox::StandardButton mesRepl;

    mesRepl = QMessageBox::question(this, tr("Подтверждение удаления"),
                                    msg, QMessageBox::Yes | QMessageBox::No);
    if(mesRepl == QMessageBox::Yes)    {
        Request request;
        QModelIndexList list = ui->treeViewItems->selectionModel()->selectedRows();

        for (int i = 0; i < list.count(); i++)
            request.addId(mModelSectionsItems->dataItem(list.at(i))->getId());
        if (ApiAdapter::getApi()->deleteSESectionItem(request)) {
            mLastIdSection.clear();
            refreshSectionsItems();
        }
    }
}

void FormSESections::editSection()
{
    if (!ui->treeViewSections->currentIndex().isValid())
        return;

    DataItem *section = mModelSections->dataItem(ui->treeViewSections->currentIndex());
    if (section->getIdParent().isEmpty())
        return;

    DialogSESection dialog(this, section);
    if (dialog.exec() == QDialog::Accepted) {
        section->copy(dialog.section());
        QModelIndex index = ui->treeViewSections->currentIndex();
        mModelSections->setData(mModelSections->index(index.row(), 1, index.parent()), section->getName(), Qt::EditRole);
    }
}

void FormSESections::refreshSections()
{
    mLastIdSection.clear();
    preloader->startAnimation();
    ApiAdapter::getApi()->getListSESections(Request(), mSections);
    mModelSections->resetModel();
    if (!mIsRestoreStates)
        for (int i = 0; i < mModelSections->columnCount() - 1; i++)
            ui->treeViewSections->resizeColumnToContents(i);
    ui->treeViewSections->expandAll();
    if (mModelSections->rowCount() > 0)
        ui->treeViewSections->selectionModel()->
                setCurrentIndex(mModelSections->index(0, 0),
                                QItemSelectionModel::SelectCurrent |
                                QItemSelectionModel::Rows);    
    ui->treeViewSections->setFocus();
    preloader->stopAnimation();
}

void FormSESections::refreshSectionsItems()
{
    if (!ui->treeViewSections->currentIndex().isValid())
        return;

    DataItem *section = mModelSections->dataItem(ui->treeViewSections->currentIndex());
    QString idSection = section->getIdParent();
    if (idSection.isEmpty())
        idSection = section->getId();
    if (idSection == mLastIdSection)
        return;

    mLastIdSection = idSection;
    Request request;
    request.setIdParent(idSection);
    setCursor(Qt::WaitCursor);
    ApiAdapter::getApi()->getListSESectionsItems(request, mSectionsItems);
    mModelSectionsItems->resetModel();
    onChangeCurrentItem();
    if (mModelSectionsItems->rowCount() > 0)
        ui->treeViewItems->selectionModel()->
                setCurrentIndex(mModelSectionsItems->index(0, 0),
                                QItemSelectionModel::SelectCurrent |
                                QItemSelectionModel::Rows);
    if (!mIsRestoreStates)
        for (int i = 0; i < mModelSectionsItems->columnCount() - 1; i++)
            ui->treeViewItems->resizeColumnToContents(i);
    mIsRestoreStates = true;
    ui->treeViewItems->setFocus();
    setCursor(Qt::ArrowCursor);
}
