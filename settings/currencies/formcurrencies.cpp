#include "formcurrencies.h"
#include "ui_formcurrencies.h"
#include "field.h"
#include "apiadapter.h"
#include "formcurrencycard.h"
#include "dialogsetrate.h"
#include "userpermission.h"

FormCurrencies::FormCurrencies(QWidget *parent, DataMain *mainInfo) :
    QWidget(parent),
    ui(new Ui::FormCurrencies), mMainInfo(mainInfo)
{
    ui->setupUi(this);        
    initVariables();    
    initSignals();
    initPermissions();
}

FormCurrencies::~FormCurrencies()
{
    delete ui;
    delete mCurrencies;
}

void FormCurrencies::showEvent(QShowEvent *)
{
    startTimer(1);    
}

void FormCurrencies::timerEvent(QTimerEvent *e)
{
    killTimer(e->timerId());    
    refreshData();    
}

void FormCurrencies::onAddEditCurrency()
{
    bool isNew = sender() == ui->toolButtonAdd;
    if (!isNew && !ui->treeViewCurrencies->currentIndex().isValid())
        return;

    FormCurrencyCard card(this);
    if (!isNew)
        card.setCurrency(dynamic_cast <DataCurrency *> (mModel->dataItem(ui->treeViewCurrencies->currentIndex())));

    if (card.exec() == QDialog::Accepted) {
        refreshData();
        if (isNew)
            goToRowById(card.currency()->getId());
    }
}

void FormCurrencies::onDeleteCurrencies()
{
    QString msg;
    if (ui->treeViewCurrencies->selectionModel()->selectedRows().count() > 1)
        msg = tr("Удалить выбранные валюты?");
    else msg = tr("Удалить валюту") + ": " +
            mModel->nameData(ui->treeViewCurrencies->currentIndex()) + "?";

    QMessageBox::StandardButton mesRepl;

    mesRepl = QMessageBox::question(this, tr("Подтверждение удаления"),
                                    msg, QMessageBox::Yes | QMessageBox::No);
    if(mesRepl == QMessageBox::Yes)    {
        Request request;
        QModelIndexList list = ui->treeViewCurrencies->selectionModel()->selectedRows();

        for (int i = 0; i < list.count(); i++)
            request.addId(mModel->idData(list.at(i)));
            ApiAdapter::getApi()->deleteCurrencies(request);
            refreshData();
    }
}

void FormCurrencies::onSetRateCurency()
{
    if (!ui->treeViewCurrencies->currentIndex().isValid())
        return;

    if (mModel->dataItem(ui->treeViewCurrencies->currentIndex())->getCode() == mMainInfo->baseDataCurrency()->getCode())
        return;

    DialogSetRate dlg;
    dlg.setCurrency(dynamic_cast <DataCurrency *> (mModel->dataItem(ui->treeViewCurrencies->currentIndex())),
                    mMainInfo->baseDataCurrency());
    if (dlg.exec() == QDialog::Accepted)
        refreshData();
}

void FormCurrencies::onChangeCurrentItem()
{
    ui->toolButtonEdit->setEnabled(ui->treeViewCurrencies->currentIndex().isValid() && isEditPermission);
    ui->toolButtonDelete->setEnabled(ui->treeViewCurrencies->currentIndex().isValid() && isDeletePermission);
    ui->toolButtonSetCourse->setEnabled((!ui->checkBoxIsAutoRate->isVisible() || !ui->checkBoxIsAutoRate->isChecked()) &&
                                        ui->treeViewCurrencies->currentIndex().isValid() &&
                                        mModel->dataItem(ui->treeViewCurrencies->currentIndex())->getCode() !=
            mMainInfo->baseDataCurrency()->getCode() &&
            isEditPermission);
    ui->dateEdit->setEnabled(ui->toolButtonSetCourse->isEnabled());
}

void FormCurrencies::onChangeAutoRate()
{
    setCursor(Qt::WaitCursor);
    if (ui->checkBoxIsAutoRate->isVisible() && ui->checkBoxIsAutoRate->isChecked())
        ui->dateEdit->setDate(QDate::currentDate());
    ui->dateEdit->setEnabled((!ui->checkBoxIsAutoRate->isVisible() || !ui->checkBoxIsAutoRate->isChecked()) &&
                             isEditPermission);
    ui->toolButtonSetCourse->setEnabled((!ui->checkBoxIsAutoRate->isVisible() ||
                                         !ui->checkBoxIsAutoRate->isChecked()) && isEditPermission);
    mMainInfo->setIsManualCurrencyRate(!ui->checkBoxIsAutoRate->isChecked() && isEditPermission);
    ApiAdapter::getApi()->saveMainSettings(mMainInfo);
    refreshData();
}

void FormCurrencies::goToRowById(const QString &id)
{
    for (int i = 0; i < mModel->rowCount(); i ++)
        if (mModel->getItems()->at(i)->getId() == id) {
            ui->treeViewCurrencies->selectionModel()->
                    setCurrentIndex(mModel->index(i, 0),
                                    QItemSelectionModel::SelectCurrent |
                                    QItemSelectionModel::Rows);
            return;
        }

    if (mModel->rowCount() > 0)
        ui->treeViewCurrencies->selectionModel()->
                setCurrentIndex(mModel->index(0, 0),
                                QItemSelectionModel::SelectCurrent |
                                QItemSelectionModel::Rows);
    else onChangeCurrentItem();
}

