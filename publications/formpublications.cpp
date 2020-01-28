#include "formpublications.h"
#include "ui_formpublications.h"
#include "apiadapter.h"
#include "seconfig.h"
#include "seconsts.h"
#include "userpermission.h"

#include <QMenu>

FormPublications::FormPublications(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormPublications)
{
    ui->setupUi(this);
    initFields();
    initNavigator();
    initVariables();
    initActions();
    initSignals();
    initPermissions();
}

void FormPublications::setSelectMode(const bool &isMultiSelect)
{
    if (!isMultiSelect)
         ui->treeViewNews->setSelectionMode(QAbstractItemView::SingleSelection);
    mIsSelectMode = true;
    ui->actionDel->setVisible(false);
    ui->toolButtonDelete->hide();
    ui->toolButtonSelect->show();
    ui->webView->hide();
    ui->actionEdit->disconnect();
    if (mIsSelectMode)
        connect(ui->treeViewNews, SIGNAL(doubleClicked(QModelIndex)), ui->toolButtonSelect, SLOT(click()));
}

ListNews *FormPublications::selectedNews() const
{
    return mSelectedNews;
}

FormPublications::~FormPublications()
{
    saveSateControls();
    delete ui;    
    delete mSelectedNews;
}

void FormPublications::showContextMenu(QPoint point)
{
    onChange();

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

void FormPublications::onClickHeaderItem(int )
{
    refreshData();
}

void FormPublications::onChange()
{
    if (mModel->rowCount() > 0 &&
            ui->treeViewNews->currentIndex().isValid())
        mNavigator->setNumRecord(mNumPage * mNavigator->getLimit() +
                                 ui->treeViewNews->currentIndex().row() + 1,
                                 mCountNews);
    else mNavigator->setNumRecord(0, 0);
    showCurrentDescription();
    checkEnabled();
}

void FormPublications::onAddEditNews()
{
    bool isNew = sender() == ui->actionAdd;
    if (!isNew && !ui->treeViewNews->currentIndex().isValid())
        return;

    FormPublicationCard cardNews(this);
    if (!isNew) {
        DataNew *currentNew = dynamic_cast <DataNew *> (mModel->dataItem(ui->treeViewNews->currentIndex()));
        cardNews.setWindowTitle(tr("Редактирование новости"));
        cardNews.setNew(currentNew);
    }        
    if (cardNews.exec() == QDialog::Accepted)
        refreshData();
}

void FormPublications::onDeleteNews()
{
    if (!ui->treeViewNews->currentIndex().isValid())
        return;

    QString msg;
    if (ui->treeViewNews->selectionModel()->selectedRows().count() > 1)
        msg = tr("Удалить выбранные новости?");
    else msg = tr("Удалить новость") + ": " +
            mModel->nameData(ui->treeViewNews->currentIndex()) + "?";

    QMessageBox::StandardButton mesRepl;

    mesRepl = QMessageBox::question(this, tr("Подтверждение удаления"),
                                    msg, QMessageBox::Yes | QMessageBox::No);
    if(mesRepl == QMessageBox::Yes)    {
        Request request;
        QModelIndexList list = ui->treeViewNews->selectionModel()->selectedRows();

        for (int i = 0; i < list.count(); i++) {
            request.addId(mModel->getItems()->at(list.at(i).row())->getId());
            mModel->getItems()->at(list.at(i).row())->setIsSelected(true);
        }
        ApiAdapter::getApi()->deleteNews(request);
        for (int i = mModel->getItems()->size() - 1; i >= 0; i--)
            if (mModel->getItems()->at(i)->getIsSelected())
                delete mModel->getItems()->takeAt(i);
        mModel->resetModel();
    }
}

void FormPublications::goNextPage()
{
    mNumPage++;
    if (mNumPage > int(mCountNews / ui->widgetNavigator->getLimit()))
        mNumPage = int(mCountNews / ui->widgetNavigator->getLimit());
    refreshData();
}

void FormPublications::goPreviosPage()
{
    mNumPage--;
    if (mNumPage < 0)
        mNumPage = 0;
    refreshData();
}

void FormPublications::goLastPage()
{
    mNumPage = int(mCountNews / ui->widgetNavigator->getLimit());
    refreshData();
}

void FormPublications::goFirstPage()
{
    mNumPage = 0;
    refreshData();
}

void FormPublications::onRunSearch()
{

}

void FormPublications::showEvent(QShowEvent *)
{
    if (!mIsShowed) {
        restoreStateControls();
        startTimer(1);
    }
}

void FormPublications::timerEvent(QTimerEvent *e)
{
    killTimer(e->timerId());
    refreshData();
}

void FormPublications::accept()
{
    QModelIndexList list = ui->treeViewNews->selectionModel()->selectedRows();
    mSelectedNews->clear();
    for (int i = 0; i < list.size(); i++)
        mSelectedNews->append(mNews->at(list.at(i).row()));
    QDialog::accept();
}

void FormPublications::goToRowById(const QString id)
{
    if (!id.isEmpty()) {
            for (int i = 0; i < mNews->size(); i ++)
                if (mNews->at(i)->getId() == id) {
                    ui->treeViewNews->selectionModel()->setCurrentIndex(
                                mModel->index(i, 0), QItemSelectionModel::SelectCurrent |
                                QItemSelectionModel::Rows);
                    ui->treeViewNews->setFocus();
                    return;
                }
    }
    if (mNews->size())
        ui->treeViewNews->selectionModel()->setCurrentIndex(
                    mModel->index(0, 0), QItemSelectionModel::SelectCurrent |
                    QItemSelectionModel::Rows);
    else onChange();
    ui->treeViewNews->setFocus();
}

void FormPublications::resetRequest()
{
    mRequest.clear();
    mRequest.setLimit(mNavigator->getLimit());
    mRequest.setOffset(mNumPage * mNavigator->getLimit());

    mRequest.clearFilter();
    if (ui->widgetGroupsNews->getIdsGroupsChecked().size()) {
        QString idsGroups;
        for (int i = 0; i < ui->widgetGroupsNews->getIdsGroupsChecked().size(); i++) {
            if (!idsGroups.isEmpty())
                idsGroups += ",";
            idsGroups += "'" + ui->widgetGroupsNews->getIdsGroupsChecked().at(i) + "'";
        }
        QString filter = "idGroup in (" +
                idsGroups + ")";
        mRequest.setFilter(filter);
    }


    QStringList lst;
    foreach (Field *field, mFields)
        lst.append(field->name());
    QString sortField;
    sortField = lst[ui->treeViewNews->header()->sortIndicatorSection()];
    mRequest.setSortBy(sortField);
    mRequest.setSearchText(ui->widgetSearch->searchText().trimmed());
    mRequest.setAscendingOrder(ui->treeViewNews->header()->
                               sortIndicatorOrder() == Qt::AscendingOrder);
}

void FormPublications::checkEnabled()
{
    ui->actionEdit->setEnabled(ui->treeViewNews->currentIndex().isValid() && isEditPermission);
    ui->actionDel->setEnabled(ui->treeViewNews->currentIndex().isValid() && isDeletePermission);
    ui->toolButtonSelect->setEnabled(ui->treeViewNews->currentIndex().isValid());
}

void FormPublications::initFields()
{
    mFields.append(new Field("image", "Фото"));
    mFields.append(new Field("newsDate", "Дата"));
    mFields.append(new Field("display", "Заголовок"));
}

void FormPublications::initVariables()
{
    mCountNews = 0;
    mNumPage = 0;
    mIsShowed = false;
    mIsRestoreStates = false;
    mIsSelectMode = false;
    mPreloader = new Preloader(ui->treeViewNews);
    mNews = new ListNews();
    mSelectedNews = new ListNews();
    ui->toolButtonSelect->hide();
    mModel = new NameTableModel(this, reinterpret_cast <ListDataItems *> (mNews));        
    mModel->clearFields();    
    foreach (Field *field, mFields)
        mModel->appendField(field);
    ui->treeViewNews->setModel(mModel);
    ui->treeViewNews->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->treeViewNews->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->treeViewNews->setContextMenuPolicy(Qt::CustomContextMenu);
}

void FormPublications::initActions()
{
    ui->toolButtonAdd->setDefaultAction(ui->actionAdd);
    ui->toolButtonEdit->setDefaultAction(ui->actionEdit);
    ui->toolButtonDelete->setDefaultAction(ui->actionDel);
}

void FormPublications::initSignals()
{
    connect(ui->treeViewNews, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(showContextMenu(QPoint)));
    connect(ui->treeViewNews->selectionModel(),
            SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), SLOT(onChange()));
    connect(ui->treeViewNews->header(), SIGNAL(sectionClicked(int)), SLOT(onClickHeaderItem(int)));
    connect(ui->toolButtonRefresh, &QToolButton::clicked, this, &FormPublications::refreshData);
    connect(ui->actionAdd, &QAction::triggered, this, &FormPublications::onAddEditNews);
    connect(ui->actionDel, &QAction::triggered, this, &FormPublications::onDeleteNews);
    connect(ui->treeViewNews, SIGNAL(doubleClicked(QModelIndex)), ui->actionEdit, SLOT(trigger()));
    connect(ui->toolButtonSelect, SIGNAL(clicked(bool)), SLOT(accept()));
    connect(ui->actionEdit, &QAction::triggered, this, &FormPublications::onAddEditNews);
}

