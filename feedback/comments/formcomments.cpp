#include "formcomments.h"
#include "ui_formcomments.h"
#include "apiadapter.h"
#include "preloader.h"
#include "field.h"

#include <QMenu>

FormComments::FormComments(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormComments)
{
    ui->setupUi(this);
    mComments = 0;
    mIsLocalMode = false;
    initialization();
}

FormComments::FormComments(QWidget *parent, ListDataItems *comments) :
    QWidget(parent),
    ui(new Ui::FormComments)
{
    ui->setupUi(this);
    mComments = comments;
    mIsLocalMode = true;
    initialization();
}

FormComments::~FormComments()
{    
    if (!mIsLocalMode)
        delete mComments;
    delete ui;
}

void FormComments::fillControls()
{    
    mModel->resetModel();
}

void FormComments::clearInfo()
{

}

void FormComments::setIdProduct(const QString &id)
{
    mIdProduct = id;    
}

void FormComments::refreshData()
{
    resetRequest();

    QString id;

    QModelIndex index = ui->tableViewComments->currentIndex();
    if (index.isValid())
        id = mModel->idData(ui->tableViewComments->currentIndex());

    if (!mIsLocalMode) {
        mModel->beginReset();
        ApiAdapter::getApi()->getListComents(mRequest, mComments);
        mModel->endReset();        
    }
    if (!mIsLoadedLayouts) {
        ui->tableViewComments->resizeColumnsToContents();
        ui->tableViewComments->horizontalHeader()->resizeSection(3, 250);
    }
    ui->tableViewComments->resizeRowsToContents();

    goToRowById(id);
}

void FormComments::onAddEditComment()
{
    if (mCardComment)
        delete mCardComment;
    mCardComment = new FormCommentCard(this);
    if(mIsLocalMode)
        mCardComment->setLocalMode();
    mCardComment->clear();
    if (sender() != ui->actionAdd) {
        if (!ui->tableViewComments->selectionModel()->currentIndex().isValid())
            return;
        mCardComment->setComment(mComments->at(ui->tableViewComments->currentIndex().row()));
    }
    if (mCardComment->exec() == QDialog::Accepted) {
          mModel->beginReset();
          if (sender() != ui->actionAdd) {
              mComments->value(ui->tableViewComments->currentIndex().row())->
                      copy(mCardComment->comment());
          } else {
              DataItem *comment = new DataItem(mCardComment->comment());
              mComments->append(comment);
          }
          mModel->endReset();
          emit modified();
          mComments->setModified();
    }
}

void FormComments::onDeleteComment()
{
    if (!ui->tableViewComments->selectionModel()->currentIndex().isValid())
        return;

    QString msg;
    if (ui->tableViewComments->selectionModel()->selectedRows().count() > 1)
        msg = tr("Удалить выбранные комментарии?");
    else msg = tr("Удалить выбранный комментарий?");

    QMessageBox::StandardButton mesRepl;

    if (!mIsLocalMode)
        mesRepl = QMessageBox::question(this, tr("Подтверждение удаления"),
                                        msg, QMessageBox::Yes | QMessageBox::No);
    if(mIsLocalMode ||  (mesRepl == QMessageBox::Yes))    {
        Request request;
        QModelIndexList list = ui->tableViewComments->selectionModel()->selectedRows();
        if (!mIsLocalMode) {
            for (int i = 0; i < list.count(); i++)
                request.addId(mModel->dataItem(list.at(i))->getId());
            ApiAdapter::getApi()->deleteComments(request);
        }
        mModel->beginReset();
        QStringList delList;
        for (int i = 0; i < list.count(); i++)
            delList.append(mModel->dataItem(list.at(i))->getId());
        for (int i = 0; i < mComments->size(); i++)
            if (delList.contains(mComments->at(i)->getId()))
                mComments->at(i)->setIsSelected(true);
        for (int i = mComments->size() - 1; i >= 0; i--)
            if (mComments->at(i)->getIsSelected())
                delete mComments->takeAt(i);
        mModel->endReset();
        emit modified();
        mComments->setModified();
    }
    onChange();
}