void FormCurrencies::initSignals()
{
    connect(ui->toolButtonAdd, &QToolButton::clicked, this, &FormCurrencies::onAddEditCurrency);
    connect(ui->toolButtonEdit, &QToolButton::clicked, this, &FormCurrencies::onAddEditCurrency);    
    connect(ui->toolButtonRefresh, &QToolButton::clicked, this, &FormCurrencies::refreshData);
    connect(ui->toolButtonDelete, &QToolButton::clicked, this, &FormCurrencies::onDeleteCurrencies);
    connect(ui->toolButtonSetCourse, &QToolButton::clicked, this, &FormCurrencies::onSetRateCurency);
    connect(ui->checkBoxIsAutoRate, &QCheckBox::clicked, this, &FormCurrencies::onChangeAutoRate);
}

void FormCurrencies::connectChangeSignals()
{
    connect(ui->comboBoxListCurrencies,
            static_cast<void (QComboBox::*)(int)> (&QComboBox::currentIndexChanged), this,
            &FormCurrencies::changeIndexCurrencyBox);
    connect(ui->comboBoxListCurrencies,
            static_cast<void (QComboBox::*)(int)> (&QComboBox::currentIndexChanged), this,
            &FormCurrencies::refreshData);
    connect(ui->dateEdit, &QDateEdit::dateChanged, this, &FormCurrencies::refreshData);
    connect(ui->treeViewCurrencies->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &FormCurrencies::onChangeCurrentItem);
    connect(ui->treeViewCurrencies, &QTreeView::doubleClicked, ui->toolButtonEdit, &QToolButton::click);
}

void FormCurrencies::disconnectChaneSignals()
{
    ui->treeViewCurrencies->disconnect();
    ui->comboBoxListCurrencies->disconnect();
    ui->dateEdit->disconnect();
}

void FormCurrencies::initVariables()
{
    ui->dateEdit->setDate(QDate::currentDate());
    initBoxCurrencies();
    mCurrencies = new ListCurrencies();
    mModel = new NameTableModel(this, reinterpret_cast<ListDataItems *> (mCurrencies));
    mModel->clearFields();
    mModel->appendField(new Field("name", "Валюта"));
    mModel->appendField(new Field("code", "Код"));
    mModel->appendField(new Field("rateDate", "Дата"));
    mModel->appendField(new Field("rate", "Курс"));
    mModel->appendField(new Field("rateDisplay", "Текст"));
    ui->treeViewCurrencies->setModel(mModel);
    ui->treeViewCurrencies->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->treeViewCurrencies->setSelectionMode(QAbstractItemView::ExtendedSelection);
}

void FormCurrencies::initBoxCurrencies()
{
    for (int i = 0; i < mMainInfo->listCurrency()->size(); i++)
        ui->comboBoxListCurrencies->addItem(mMainInfo->listCurrency()->at(i)->getName(),
                                            mMainInfo->listCurrency()->at(i)->getCode());

    ui->checkBoxIsAutoRate->setChecked(!mMainInfo->isManualCurrencyRate());
    ui->dateEdit->setEnabled(!ui->checkBoxIsAutoRate->isChecked());
    ui->toolButtonSetCourse->setEnabled(!ui->checkBoxIsAutoRate->isChecked());
}

void FormCurrencies::initPermissions()
{
    isAddPermission = UserPermission::isAdd(UserPermission::CURRENCIES);
    isEditPermission = UserPermission::isEdit(UserPermission::CURRENCIES);
    isDeletePermission = UserPermission::isDelete(UserPermission::CURRENCIES);

    ui->toolButtonAdd->setEnabled(isAddPermission);
}

void FormCurrencies::refreshData()
{
    if (!UserPermission::isRead(UserPermission::CURRENCIES))
        return;

    setCursor(QCursor(Qt::WaitCursor));
    disconnectChaneSignals();
    QString id;
    if (ui->treeViewCurrencies->currentIndex().isValid())
        id = mModel->dataItem(ui->treeViewCurrencies->currentIndex())->getId();

    for (int i = 0; i < ui->comboBoxListCurrencies->count(); i++)
        if (mMainInfo->baseCurrency() == ui->comboBoxListCurrencies->itemData(i)) {
            ui->comboBoxListCurrencies->setCurrentIndex(i);
            ui->checkBoxIsAutoRate->setVisible(mMainInfo->listCurrency()->at(i)->isCanAutoRate());
            break;
        }

    mRequest.addStringValue("rateDate", ui->dateEdit->date().toString("yyyy-MM-dd"));
    if (ApiAdapter::getApi()->getListCurrencies(mRequest, mCurrencies)) {
        mModel->resetModel();        
        ui->treeViewCurrencies->resizeColumnToContents(0);
    }
    connectChangeSignals();
    goToRowById(id);    
    setCursor(QCursor(Qt::ArrowCursor));
}
