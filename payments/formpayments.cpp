#include "formpayments.h"
#include "ui_formpayments.h"
#include "apiadapter.h"
#include "seconfig.h"
#include "formpaymentcard.h"

#include <QMenu>

FormPayments::FormPayments(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormPayments)
{
    ui->setupUi(this);
    initVariables();
    initActions();
    initSignals();
}

FormPayments::~FormPayments()
{
    delete ui;
}

void FormPayments::goNextPage()
{
    mNumPage++;
    if (mNumPage > int(mCountPayemnts / mNavigator->getLimit()))
        mNumPage = int(mCountPayemnts / mNavigator->getLimit());
    refreshData();
}

void FormPayments::goPreviosPage()
{
    mNumPage--;
    if (mNumPage < 0)
        mNumPage = 0;
    refreshData();
}

void FormPayments::goLastPage()
{
    mNumPage = int(mCountPayemnts / mNavigator->getLimit());
    refreshData();
}

void FormPayments::goFirstPage()
{
    mNumPage = 0;
    refreshData();
}

void FormPayments::onChangePayment()
{
    if (mModel->rowCount() > 0 &&
            ui->treeViewPayments->currentIndex().isValid())
        mNavigator->setNumRecord(mNumPage * mNavigator->getLimit() +
                                 ui->treeViewPayments->currentIndex().row() + 1,
                                 mCountPayemnts);
    else mNavigator->setNumRecord(0, 0);
    checkEnabled();
}

void FormPayments::onRunSearch()
{
    refreshData();
    ui->treeViewPayments->setFocus();
}

void FormPayments::onClickHeaderItem(int )
{
    refreshData();
}

void FormPayments::checkEnabled()
{
    ui->actionEdit->setEnabled(ui->treeViewPayments->currentIndex().isValid());
    ui->actionDelete->setEnabled(ui->treeViewPayments->currentIndex().isValid());
}

void FormPayments::goToRowById(QString id)
{
    if (!id.isEmpty()) {
            for (int i = 0; i < mPayments->size(); i ++)
                if (mPayments->at(i)->getId() == id) {
                    ui->treeViewPayments->selectionModel()->setCurrentIndex(
                                mModel->index(i, 0), QItemSelectionModel::SelectCurrent |
                                QItemSelectionModel::Rows);
                    ui->treeViewPayments->setFocus();
                    return;
                }
    }
    if (mPayments->size())
        ui->treeViewPayments->selectionModel()->setCurrentIndex(
                    mModel->index(0, 0), QItemSelectionModel::SelectCurrent |
                    QItemSelectionModel::Rows);
    else onChangePayment();
    ui->treeViewPayments->setFocus();
}

void FormPayments::initActions()
{
    ui->toolButtonAdd->setDefaultAction(ui->actionAdd);
    ui->toolButtonEdit->setDefaultAction(ui->actionEdit);
    ui->toolButtonDelete->setDefaultAction(ui->actionDelete);
}

void FormPayments::showEvent(QShowEvent *)
{
    mIsLoadedLayouts = SEConfig::restoreTableState(ui->treeViewPayments->header(),
                                                   ui->treeViewPayments->objectName());
    startTimer(1);
}

void FormPayments::timerEvent(QTimerEvent *event)
{
    killTimer(event->timerId());

    if (!mIsShowed) {
        refreshData();
        mIsShowed = true;
    }
}

void FormPayments::hideEvent(QHideEvent *)
{
    SEConfig::saveTableState(ui->treeViewPayments->header(),
                             ui->treeViewPayments->objectName());
}

void FormPayments::addEditPayment()
{
    bool isNew = sender() == ui->actionAdd;

    FormPaymentCard *card = 0;
    setCursor(Qt::WaitCursor);
    if (isNew)
        card = new FormPaymentCard(this);
    else {
        DataPayment *payment = reinterpret_cast <DataPayment *>
                (mModel->dataItem(ui->treeViewPayments->currentIndex()));
        card = new FormPaymentCard(this, payment);
    }
    if (card->exec() == QDialog::Accepted)
        refreshData();
    setCursor(Qt::ArrowCursor);
    delete card;    
}

