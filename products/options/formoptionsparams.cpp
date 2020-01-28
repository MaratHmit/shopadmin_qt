#include "formoptionsparams.h"
#include "ui_formoptionsparams.h"
#include "formoptioncart.h"

#include <QKeyEvent>

FormOptionsParams::FormOptionsParams(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormOptionsParams)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Widget);

    mSelectMode = false;
    mIsShowed = false;
    mIsInitSignals = false;
    ui->toolButtonSelect->setHidden(true);

    initialization();
}

FormOptionsParams::FormOptionsParams(QWidget *parent, const bool &selectMode) :
     QDialog(parent),
     ui(new Ui::FormOptionsParams)
{
    ui->setupUi(this);

    mIsShowed = false;
    mIsInitSignals = false;

    mSelectMode = selectMode;
    ui->toolButtonSelect->show();
    ui->toolButtonDelOption->hide();
    ui->toolButtonOptionUp->hide();
    ui->toolButtonOptionDown->hide();

    initialization();
}

FormOptionsParams::~FormOptionsParams()
{
    delete ui;
    delete mListSelected;
    delete mExludingOptions;
}

QString FormOptionsParams::currentId() const
{
    QModelIndex index = ui->treeView->currentIndex();
    if (index.isValid())
        return mModel->idData(index);
    else return 0;
}

const ListOptions *FormOptionsParams::getListOptions()
{
    return mOptions;
}

QStringList FormOptionsParams::getIdsOptionsChecked()
{
     QStringList list;

     QModelIndexList listRows = ui->treeView->selectionModel()->selectedRows(0);

     for (int i = 0; i < listRows.count(); i++)
         list.append(mModel->idData(listRows.at(i)));

     return list;
}

void FormOptionsParams::setExludingOptions(const ListOptions *options)
{
    for (int i = 0; i < options->size(); i++)
        mExludingOptions->append(new DataOption(options->at(i)));
}

ListOptions *FormOptionsParams::selectedList() const
{
    mListSelected->clear();
    QModelIndexList list =
            ui->treeView->selectionModel()->selectedRows();
    for (int i = 0; i < list.size(); i++) {
        DataOption *item = dynamic_cast <DataOption *> (mModel->dataItem(list.at(i)));
        if (item)
            mListSelected->append(item);
    }
    return mListSelected;
}

void FormOptionsParams::refreshData()
{
    setCursor(Qt::WaitCursor);
    QString id = currentId();

    mModel->beginReset();
    ApiAdapter::getApi()->getListOptions(mOptions);
    for (int i = 0; i < mExludingOptions->size(); i++)
        for (int j = 0; j < mOptions->size(); j++)
            if (mOptions->at(j)->getId() == mExludingOptions->at(i)->getId()) {
                mOptions->deleteItem(mOptions->at(j));
                break;
            }

    mModel->endReset();

    goToRowById(id);
    ui->treeView->setFocus();
    setCursor(Qt::ArrowCursor);
}

void FormOptionsParams::onAddEditOption()
{
    int isNew = false;

    QToolButton *button = qobject_cast<QToolButton *> (sender());
    isNew = button == ui->toolButtonAddOption;

    FormOptionCart *card = new FormOptionCart(this);
    QString id = currentId();
    if (!id.isEmpty() && !isNew)
        card->setId(id);
    card->show();

    connect(card, SIGNAL(accepted()), SLOT(refreshData()));
}

void FormOptionsParams::onUpDownOption()
{
    if (mIdTimer)
        killTimer(mIdTimer);

    QToolButton *button = qobject_cast<QToolButton *> (sender());
    bool isMoveUp = button == ui->toolButtonOptionUp;

    QString id = currentId();
    mModel->beginReset();
    if (isMoveUp)
        mOptions->moveUp(mModel->dataItem((ui->treeView->selectionModel()->currentIndex())));
    else mOptions->moveDown(mModel->dataItem((ui->treeView->selectionModel()->currentIndex())));
    mModel->endReset();

    goToRowById(id);
    mTypeTimer = 1;
    mIdTimer = startTimer(1000);
}

void FormOptionsParams::onChange()
{
    QString id = currentId();

    ui->toolButtonEditOption->setEnabled(!id.isEmpty());
    ui->toolButtonDelOption->setEnabled(!id.isEmpty());
    ui->toolButtonOptionUp->setEnabled(!id.isEmpty() && ui->treeView->selectionModel()->currentIndex().row());
    ui->toolButtonOptionDown->setEnabled(!id.isEmpty() && mModel->rowCount() - 1 -
                                         ui->treeView->selectionModel()->currentIndex().row());
    emit checkedOption();
}

void FormOptionsParams::onSelect()
{
    accept();
}

void FormOptionsParams::onChecked()
{
    setCursor(Qt::WaitCursor);
    emit checkedOption();
    setCursor(Qt::ArrowCursor);
}