void FormComments::onChange()
{
    ui->actionDelete->setEnabled(
                ui->tableViewComments->selectionModel()->currentIndex().isValid());
    ui->actionEdit->setEnabled(
                ui->tableViewComments->selectionModel()->currentIndex().isValid());
}

void FormComments::showContextMenu(QPoint point)
{
    onChange();
    QPoint globalPos;

    if (sender()->inherits("QAbstractScrollArea"))
        globalPos = ((QAbstractScrollArea*)sender())->viewport()->mapToGlobal(point);
    else globalPos = ((QWidget*)sender())->mapToGlobal(point);

    QMenu menu;

    menu.addAction(ui->actionAdd);
    menu.addAction(ui->actionEdit);
    menu.addAction(ui->actionDelete);
    menu.exec(globalPos);
}

void FormComments::showEvent(QShowEvent *)
{
    if (!mIsShowed) {
        int tag = 0;
        if (mIsLocalMode)
            tag = 1;
        mIsLoadedLayouts = SEConfig::restoreTableState(
                    ui->tableViewComments->horizontalHeader(),
                    ui->tableViewComments->objectName(), tag);
        startTimer(1);
        mIsShowed = true;
    }
}

void FormComments::hideEvent(QHideEvent *)
{
    int tag = 0;
    if (mIsLocalMode)
        tag = 1;
    SEConfig::saveTableState(ui->tableViewComments->horizontalHeader(),
                                    ui->tableViewComments->objectName(), tag);
}

void FormComments::timerEvent(QTimerEvent *event)
{
    killTimer(event->timerId());
    refreshData();
}

void FormComments::goToRowById(const QString &id)
{
    for (int i = 0; i < mModel->rowCount(); i ++)
        if (mModel->listData()->at(i)->getId() == id) {
            ui->tableViewComments->selectRow(i);
            return;
        }
    if (mModel->rowCount() > 0)
        ui->tableViewComments->selectRow(0);
    else onChange();
}

void FormComments::initialization()
{
    initVariables();
    initActions();
    initTable();
    initSignals();
}

void FormComments::initActions()
{
    ui->toolButtonAdd->setDefaultAction(ui->actionAdd);
    ui->toolButtonEdit->setDefaultAction(ui->actionEdit);
    ui->toolButtonDelete->setDefaultAction(ui->actionDelete);
    ui->actionEdit->setEnabled(false);
    ui->actionDelete->setEnabled(false);
}

void FormComments::initVariables()
{
    if (!mComments)
        mComments = new ListDataItems();
    mModel = new NameTableModel(this, mComments);
    mModel->clearFields();
    mModel->appendField(new Field("dateComment", "Дата"));
    mModel->appendField(new Field("contactTitle", "Имя пользователя"));
    if (!mIsLocalMode)
        mModel->appendField(new Field("nameProduct", "Наименование товара"));
    mModel->appendField(new Field("commentary", "Комментарий"));
    mModel->appendField(new Field("response", "Ответ администратора"));
    ui->tableViewComments->setModel(mModel);
    mIsShowed = false;
    mCardComment = 0;   
}

void FormComments::initTable()
{    
    ui->tableViewComments->horizontalHeader()->setStretchLastSection(true);
    ui->tableViewComments->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableViewComments->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->tableViewComments->setContextMenuPolicy(Qt::CustomContextMenu);
}

void FormComments::initSignals()
{
    connect(ui->tableViewComments->selectionModel(),
            SIGNAL(currentChanged(QModelIndex,QModelIndex)), SLOT(onChange()));
    connect(ui->tableViewComments, SIGNAL(customContextMenuRequested(QPoint)),
            SLOT(showContextMenu(QPoint)));
    connect(ui->toolButtonRefresh, SIGNAL(clicked()), SLOT(refreshData()));
    connect(ui->actionAdd, SIGNAL(triggered()), SLOT(onAddEditComment()));
    connect(ui->actionEdit, SIGNAL(triggered()), SLOT(onAddEditComment()));
    connect(ui->actionDelete, SIGNAL(triggered()), SLOT(onDeleteComment()));
    connect(ui->tableViewComments, SIGNAL(doubleClicked(QModelIndex)), SLOT(onAddEditComment()));
}

void FormComments::resetRequest()
{
    mRequest.setSortBy("id");
    mRequest.setAscendingOrder(false);
}


