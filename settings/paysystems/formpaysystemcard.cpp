#include "formpaysystemcard.h"
#include "ui_formpaysystemcard.h"
#include "apiadapter.h"
#include "dialogaddparampaysystem.h"
#include "formproducts.h"
#include "dialoghostname.h"
#include "field.h"

#include <QDebug>

FormPaySystemCard::FormPaySystemCard(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormPaySystemCard)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window);        
    initVariables();
    initSignals();
}

FormPaySystemCard::~FormPaySystemCard()
{
    delete ui;   
}

void FormPaySystemCard::setPaySystem(const DataPaySystem *paySystem)
{
    mPaySystem->copy(paySystem);
}

void FormPaySystemCard::setExistPlugins(const QStringList &list)
{
    foreach (QString s, list)
        if (!s.compare(mPaySystem->identifier()))
            mExistPluginsList.append(s);
}

void FormPaySystemCard::onChangeData()
{
    if (sender() == ui->lineEditName)
        mPaySystem->setName(ui->lineEditName->text().trimmed());
    if (sender() == ui->lineEditHelpURL)
        mPaySystem->setUrlHelp(ui->lineEditHelpURL->text().trimmed());
    if (sender() == ui->checkBoxOuterBlank)
        mPaySystem->setIsExtBlank(ui->checkBoxOuterBlank->isChecked());
    if (sender() == ui->checkBoxIsActive)
        mPaySystem->setIsActive(ui->checkBoxIsActive->isChecked());
    if (sender() == ui->checkBoxIsAdvance)
        mPaySystem->setIsAdvance(ui->checkBoxIsAdvance->isChecked());
    if (sender() == ui->checkIsAuthorize)
        mPaySystem->setIsAuthorize(ui->checkIsAuthorize->isChecked());
    if (sender() == ui->checkBoxIsTestMode)
        mPaySystem->setIsTestMode(ui->checkBoxIsTestMode->isChecked());
    if (sender() == ui->comboBoxCustomerType)
        mPaySystem->setProperty("customerType", ui->comboBoxCustomerType->currentIndex());
    if (sender() == ui->comboBoxPlugins) {
        if (ui->comboBoxPlugins->currentIndex() >= 0) {
            mPaySystem->setIdentifier(ui->comboBoxPlugins->currentText());
            ui->toolButtonLoadParameters->setEnabled(true);
            if (ui->lineEditName->text().isEmpty()) {
                ui->lineEditName->setText(ui->comboBoxPlugins->currentText());
                mPaySystem->setName(ui->lineEditName->text().trimmed());
            }
        }
        else mPaySystem->setIdentifier("");
    }    
    if (sender() == ui->widgetLogoImage)
        mPaySystem->setImageFile(ui->widgetLogoImage->fileImageName());

    ui->pushButtonSave->setEnabled(!ui->lineEditName->text().trimmed().isEmpty());
}

