#include "formoptions.h"
#include "ui_formoptions.h"

#include <QKeyEvent>

FormOptions::FormOptions(QWidget *parent, const bool &isSelectMode) :
    QDialog(parent),
    ui(new Ui::FormOptions)
{
    ui->setupUi(this);

    mSelectMode = isSelectMode;

    ui->widgetGroupsOptions->hide();    

    if (mSelectMode) {
        ui->checkBoxIsShowGroups->hide();
        ui->widgetOptions->hide();
        ui->toolButtonDelete->hide();
        ui->toolButtonDown->hide();
        ui->toolButtonUp->hide();
    } else {
        setWindowFlags(Qt::Widget);
        ui->toolButtonSelect->hide();
    }

    initSignals();

    mOptionValues = new ListOptionValues();
    mModel = new NameTableModel(this, reinterpret_cast <ListDataItems *> (mOptionValues));

    mTypeTimer = 0;
    mIdTimer = 0;    
    mIsShowed = false;
    mIsRestoreStates = false;
    mListSelected = new ListOptionValues;

    initTreeView();
    initButtonsSignals();
}

FormOptions::~FormOptions()
{
    delete ui;
    delete mListSelected;
}

QString FormOptions::currentId() const
{
    QModelIndex index = ui->treeView->currentIndex();
    if (index.isValid())
        return mModel->idData(index);
    else return 0;
}

void FormOptions::setIdOption(const QString id)
{
    mIdOption = id;
}

ListOptionValues *FormOptions::selectedList() const
{
    mListSelected->clear();
    QModelIndexList list =
            ui->treeView->selectionModel()->selectedRows();
    for (int i = 0; i < list.size(); i++) {
        DataOptionValue *item = dynamic_cast <DataOptionValue *> (mModel->dataItem(list.at(i)));
        if (item)
            mListSelected->append(item);
    }
    return mListSelected;
}

void FormOptions::refreshData()
{
    setCursor(Qt::WaitCursor);
    QString id = currentId();
    resetRequest();

    mModel->beginReset();
    ApiAdapter::getApi()->getListOptionsValues(mRequest, mOptionValues);
    mModel->endReset();

    goToRowById(id);
    ui->treeView->setFocus();
    setCursor(Qt::ArrowCursor);
}

void FormOptions::onAddEditValue()
{
    int isNew = false;

    QToolButton *button = qobject_cast<QToolButton *> (sender());
    isNew = button == ui->toolButtonAdd;

    FormOptionValueCard *card = new FormOptionValueCard(this);
    if (mSelectMode) {
        card->setCurrentIdOption(mIdOption);
        card->hideSelectedOption();
    } else {
        card->setOptions(ui->widgetOptions->getListOptions());
        card->setCurrentIdOption(ui->widgetOptions->currentId());
    }
    QString id = currentId();
    if (!id.isEmpty() && !isNew)
        card->setId(id);
    if (card->exec() == QDialog::Accepted)
        refreshData();
}

void FormOptions::onUpDownValue()
{
    if (mIdTimer)
        killTimer(mIdTimer);

    QToolButton *button = qobject_cast<QToolButton *> (sender());
    bool isMoveUp = button == ui->toolButtonUp;

    QString id = currentId();
    mModel->beginReset();
    if (isMoveUp)
        mOptionValues->moveUp(mModel->dataItem((ui->treeView->selectionModel()->currentIndex())));
    else mOptionValues->moveDown(mModel->dataItem((ui->treeView->selectionModel()->currentIndex())));
    mModel->endReset();

    goToRowById(id);
    mTypeTimer = 1;
    mIdTimer = startTimer(1000);
}

void FormOptions::onSelect()
{
     accept();
}

void FormOptions::onChange()
{
    QString id = currentId();

    ui->toolButtonEdit->setEnabled(!id.isEmpty());
    ui->toolButtonDelete->setEnabled(!id.isEmpty());
    ui->toolButtonUp->setEnabled(!id.isEmpty() && ui->treeView->selectionModel()->currentIndex().row());
    ui->toolButtonDown->setEnabled(!id.isEmpty() && mModel->rowCount() - 1 -
                                        ui->treeView->selectionModel()->currentIndex().row());
    ui->toolButtonSelect->setEnabled(!id.isEmpty());
}

void FormOptions::onDeleteValue()
{
    QString msg;
    if (ui->treeView->selectionModel()->selectedRows().count() > 1)
        msg = tr("Удалить выбранные элементы?");
    else msg = tr("Удалить элемент") + ": " +
            mModel->nameData(ui->treeView->currentIndex()) + "?";

    QMessageBox::StandardButton mesRepl;

    mesRepl = QMessageBox::question(this, tr("Подтверждение удаления"),
                                    msg, QMessageBox::Yes | QMessageBox::No);
    if(mesRepl == QMessageBox::Yes)    {
        Request request;
        QModelIndexList list = ui->treeView->selectionModel()->selectedRows(0);

        for (int i = 0; i < list.count(); i++)
            request.addId(mModel->idData(list.at(i)));
        ApiAdapter::getApi()->deleteOptionsValues(request);
        refreshData();
    }
}

void FormOptions::showEvent(QShowEvent *)
{
    if (!mIsShowed) {
        SEConfig::loadSplitterLayout(ui->splitter);
        restoreStateTable();
        startTimer(1);
    }
}

