#include "formreviews.h"
#include "ui_formreviews.h"
#include "apiadapter.h"
#include "seconfig.h"
#include "userpermission.h"

#include <QApplication>
#include <QClipboard>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPoint>
#include <QDebug>

FormReviews::FormReviews(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormReviews)
{
    ui->setupUi(this);

    initVariables();
    initSignals();
    initPermissions();
}

FormReviews::~FormReviews()
{
    if (mIsShowed)
        SEConfig::saveTableState(ui->tableViewReviews->horizontalHeader(), ui->tableViewReviews->objectName());
    delete ui;
}

void FormReviews::showContextMenu(QPoint point)
{
    onChangeCurrentItem();
    QPoint globalPos;

    if (sender()->inherits("QAbstractScrollArea"))
        globalPos = ((QAbstractScrollArea*)sender())->viewport()->mapToGlobal(point);
    else globalPos = ((QWidget*)sender())->mapToGlobal(point);

    QMenu menu;
    menu.addAction(ui->actionAdd);
    menu.addAction(ui->actionEdit);
    menu.addAction(ui->actionDel);
    menu.exec(globalPos);
}

void FormReviews::onChangeCurrentItem()
{
    if (ui->tableViewReviews->currentIndex().isValid())
        ui->widgetNavigator->setNumRecord(mNumPage * ui->widgetNavigator->getLimit() +
                                          ui->tableViewReviews->currentIndex().row() + 1,
                                          mCountReviews);
    else ui->widgetNavigator->setNumRecord(0, 0);

    ui->actionEdit->setEnabled(ui->tableViewReviews->currentIndex().isValid() &&
                               isEditPermission);
    ui->actionDel->setEnabled(ui->tableViewReviews->currentIndex().isValid() &&
                              isDeletePermission);
}

void FormReviews::onRunSearch()
{
    refreshData();
    ui->tableViewReviews->setFocus();
}

void FormReviews::showEvent(QShowEvent *)
{
    emit afterShow();
}

void FormReviews::initVariables()
{
    mNumPage = 0;
    mCountReviews = 0;
    mIsLoadedLayouts = false;
    mPreloader = new Preloader(this);
    mListReviews = new ListReviews;
    mModel = new NameTableModel(this, reinterpret_cast<ListDataItems *> (mListReviews));    
    mModel->clearFields();
    ui->tableViewReviews->setModel(mModel);
    ui->tableViewReviews->setContextMenuPolicy(Qt::CustomContextMenu);
    initFields();
    initActions();
}

void FormReviews::initPermissions()
{
    isAddPermission = UserPermission::isAdd(UserPermission::REVIEWS);
    isEditPermission = UserPermission::isEdit(UserPermission::REVIEWS);
    isDeletePermission = UserPermission::isDelete(UserPermission::REVIEWS);

    ui->actionAdd->setEnabled(isAddPermission);
}

void FormReviews::goToRowById(const QString &id)
{
    if (!id.isEmpty()) {
            for (int i = 0; i < mListReviews->size(); i ++)
                if (mListReviews->at(i)->getId() == id) {
                    ui->tableViewReviews->selectionModel()->setCurrentIndex(
                                mModel->index(i, 0), QItemSelectionModel::SelectCurrent |
                                QItemSelectionModel::Rows);
                    ui->tableViewReviews->setFocus();
                    return;
                }
    }
    if (mListReviews->size())
        ui->tableViewReviews->selectionModel()->setCurrentIndex(
                    mModel->index(0, 0), QItemSelectionModel::SelectCurrent |
                    QItemSelectionModel::Rows);
    else onChangeCurrentItem();
    ui->tableViewReviews->setFocus();
}

void FormReviews::initSignals()
{
    connect(this, SIGNAL(afterShow()), SLOT(refreshData()));
    connect(ui->tableViewReviews, SIGNAL(customContextMenuRequested(QPoint)),
            SLOT(showContextMenu(QPoint)));
    connect(ui->tableViewReviews->horizontalHeader(), SIGNAL(sectionClicked(int)),
            SLOT(refreshData()));
    connect(ui->tableViewReviews->selectionModel(),
            SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), SLOT(onChangeCurrentItem()));
    connect(ui->widgetSearch, SIGNAL(runSearch()), SLOT(onRunSearch()));
    connect(ui->tableViewReviews, SIGNAL(doubleClicked(QModelIndex)), ui->actionEdit, SLOT(trigger()));

    connect(ui->widgetNavigator, SIGNAL(goNextPage()), SLOT(goNextPage()));
    connect(ui->widgetNavigator, SIGNAL(goPreviosPage()), SLOT(goPreviosPage()));
    connect(ui->widgetNavigator, SIGNAL(goLastPage()), SLOT(goLastPage()));
    connect(ui->widgetNavigator, SIGNAL(goFirstPage()), SLOT(goFirstPage()));
    connect(ui->widgetNavigator, SIGNAL(refreshDataStart()), SLOT(goFirstPage()));

    connect(ui->actionAdd, SIGNAL(triggered()), SLOT(addEditReview()));
    connect(ui->actionEdit, SIGNAL(triggered()), SLOT(addEditReview()));
    connect(ui->actionDel, SIGNAL(triggered()), SLOT(deleteReviews()));
    connect(ui->actionRefresh, SIGNAL(triggered()), SLOT(refreshData()));
}