void FormPaySystemCard::onChangeTab()
{
    setCursor(Qt::WaitCursor);
    if (ui->tabWidget->currentWidget() == ui->tabInfoPage && !mFrameInfoPage) {
        QHBoxLayout *layoutInfo = new QHBoxLayout;
        mFrameInfoPage = new FrameWYSIWYG(ui->tabInfoPage);
        mFrameInfoPage->setSection(SEConsts::Section_ShopPayment);
        layoutInfo->addWidget(mFrameInfoPage);
        layoutInfo->setMargin(2);
        ui->tabInfoPage->setLayout(layoutInfo);
        mFrameInfoPage->setHTMLText(mPaySystem->pageMainInfo());
        connect(mFrameInfoPage, SIGNAL(modified()), SLOT(onChangeData()));
    }
    if (ui->tabWidget->currentWidget() == ui->tabBlankAccount && !mFrameAccountPage) {
        QHBoxLayout *layoutAccount = new QHBoxLayout;
        mFrameAccountPage = new FrameWYSIWYG(this);
        mFrameAccountPage->setSection(SEConsts::Section_ShopPayment);
        layoutAccount->addWidget(mFrameAccountPage);
        layoutAccount->setMargin(2);
        ui->tabBlankAccount->setLayout(layoutAccount);
        mFrameAccountPage->setHTMLText(mPaySystem->pageBlank());
        connect(mFrameAccountPage, SIGNAL(modified()), SLOT(onChangeData()));
    }
    if (ui->tabWidget->currentWidget() == ui->tabSuccess && !mFrameSuccesPage) {
        QHBoxLayout *layoutSuccess = new QHBoxLayout;
        mFrameSuccesPage = new FrameWYSIWYG(this);
        mFrameSuccesPage->setSection(SEConsts::Section_ShopPayment);
        layoutSuccess->addWidget(mFrameSuccesPage);
        layoutSuccess->setMargin(2);
        ui->tabSuccess->setLayout(layoutSuccess);
        mFrameSuccesPage->setHTMLText(mPaySystem->pageSuccess());
        connect(mFrameSuccesPage, SIGNAL(modified()), SLOT(onChangeData()));
    }
    if (ui->tabWidget->currentWidget() == ui->tabResult && !mFrameResultPage) {
        QHBoxLayout *layoutResult = new QHBoxLayout;
        mFrameResultPage = new FrameWYSIWYG(this);
        mFrameResultPage->setSection(SEConsts::Section_ShopPayment);
        layoutResult->addWidget(mFrameResultPage);
        layoutResult->setMargin(2);
        ui->tabResult->setLayout(layoutResult);
        mFrameResultPage->setHTMLText(mPaySystem->pageResult());
        connect(mFrameResultPage, SIGNAL(modified()), SLOT(onChangeData()));
    }
    if (ui->tabWidget->currentWidget() == ui->tabFail && !mFrameFailPage) {
        QHBoxLayout *layoutFailf = new QHBoxLayout;
        mFrameFailPage = new FrameWYSIWYG(this);
        mFrameFailPage->setSection(SEConsts::Section_ShopPayment);
        layoutFailf->addWidget(mFrameFailPage);
        layoutFailf->setMargin(2);
        ui->tabFail->setLayout(layoutFailf);
        mFrameFailPage->setHTMLText(mPaySystem->pageFail());
        connect(mFrameFailPage, SIGNAL(modified()), SLOT(onChangeData()));
    }
    setCursor(Qt::ArrowCursor);
}

void FormPaySystemCard::showEvent(QShowEvent *)
{    
    initPlugins();
    startTimer(10);
}

void FormPaySystemCard::timerEvent(QTimerEvent *e)
{
    killTimer(e->timerId());
    getData();
    fillControls();
}

void FormPaySystemCard::onOk()
{
    if (ui->lineEditName->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, tr("Cохранение платежной системы"),  tr("Задайте пожалуйста имя платёжной системы!"));
        ui->lineEditName->setFocus();
        return;
    }

    setData();
    bool isNew = mPaySystem->getId().isEmpty();        
    mPaySystem->setIsSetAllFields(isNew);
    if (ApiAdapter::getApi()->savePaySystem(mPaySystem)) {
        ui->widgetLogoImage->saveImage();
        accept();
        mPaySystem->setIsChecked(mPaySystem->getIsActive());
        emit save(isNew, mPaySystem);
    }
}

void FormPaySystemCard::onSave()
{
    if (ui->lineEditName->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, tr("Cохранение платежной системы"),  tr("Задайте пожалуйста имя платёжной системы!"));
        ui->lineEditName->setFocus();
        return;
    }

    ui->pushButtonCancel->setEnabled(false);
    ui->pushButtonOK->setEnabled(false);
    ui->pushButtonSave->setEnabled(false);

    setData();
    bool isNew = mPaySystem->getId().isEmpty();    
    mPaySystem->setIsSetAllFields(isNew);
    if (ApiAdapter::getApi()->savePaySystem(mPaySystem)) {
        if (isNew) {
            getData();
            fillControls();
        }    
        ui->widgetLogoImage->saveImage();
        mPaySystem->setIsChecked(mPaySystem->getIsActive());
        emit save(isNew, mPaySystem);
    } else ui->pushButtonSave->setEnabled(true);
    ui->pushButtonCancel->setEnabled(true);
    ui->pushButtonOK->setEnabled(true);
}

void FormPaySystemCard::hideTabControls()
{
    ui->tabWidget->removeTab(ui->tabWidget->indexOf(ui->tabBlankAccount));
    ui->tabWidget->removeTab(ui->tabWidget->indexOf(ui->tabResult));
    ui->tabWidget->removeTab(ui->tabWidget->indexOf(ui->tabSuccess));
    ui->tabWidget->removeTab(ui->tabWidget->indexOf(ui->tabFail));
}