void FormPayments::deletePayments()
{
    QString msg;
    if (ui->treeViewPayments->selectionModel()->selectedRows().count() > 1)
        msg = tr("Удалить выбранные платежы?");
    else msg = tr("Удалить платеж") + ": " +
            mModel->dataItem(ui->treeViewPayments->currentIndex())->display() + "?";

    QMessageBox::StandardButton mesRepl;

    mesRepl = QMessageBox::question(this, tr("Подтверждение удаления"),
                                    msg, QMessageBox::Yes | QMessageBox::No);
    if(mesRepl == QMessageBox::Yes)
    {
        Request request;

        QModelIndexList list = ui->treeViewPayments->selectionModel()->selectedRows();

        for (int i = 0; i < list.count(); i++)
                request.addId(mPayments->at(list.at(i).row())->getId());

        if (list.count()) {
            ApiAdapter::getApi()->deletePayments(request);
            refreshData();
        }
    }
}

void FormPayments::initFields()
{
    mModel->clearFields();
    mFields = new QVector<Field *>();
    mFields->append(new Field("id", "Ид.", Qt::AlignRight | Qt::AlignVCenter));
    mFields->append(new Field("docNum", "№", Qt::AlignRight | Qt::AlignVCenter));
    mFields->append(new Field("docDate", "Дата"));
    mFields->append(new Field("paymentTarget", "Назначение"));
    mFields->append(new Field("name", "Наименование"));
    mFields->append(new Field("payerName", "Плательщик"));
    mFields->append(new Field("amount", "Сумма", Qt::AlignRight | Qt::AlignVCenter));
    mFields->append(new Field("idOrder", "№ заказа", Qt::AlignRight | Qt::AlignVCenter));
    mFields->append(new Field("note", "Примечание"));
    mModel->appendFields(mFields);
}

void FormPayments::initTreeViewPayments()
{
    ui->treeViewPayments->setModel(mModel);
    ui->treeViewPayments->header()->setSortIndicator(0, Qt::DescendingOrder);
    ui->treeViewPayments->header()->setSortIndicatorShown(true);
    ui->treeViewPayments->setAlternatingRowColors(true);

    connect(ui->treeViewPayments->selectionModel(), &QItemSelectionModel::currentRowChanged,
            this, &FormPayments::onChangePayment);
    connect(ui->treeViewPayments->header(), &QHeaderView::sectionClicked,
            this, &FormPayments::onClickHeaderItem);
    ui->treeViewPayments->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeViewPayments, &QWidget::customContextMenuRequested,
            this, &FormPayments::showContextMenu);
}

void FormPayments::initNavigator()
{
    mNavigator = ui->widgetNavigator;
    connect(mNavigator, &FramePageNavigator::goNextPage, this, &FormPayments::goNextPage);
    connect(mNavigator, &FramePageNavigator::goPreviosPage, this, &FormPayments::goPreviosPage);
    connect(mNavigator, &FramePageNavigator::goLastPage, this, &FormPayments::goLastPage);
    connect(mNavigator, &FramePageNavigator::goFirstPage, this, &FormPayments::goFirstPage);
    connect(mNavigator, &FramePageNavigator::refreshDataStart, this, &FormPayments::goFirstPage);
    connect(ui->widgetSearch, &FrameSearchTable::runSearch, this, &FormPayments::onRunSearch);
}

void FormPayments::initVariables()
{
    mPayments = new ListPayments();
    mModel = new NameTableModel(this, reinterpret_cast<ListDataItems *>(mPayments));
    mPreloader = new Preloader(this);
    mNumPage = 0;
    mIsShowed = false;
    mIsLoadedLayouts = false;
    ui->treeViewPayments->setModel(mModel);
    ui->dateEditDateFrom->setDate(QDate::currentDate());
    ui->dateEditDateTo->setDate(QDate::currentDate());
    initFields();
    initTreeViewPayments();
    initNavigator();
}