void FormReviews::initFields()
{
    mModel->appendField(new Field("id", "Ид.", Qt::AlignRight | Qt::AlignVCenter));
    mModel->appendField(new Field("isActiveIco", "А", Qt::AlignCenter | Qt::AlignVCenter, Qt::DecorationRole));
    mModel->appendField(new Field("dateTime", "Дата отзывы"));
    mModel->appendField(new Field("nameProduct", "Наименование товара"));
    mModel->appendField(new Field("nameUser", "Пользователь"));
    mModel->appendField(new Field("mark", "Оценка",  Qt::AlignCenter | Qt::AlignVCenter, Qt::DecorationRole));
    mModel->appendField(new Field("useTime", "Опыт использования"));
    mModel->appendField(new Field("countLikes", "Лайков", Qt::AlignRight | Qt::AlignVCenter));
    mModel->appendField(new Field("countDislikes", "Дислайков", Qt::AlignRight | Qt::AlignVCenter));
    mModel->appendField(new Field("merits", "Достоинства"));
    mModel->appendField(new Field("demerits", "Недостатки"));
    mModel->appendField(new Field("comment", "Комментарий"));
}

void FormReviews::initActions()
{
    ui->toolButtonAdd->setDefaultAction(ui->actionAdd);
    ui->toolButtonEdit->setDefaultAction(ui->actionEdit);
    ui->toolButtonDelete->setDefaultAction(ui->actionDel);
    ui->toolButtonRefresh->setDefaultAction(ui->actionRefresh);
}

void FormReviews::refreshData()
{
    setCursor(QCursor(Qt::WaitCursor));
    resetRequest();

    QString id;

    QModelIndex index = ui->tableViewReviews->currentIndex();
    if (index.isValid())
        id = mModel->idData(ui->tableViewReviews->currentIndex());

    mPreloader->startAnimation();
    if (ApiAdapter::getApi()->getListReviews(mRequest, mListReviews)) {
        mIsShowed = true;
        mCountReviews = mListReviews->countInBase();
        mModel->resetModel();
        if (!mIsLoadedLayouts)
            mIsLoadedLayouts =
                    SEConfig::restoreTableState(ui->tableViewReviews->horizontalHeader(),
                                                ui->tableViewReviews->objectName());
        if (!mIsLoadedLayouts)            
                ui->tableViewReviews->resizeColumnsToContents();
        ui->tableViewReviews->resizeRowsToContents();
        mIsLoadedLayouts = true;
        goToRowById(id);
    }
    mPreloader->stopAnimation();
    setCursor(QCursor(Qt::ArrowCursor));
}

void FormReviews::goNextPage()
{
    mNumPage++;
    if (mNumPage > int(mCountReviews / ui->widgetNavigator->getLimit()))
        mNumPage = int(mCountReviews / ui->widgetNavigator->getLimit());
    refreshData();
}

void FormReviews::goPreviosPage()
{
    mNumPage--;
    if (mNumPage < 0)
        mNumPage = 0;
    refreshData();
}

void FormReviews::goLastPage()
{
    mNumPage = int(mCountReviews / ui->widgetNavigator->getLimit());
    refreshData();
}

void FormReviews::goFirstPage()
{
    mNumPage = 0;
    refreshData();
}

void FormReviews::addEditReview()
{
    if (sender() == ui->actionEdit && (!ui->tableViewReviews->currentIndex().isValid() ||
                                       !isEditPermission))
        return;

    FormReviewCard card(this);

    if (sender() == ui->actionEdit && ui->tableViewReviews->currentIndex().isValid())
        card.setReview(dynamic_cast<DataReview*> (mModel->dataItem(ui->tableViewReviews->currentIndex())));

    if (card.exec() == QDialog::Accepted)
        refreshData();
}

void FormReviews::deleteReviews()
{
    QString msg;
    if (ui->tableViewReviews->selectionModel()->selectedRows().count() > 1)
        msg = tr("Удалить выбранные отзывы?");
    else msg = tr("Удалить выбранный отзыв?");

    QMessageBox::StandardButton mesRepl;

    mesRepl = QMessageBox::question(this, tr("Подтверждение удаления"),
                                    msg, QMessageBox::Yes | QMessageBox::No);
    if(mesRepl == QMessageBox::Yes)
    {
        Request request;

        QModelIndexList list = ui->tableViewReviews->selectionModel()->selectedRows();

        for (int i = 0; i < list.count(); i++)
                request.addId(mListReviews->at(list.at(i).row())->getId());

        if (list.count()) {
            ApiAdapter::getApi()->deleteReviews(request);
            refreshData();
        }
    }
}

void FormReviews::resetRequest()
{
    mRequest.clear();
    mRequest.setOffset(mNumPage * ui->widgetNavigator->getLimit());
    mRequest.setLimit(ui->widgetNavigator->getLimit());

    QString sortField = mModel->getFields().at(ui->tableViewReviews->horizontalHeader()->sortIndicatorSection())->name();
    mRequest.setSortBy(sortField);
    mRequest.setSearchText(ui->widgetSearch->searchText().trimmed());
    mRequest.setAscendingOrder(ui->tableViewReviews->horizontalHeader()->sortIndicatorOrder() == Qt::AscendingOrder);
}