void FormOptionsParams::showEvent(QShowEvent *)
{
    initButtonsSignals();
    refreshData();
}

void FormOptionsParams::timerEvent(QTimerEvent *event)
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

bool FormOptionsParams::eventFilter(QObject *, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        switch (keyEvent->key()) {
         case Qt::Key_Up:
             if (keyEvent->modifiers() & Qt::ControlModifier &&
                     ui->treeView->hasFocus()) {
                 emit ui->toolButtonOptionUp->clicked();
                 return true;
             }
             break;
         case Qt::Key_Down:
             if (keyEvent->modifiers() & Qt::ControlModifier &&
                     ui->treeView->hasFocus()) {
                 emit ui->toolButtonOptionDown->clicked();
                 return true;
             }
             break;
        case Qt::Key_Insert:
            if (ui->treeView->hasFocus()) {
                emit ui->toolButtonAddOption->click();
                return true;
            }
            break;
        case Qt::Key_Delete:
            if (keyEvent->modifiers() & Qt::ControlModifier &&
                    ui->toolButtonDelOption->isEnabled() &&
                    ui->treeView->hasFocus()) {
                emit ui->toolButtonDelOption->click();
                return true;
            }
            break;
        }
    }

    return false;
}

void FormOptionsParams::saveSort()
{
    ApiAdapter::getApi()->sortOption(mModel->getSortParamsForApi());
}

void FormOptionsParams::initTreeView()
{
    //mModel->appendField(new Field("note", "Метка"));

    ui->treeView->setModel(mModel);
    //mModel->setCheckedMode(true);

    ui->treeView->header()->setStretchLastSection(true);
    ui->treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);

    connect(ui->treeView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), SLOT(onChange()));
    if (!mSelectMode)
        connect(mModel, SIGNAL(checked(QString,bool)), SLOT(onChecked()));

    ui->treeView->installEventFilter(this);
}

void FormOptionsParams::initButtonsSignals()
{
    if (mIsInitSignals)
        return;

    connect(ui->toolButtonAddOption, SIGNAL(clicked()), SLOT(onAddEditOption()));
    connect(ui->toolButtonEditOption, SIGNAL(clicked()), SLOT(onAddEditOption()));
    if (!mSelectMode)
        connect(ui->treeView, SIGNAL(doubleClicked(QModelIndex)), SLOT(onAddEditOption()));
    else connect(ui->treeView, SIGNAL(doubleClicked(QModelIndex)), SLOT(onSelect()));
    connect(ui->toolButtonOptionDown, SIGNAL(clicked()), SLOT(onUpDownOption()));
    connect(ui->toolButtonOptionUp, SIGNAL(clicked()), SLOT(onUpDownOption()));
    connect(ui->toolButtonRefresh, SIGNAL(clicked()), SLOT(refreshData()));
    connect(ui->toolButtonSelect, SIGNAL(clicked(bool)), SLOT(onSelect()));
    mIsInitSignals = true;
}

void FormOptionsParams::initialization()
{
    mOptions = new ListOptions();
    mModel = new NameTableModel(this, reinterpret_cast <ListDataItems *> (mOptions));

    mTypeTimer = 0;
    mIdTimer = 0;
    mListSelected = new ListOptions;
    mExludingOptions = new ListOptions;
    mIsShowed = false;

    initTreeView();
}

void FormOptionsParams::goToRowById(QString id)
{
    QModelIndex modelIndex = mModel->getModelIndexById(id);

    if (modelIndex.isValid()) {
        ui->treeView->selectionModel()->setCurrentIndex(
                    modelIndex, QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);
        return;
    }

    if (mModel->rowCount() > 0)
        ui->treeView->selectionModel()->
                setCurrentIndex(mModel->index(0, 0),
                                QItemSelectionModel::SelectCurrent |
                                QItemSelectionModel::Rows);
    else onChange();
}

void FormOptionsParams::on_toolButtonDelOption_clicked()
{
    QString msg;
    if (ui->treeView->selectionModel()->selectedRows().count() > 1)
        msg = tr("Удалить выбранные опции?");
    else msg = tr("Удалить опцию") + ": " +
            mModel->nameData(ui->treeView->currentIndex()) + "?";

    QMessageBox::StandardButton mesRepl;

    mesRepl = QMessageBox::question(this, tr("Подтверждение удаления"),
                                    msg, QMessageBox::Yes | QMessageBox::No);
    if(mesRepl == QMessageBox::Yes)    {
        Request request;
        QModelIndexList list = ui->treeView->selectionModel()->selectedRows(0);

        for (int i = 0; i < list.count(); i++)
            request.addId(mModel->idData(list.at(i)));
        ApiAdapter::getApi()->deleteOptions(request);
        refreshData();
    }
}