void FormPublications::initNavigator()
{
    mNavigator = ui->widgetNavigator;
    mNavigator->setLimit(20);

    connect(mNavigator, SIGNAL(goNextPage()), SLOT(goNextPage()));
    connect(mNavigator, SIGNAL(goPreviosPage()), SLOT(goPreviosPage()));
    connect(mNavigator, SIGNAL(goLastPage()), SLOT(goLastPage()));
    connect(mNavigator, SIGNAL(goFirstPage()), SLOT(goFirstPage()));
    connect(mNavigator, SIGNAL(refreshDataStart()), SLOT(goFirstPage()));
    connect(ui->widgetSearch, SIGNAL(runSearch()), SLOT(onRunSearch()));
}

void FormPublications::initPermissions()
{
    isAddPermission = UserPermission::isAdd(UserPermission::NEWS);
    isEditPermission = UserPermission::isEdit(UserPermission::NEWS);
    isDeletePermission = UserPermission::isDelete(UserPermission::NEWS);

    ui->actionAdd->setEnabled(isAddPermission);
}

void FormPublications::saveSateControls()
{
    if (mIsShowed) {
        SEConfig::saveSplitterLayout(ui->splitterMainNews, mIsSelectMode);
        SEConfig::saveSplitterLayout(ui->splitterTableNews, mIsSelectMode);
        SEConfig::saveTableState(ui->treeViewNews->header(), ui->treeViewNews->objectName(), mIsSelectMode);
    }
}