void FormPaySystemCard::initVariables()
{    
    mFrameInfoPage = 0;
    mFrameAccountPage = 0;
    mFrameSuccesPage = 0;
    mFrameResultPage = 0;
    mFrameFailPage = 0;

    mPaySystem = new DataPaySystem();
    mPlugins = new ListDataItems();
    mModelPayReqs = new NameTableModel(this, reinterpret_cast <ListDataItems *> (mPaySystem->payReqs()));
    mModelPayReqs->clearFields();
    mModelPayReqs->appendField(new Field("code", "Код"));
    mModelPayReqs->appendField(new Field("name", "Заголовок"));
    mModelPayReqs->appendField(new Field("valueSecret", "Значение"));
    ui->treeViewPayReqs->setModel(mModelPayReqs);
    ui->treeViewPayReqs->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->treeViewPayReqs->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->tabWidget->setCurrentIndex(0);
    mModelHosts = new NameTableModel(this, mPaySystem->hosts());
    ui->listViewHosts->setModel(mModelHosts);
    ui->listViewHosts->setSelectionMode(QAbstractItemView::ExtendedSelection);
    mModelFilters = new NameTableModel(this, reinterpret_cast <ListDataItems *> (mPaySystem->filters()));
    ui->listViewFilters->setModel(mModelFilters);
    ui->listViewFilters->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->widgetLogoImage->hideImageAlt();
    ui->widgetLogoImage->setSection(SEConsts::Section_ShopPayment);
}

void FormPaySystemCard::initSignals()
{
    connect(ui->pushButtonCancel, &QPushButton::clicked, this, &FormPaySystemCard::reject);
    connect(ui->toolButtonLoadParameters, &QToolButton::clicked, this, &FormPaySystemCard::loadParameters);
    connect(ui->pushButtonSave, &QPushButton::clicked, this, &FormPaySystemCard::onSave);
    connect(ui->pushButtonOK, &QPushButton::clicked, this, &FormPaySystemCard::onOk);
    connect(ui->toolButtonAddParam, &QToolButton::clicked, this, &FormPaySystemCard::onAddEditParam);
    connect(ui->toolButtonEditParam, &QToolButton::clicked, this, &FormPaySystemCard::onAddEditParam);
    connect(ui->treeViewPayReqs, &QTableView::doubleClicked, ui->toolButtonEditParam, &QToolButton::click);
    connect(ui->treeViewPayReqs->selectionModel(), &QItemSelectionModel::currentChanged, this,
            &FormPaySystemCard::onCurrentChangedParam);
    connect(ui->toolButtonDelParam, &QToolButton::clicked, this, &FormPaySystemCard::onDelParam);
    connect(ui->toolButtonAddFilter, &QToolButton::clicked, this, &FormPaySystemCard::onAddFilter);
    connect(ui->toolButtonDelFilter, &QToolButton::clicked, this, &FormPaySystemCard::onDelFilter);
    connect(ui->listViewFilters->selectionModel(), &QItemSelectionModel::currentChanged, this,
            &FormPaySystemCard::onCurrentChangedFilter);
    connect(ui->toolButtonAddHost, &QToolButton::clicked, this, &FormPaySystemCard::onAddHost);
    connect(ui->toolButtonDelHost, &QToolButton::clicked, this, &FormPaySystemCard::onDelHost);
    connect(ui->listViewHosts->selectionModel(), &QItemSelectionModel::currentChanged, this,
            &FormPaySystemCard::onCurrentChangedHost);    
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), SLOT(onChangeTab()));    
}

void FormPaySystemCard::initPlugins()
{
    if (ApiAdapter::getApi()->getListPaySystemsPlugins(mPlugins)) {
        for (int i = 0; i < mPlugins->size(); i++)          
            ui->comboBoxPlugins->addItem(mPlugins->at(i)->getName());
    }
    ui->comboBoxPlugins->setCurrentIndex(-1);
}

void FormPaySystemCard::getData()
{
    if (!mPaySystem->getId().isEmpty())
        ApiAdapter::getApi()->getInfoPaySystem(mPaySystem);
}