void FormOptions::hideEvent(QHideEvent *)
{
    if (mIsShowed)
        saveStateTable();
}

void FormOptions::timerEvent(QTimerEvent *event)
{
    killTimer(event->timerId());
    switch (mTypeTimer) {
    case 0:
        if (!mIsShowed) {
            refreshData();
            mIsShowed = true;
        }
        break;
    case 1:
        saveSort();
        break;
    default:
        break;
    }
}

bool FormOptions::eventFilter(QObject *, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        switch (keyEvent->key()) {
         case Qt::Key_Up:
             if (keyEvent->modifiers() & Qt::ControlModifier &&
                     ui->treeView->hasFocus()) {
                 emit ui->toolButtonUp->clicked();
                 return true;
             }
             break;
         case Qt::Key_Down:
             if (keyEvent->modifiers() & Qt::ControlModifier &&
                     ui->treeView->hasFocus()) {
                 emit ui->toolButtonDown->clicked();
                 return true;
             }
             break;
        case Qt::Key_Insert:
            if (ui->treeView->hasFocus()) {
                emit ui->toolButtonAdd->click();
                return true;
            }
            break;
        case Qt::Key_Delete:
            if (keyEvent->modifiers() & Qt::ControlModifier &&
                    ui->toolButtonDelete->isEnabled() &&
                    ui->treeView->hasFocus()) {
                emit ui->toolButtonDelete->click();
                return true;
            }
            break;
        }
    }

    return false;
}

void FormOptions::initSignals()
{
    if (!mSelectMode) {
        connect(ui->checkBoxIsShowGroups, SIGNAL(clicked(bool)), ui->widgetGroupsOptions, SLOT(setVisible(bool)));
        connect(ui->widgetOptions, SIGNAL(checkedOption()), SLOT(refreshData()));
    }
}

void FormOptions::saveSort()
{
    ApiAdapter::getApi()->sortOptionValues(mModel->getSortParamsForApi());
}

void FormOptions::resetRequest()
{
    mRequest.clear();
    QString filter;
    QStringList filterList;
    // опции
    if (!mSelectMode) {
        if (ui->widgetOptions->getIdsOptionsChecked().size()) {
            QString idsOptions;
            for (int i = 0; i < ui->widgetOptions->getIdsOptionsChecked().size(); i++) {
                if (!idsOptions.isEmpty())
                    idsOptions += ",";
                idsOptions += "'" + ui->widgetOptions->getIdsOptionsChecked().at(i) + "'";
            }
            filterList.append("([idOption] IN (" + idsOptions + "))");
        }
    } else {
        filterList.append("([idOption] = " + mIdOption + ")");
    }
    filter = filterList.join(" AND ");
    mRequest.setFilter(filter);
}

void FormOptions::initTreeView()
{
    mModel->clearFields();
    mModel->appendField(new Field("image", "Фото"));
    mModel->appendField(new Field("id", "#"));
    mModel->appendField(new Field("name", "Наименование"));
    mModel->appendField(new Field("price", "Цена", Qt::AlignRight | Qt::AlignVCenter));
    mModel->appendField(new Field("option", "Опция"));

    ui->treeView->setModel(mModel);

    ui->treeView->header()->setStretchLastSection(true);
    ui->treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);        

    ui->treeView->installEventFilter(this);
}

void FormOptions::initButtonsSignals()
{
    connect(ui->toolButtonAdd, SIGNAL(clicked(bool)), SLOT(onAddEditValue()));
    connect(ui->toolButtonEdit, SIGNAL(clicked(bool)), SLOT(onAddEditValue()));
    connect(ui->toolButtonDelete, SIGNAL(clicked(bool)), SLOT(onDeleteValue()));
    if (mSelectMode)
        connect(ui->treeView, &QTreeView::doubleClicked, ui->toolButtonSelect, &QToolButton::click);
    else connect(ui->treeView, &QTreeView::doubleClicked, ui->toolButtonEdit, &QToolButton::click);
    connect(ui->toolButtonRefresh, SIGNAL(clicked(bool)), SLOT(refreshData()));
    connect(ui->treeView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), SLOT(onChange()));
    connect(ui->toolButtonSelect, SIGNAL(clicked(bool)), SLOT(onSelect()));
    connect(ui->toolButtonDown, SIGNAL(clicked()), SLOT(onUpDownValue()));
    connect(ui->toolButtonUp, SIGNAL(clicked()), SLOT(onUpDownValue()));
}

void FormOptions::restoreStateTable()
{
    mIsRestoreStates = SEConfig::restoreTableState(ui->treeView->header(), ui->treeView->objectName(), mSelectMode);
}

void FormOptions::saveStateTable()
{
    SEConfig::saveTableState(ui->treeView->header(), ui->treeView->objectName(), mSelectMode);
    SEConfig::saveSplitterLayout(ui->splitter, mSelectMode);
}

void FormOptions::goToRowById(QString id)
{
    QModelIndex modelIndex = mModel->getModelIndexById(id);

    if (modelIndex.isValid()) {
        ui->treeView->selectionModel()->setCurrentIndex(
                    modelIndex, QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);
        return;
    }

    if (mModel->rowCount() > 0)
        ui->treeView->selectionModel()->
                setCurrentIndex(mModel->index(0, 0), QItemSelectionModel::SelectCurrent |
                                QItemSelectionModel::Rows);
    else onChange();
    ui->treeView->setFocus();
}