void FormPublications::restoreStateControls()
{
    SEConfig::loadSplitterLayout(ui->splitterMainNews, mIsSelectMode);
    SEConfig::loadSplitterLayout(ui->splitterTableNews, mIsSelectMode);
    mIsRestoreStates = SEConfig::restoreTableState(ui->treeViewNews->header(), ui->treeViewNews->objectName(), mIsSelectMode);
}

void FormPublications::refreshData()
{    
    resetRequest();
    QString id;

    QModelIndex index = ui->treeViewNews->currentIndex();
    if (index.isValid())
        id = mModel->idData(ui->treeViewNews->currentIndex());

    mPreloader->startAnimation();
    if (ApiAdapter::getApi()->getListNews(mRequest, mNews)) {
        mCountNews = mNews->countInBase();
        mModel->resetModel();
        mIsShowed = true;
        if (!mIsRestoreStates)
            for (int i = 0; i < mModel->columnCount(); ++i)
                ui->treeViewNews->resizeColumnToContents(i);
    }        
    mPreloader->stopAnimation();
    goToRowById(id);
    connect(ui->widgetGroupsNews, &FormNewsGroups::onItemTreeChanged, this,
            &FormPublications::refreshData, Qt::UniqueConnection);
}

void FormPublications::showCurrentDescription()
{
    if (!ui->treeViewNews->currentIndex().isValid())
        return;

    setCursor(Qt::WaitCursor);
    DataNew *news = mNews->at(ui->treeViewNews->currentIndex().row());
    ui->webView->setHtml(setLocalURLImage(news->property("fullDescription").toString()));
    ui->treeViewNews->setFocus();
    setCursor(Qt::ArrowCursor);
}

QString FormPublications::setLocalURLImage(const QString &text)
{
    QString s = text;
    QRegExp exp;
    exp.setMinimal(true);
    exp.setPattern("(<img[^>]*src=\").*([^/]*\")");
    s.replace(exp, "\\1" + SEConfig::getUrlImages(SEConsts::Section_News) + "/" + "\\2");

    return s;
}