void FormPaySystemCard::setData()
{
    if (mFrameInfoPage) {
        mFrameInfoPage->refreshSourceText();
        mPaySystem->setPageMainInfo(mFrameInfoPage->textHTML());
    }
    if (mFrameAccountPage) {
        mFrameAccountPage->refreshSourceText();
        mPaySystem->setPageBlank(mFrameAccountPage->textHTML());
    }    
    if (mFrameSuccesPage) {
        mFrameSuccesPage->refreshSourceText();
        mPaySystem->setPageSuccess(mFrameSuccesPage->textHTML());
    }
    if (mFrameResultPage) {
        mFrameResultPage->refreshSourceText();
        mPaySystem->setPageResult(mFrameResultPage->textHTML());
    }
    if (mFrameFailPage) {
        mFrameFailPage->refreshSourceText();
        mPaySystem->setPageFail(mFrameFailPage->textHTML());
    }
}

void FormPaySystemCard::loadParameters()
{
    if (ui->toolButtonLoadParameters->isEnabled() && !mPaySystem->payReqs()->size())
        onSave();
}

void FormPaySystemCard::fillControls()
{    
    disconnectChangeSignals();
    if (mPaySystem->getId().isEmpty()) {
        connectChangeSignals();
        return;
    }

    ui->widgetLogoImage->setImage(mPaySystem->getImageFile());
    ui->groupBoxParams->setEnabled(true);
    ui->lineEditName->setText(mPaySystem->getName());
    ui->checkBoxOuterBlank->setChecked(mPaySystem->isExtBlank());
    ui->checkBoxIsActive->setChecked(mPaySystem->getIsActive());
    ui->checkIsAuthorize->setChecked(mPaySystem->isAuthorize());
    ui->checkBoxIsAdvance->setChecked(mPaySystem->isAdvance());
    ui->checkBoxIsTestMode->setChecked(mPaySystem->isTestMode());
    ui->comboBoxPlugins->setCurrentText(mPaySystem->identifier());
    ui->comboBoxPlugins->setEnabled(false);
    switch (mPaySystem->property("customerType").toInt()) {
    case 1:
        ui->comboBoxCustomerType->setCurrentIndex(1);
        break;
    case 2:
        ui->comboBoxCustomerType->setCurrentIndex(2);
        break;
    default:
        break;
    }
    if (!mPaySystem->identifier().isEmpty())
        ui->toolButtonLoadParameters->hide();
    if (ui->comboBoxPlugins->currentIndex() >= 0)
        hideTabControls();    
    mModelPayReqs->resetModel();
    for (int i = 0; i < mModelPayReqs->columnCount(); ++i)
        ui->treeViewPayReqs->resizeColumnToContents(i);
    mModelFilters->resetModel();
    mModelHosts->resetModel();    
    connectChangeSignals();
}

void FormPaySystemCard::connectChangeSignals()
{
    connect(ui->lineEditName, SIGNAL(textEdited(QString)), SLOT(onChangeData()));
    connect(ui->lineEditHelpURL, SIGNAL(textEdited(QString)), SLOT(onChangeData()));
    connect(ui->checkBoxOuterBlank, SIGNAL(clicked()), SLOT(onChangeData()));
    connect(ui->checkBoxIsActive, SIGNAL(clicked()), SLOT(onChangeData()));
    connect(ui->checkIsAuthorize, SIGNAL(clicked()), SLOT(onChangeData()));
    connect(ui->checkBoxIsAdvance, SIGNAL(clicked()), SLOT(onChangeData()));
    connect(ui->checkBoxIsTestMode, SIGNAL(clicked()), SLOT(onChangeData()));
    connect(ui->comboBoxPlugins, SIGNAL(currentIndexChanged(int)), SLOT(onChangeData()));
    connect(ui->comboBoxCustomerType, SIGNAL(currentIndexChanged(int)), SLOT(onChangeData()));
    connect(ui->widgetLogoImage, SIGNAL(modified()), SLOT(onChangeData()));
}

void FormPaySystemCard::disconnectChangeSignals()
{
    disconnect(ui->lineEditName);
    disconnect(ui->lineEditHelpURL);
    disconnect(ui->checkBoxOuterBlank);
    disconnect(ui->checkBoxIsActive);
    disconnect(ui->checkBoxIsTestMode);
    disconnect(ui->comboBoxPlugins);
    disconnect(ui->widgetLogoImage);
}

void FormPaySystemCard::onCurrentChangedParam()
{
    ui->toolButtonEditParam->setEnabled(ui->treeViewPayReqs->currentIndex().isValid());
    ui->toolButtonDelParam->setEnabled(ui->treeViewPayReqs->currentIndex().isValid());
}