void FormPayments::initSignals()
{
    connect(ui->checkBoxDateFrom, static_cast<void (QCheckBox::*)(bool)>
            (&QAbstractButton::clicked), this, &FormPayments::refreshData);
    connect(ui->checkBoxDateFrom, static_cast<void (QCheckBox::*)(bool)>
            (&QAbstractButton::clicked), ui->dateEditDateFrom, &QWidget::setEnabled);
    connect(ui->dateEditDateFrom, &QDateEdit::userDateChanged, this, &FormPayments::refreshData);
    connect(ui->checkBoxDateTo, static_cast<void (QCheckBox::*)(bool)>
            (&QAbstractButton::clicked), this, &FormPayments::refreshData);
    connect(ui->checkBoxDateTo, static_cast<void (QCheckBox::*)(bool)>
            (&QAbstractButton::clicked), ui->dateEditDateTo, &QWidget::setEnabled);
    connect(ui->dateEditDateTo, &QDateEdit::userDateChanged, this, &FormPayments::refreshData);
    connect(ui->toolButtonRefresh, &QAbstractButton::clicked, this, &FormPayments::refreshData);
    connect(ui->actionAdd, &QAction::triggered, this, &FormPayments::addEditPayment);
    connect(ui->actionEdit, &QAction::triggered, this, &FormPayments::addEditPayment);
    connect(ui->treeViewPayments, &QTreeView::doubleClicked, ui->actionEdit, &QAction::trigger);
    connect(ui->actionDelete, &QAction::triggered, this, &FormPayments::deletePayments);
}

void FormPayments::refreshData()
{
    setCursor(QCursor(Qt::WaitCursor));
    resetRequest();

    QString id;

    QModelIndex index = ui->treeViewPayments->currentIndex();
    if (index.isValid())
        id = mModel->idData(ui->treeViewPayments->currentIndex());

    mPreloader->startAnimation();
    mModel->beginReset();
    if (ApiAdapter::getApi()->getListPayments(mRequest, mPayments)) {
        mCountPayemnts = mPayments->countInBase();
        ui->lineEditAmount->setText(QString::number(mPayments->amount(), 'f', 2));
    }
    mModel->endReset();
    if (!mIsLoadedLayouts)
        for (int i = 0; i < mModel->columnCount(); ++i)
            ui->treeViewPayments->resizeColumnToContents(i);
    mPreloader->stopAnimation();

    goToRowById(id);
    setCursor(QCursor(Qt::ArrowCursor));
}

void FormPayments::resetRequest()
{
    mRequest.clear();
    mRequest.setOffset(mNumPage * ui->widgetNavigator->getLimit());
    mRequest.setLimit(ui->widgetNavigator->getLimit());

    mRequest.clearFilter();
    QString filter;
    // от даты
    if (ui->checkBoxDateFrom->isChecked()) {
        filter = "docDate >= '" + ui->dateEditDateFrom->date().toString("yyyy-MM-dd") + "'";
    }
    // до даты
    if (ui->checkBoxDateTo->isChecked()) {
        if (!filter.isEmpty())
            filter += " and ";
        filter += "docDate <= '" + ui->dateEditDateTo->date().toString("yyyy-MM-dd") + "'";
    }
    mRequest.setFilter(filter);

    QString sortField = mFields->at(ui->treeViewPayments->header()->sortIndicatorSection())->name();
    mRequest.setSortBy(sortField);
    mRequest.setSearchText(ui->widgetSearch->searchText().trimmed());
    mRequest.setAscendingOrder(ui->treeViewPayments->header()->sortIndicatorOrder() ==
                               Qt::AscendingOrder);
}

void FormPayments::showContextMenu(QPoint point)
{
    onChangePayment();
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