void FormPaySystemCard::onAddEditParam()
{
    bool isNew = sender() == ui->toolButtonAddParam;

    DialogAddParamPaySystem dlg(this, false);
    if (!isNew)
        dlg.setPayReq(reinterpret_cast <DataPayRequisite *>
                      (mPaySystem->payReqs()->at(ui->treeViewPayReqs->currentIndex().row())));
    if (dlg.exec() == QDialog::Accepted) {
        if (isNew)
            mPaySystem->payReqs()->append(new DataItem(dlg.payReq()));
        else if (ui->treeViewPayReqs->currentIndex().isValid()) {
            mPaySystem->payReqs()->at(ui->treeViewPayReqs->currentIndex().row())->copy(dlg.payReq());
            mPaySystem->payReqs()->setModified();
        }
        mModelPayReqs->resetModel();
        onChangeData();
    }    
}

void FormPaySystemCard::onDelParam()
{
    QModelIndexList list = ui->treeViewPayReqs->selectionModel()->selectedRows();

    for (int i = 0; i < list.size(); i++)
        mPaySystem->payReqs()->at(list.at(i).row())->setIsSelected(true);
    for (int i = mPaySystem->payReqs()->size() - 1; i >= 0; i--)
        if (mPaySystem->payReqs()->at(i)->getIsSelected()) {
            delete mPaySystem->payReqs()->takeAt(i);
            mPaySystem->payReqs()->setModified();
        }
    mModelPayReqs->resetModel();
    mPaySystem->payReqs()->setModified();
    onCurrentChangedParam();
    onChangeData();
}

void FormPaySystemCard::onCurrentChangedFilter()
{
    ui->toolButtonDelFilter->setEnabled(ui->listViewFilters->currentIndex().isValid());
}

void FormPaySystemCard::onAddFilter()
{
    ui->toolButtonAddFilter->setEnabled(false);
    FormProducts *form = new FormProducts(this, true);
    if (form->exec() == QDialog::Accepted) {
        for (int i = 0; i < form->getSelectedProducts()->size(); i++)
            if (!mPaySystem->filters()->existById(form->getSelectedProducts()->at(i)))
                mPaySystem->filters()->append(new DataProduct(form->getSelectedProducts()->at(i)));
        mModelFilters->resetModel();
    }
    ui->toolButtonAddFilter->setEnabled(true);
    onChangeData();
}

void FormPaySystemCard::onDelFilter()
{
    QModelIndexList list = ui->listViewFilters->selectionModel()->selectedRows();

    for (int i = 0; i < list.size(); i++)
        mPaySystem->filters()->at(list.at(i).row())->setIsSelected(true);
    for (int i = mPaySystem->filters()->size() - 1; i >= 0; i--)
        if (mPaySystem->filters()->at(i)->getIsSelected()) {
            delete mPaySystem->filters()->takeAt(i);
            mPaySystem->filters()->setModified();
        }
    mModelFilters->resetModel();
    mPaySystem->filters()->setModified();
    onCurrentChangedFilter();
    onChangeData();
}

void FormPaySystemCard::onCurrentChangedHost()
{
    ui->toolButtonDelHost->setEnabled(ui->listViewHosts->currentIndex().isValid());
}

void FormPaySystemCard::onAddHost()
{
    DialogHostName dlg(this);
    if (dlg.exec() == QDialog::Accepted && !dlg.hostName().trimmed().isEmpty()) {
        DataItem *item = new DataItem();
        item->setName(dlg.hostName());
        mPaySystem->hosts()->append(item);
        mModelHosts->resetModel();
        onChangeData();
    }
}

void FormPaySystemCard::onDelHost()
{
    QModelIndexList list = ui->listViewHosts->selectionModel()->selectedRows();

    for (int i = 0; i < list.size(); i++)
        mPaySystem->hosts()->at(list.at(i).row())->setIsSelected(true);
    for (int i = mPaySystem->hosts()->size() - 1; i >= 0; i--)
        if (mPaySystem->hosts()->at(i)->getIsSelected()) {
            delete mPaySystem->hosts()->takeAt(i);
            mPaySystem->hosts()->setModified();
        }
    mModelHosts->resetModel();
    mPaySystem->hosts()->setModified();
    onCurrentChangedHost();
    onChangeData();
}



