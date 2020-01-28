#include "formordercard.h"
#include "ui_formordercard.h"
#include "formcontacts.h"
#include "apiadapter.h"
#include "seconfig.h"
#include "dialogproductitem.h"
#include "formproducts.h"
#include "request.h"
#include "formdeliveries.h"
#include "formcontactcard.h"
#include "formcompanycard.h"
#include "formpaymentcard.h"
#include "userpermission.h"

#include <QDebug>
#include <QPlainTextEdit>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QComboBox>
#include <QRadioButton>
#include <QDateEdit>

FormOrderCard::FormOrderCard(QWidget *parent, const DataOrder *order) :
    QDialog(parent),
    ui(new Ui::FormOrderCard)
{
    ui->setupUi(this);

    if (order)
        mOrder = new DataOrder(order);
    else {
        mOrder = new DataOrder;
        ui->pushButtonDynFields->hide();
    }
    setWindowFlags(Qt::Window);
    ui->pageCustomFields->setDataType("order");
    ui->pageCustomFields->setItem(mOrder);

    initVariables();
    fillDefaultControls();
    initActions();
    initSignals();        
    initPermissions();
}

void FormOrderCard::setInfoMode()
{
    ui->toolButtonCustomer->setEnabled(false);
    ui->toolButtonCustomerInfo->hide();
    ui->pushButtonOK->hide();
    ui->pushButtonSave->hide();
    ui->pushButtonCancel->setText(tr("Закрыть"));
}

FormOrderCard::~FormOrderCard()
{
    delete ui;
    delete mOrder;
    delete mListManagers;
    delete mListDeliveries;       
}

void FormOrderCard::showEvent(QShowEvent *)
{    
    if (!SEConfig::getMainInfo()->baseCurrencySuffix().isEmpty()) {
        ui->doubleSpinBoxDeliverySum->setSuffix(SEConfig::getMainInfo()->baseCurrencySuffix());
        ui->doubleSpinBoxDiscountSum->setSuffix(SEConfig::getMainInfo()->baseCurrencySuffix());
    }
    else {
        ui->doubleSpinBoxDeliverySum->setSuffix(SEConfig::getMainInfo()->baseCurrencyPrefix());
        ui->doubleSpinBoxDiscountSum->setSuffix(SEConfig::getMainInfo()->baseCurrencyPrefix());
    }    
    if (!mOrder->getId().isEmpty())
        getData();
}

void FormOrderCard::addProduct()
{
    FormProducts form(this, true, true);
    setCursor(QCursor(Qt::WaitCursor));
    if (form.exec() == QDialog::Accepted) {
        for (int i = 0; i < form.getSelectedProducts()->size(); ++i) {
            DataProduct *product = form.getSelectedProducts()->at(i);
            product->setNote(QString());
            if (mIsSelectOptions && form.getSelectedProducts()->at(i)->getCountOptions()) {                
                ApiAdapter::getApi()->getInfoProduct(product);
                FrameCardOptions *frameOptions = new FrameCardOptions(this, product, 2);
                frameOptions->fillControls();
                if (frameOptions->exec() == QDialog::Accepted) {
                    mOrder->listOrderItems()->append(new DataOrderItem(product));
                }
            } else {
                mOrder->listOrderItems()->append(new DataOrderItem(product));
            }
        }
        mModelProducts->resetModel();
        onModified();        
        calcSumTotal();
    }
    setCursor(QCursor(Qt::ArrowCursor));
}

void FormOrderCard::addService()
{
    DialogProductItem dialog(this, 0, true);
    if (dialog.exec() == QDialog::Accepted) {
        mOrder->listOrderItems()->append(new DataOrderItem(dialog.product()));
        mModelProducts->resetModel();
        onModified();
        ui->tableViewProducts->resizeColumnsToContents();
        calcSumTotal();
    }
}

void FormOrderCard::editProduct()
{
    if (!ui->tableViewProducts->currentIndex().isValid())
        return;

    DataOrderItem *orderItem =
            reinterpret_cast <DataOrderItem *> (mModelProducts->dataItem(ui->tableViewProducts->currentIndex()));
    const QString id = orderItem->getId();
    if (orderItem->getCountOptions()) {
        DataProduct *product = new DataProduct;
        product->setId(orderItem->idPrice());
        product->setName(orderItem->getName());
        product->setPrice(orderItem->price());
        for (int i = 0; i < orderItem->listOptions()->size(); ++i) {
            product->addOption(new DataOption(orderItem->listOptions()->at(i)));
        }

        FrameCardOptions *frameOptions = new FrameCardOptions(this, product, 2);
        frameOptions->fillControls();
        if (orderItem->originalName().isEmpty())
            product->setName(orderItem->getName());
        else product->setName(orderItem->originalName());
        if (frameOptions->exec() == QDialog::Accepted) {

        }

    } else {
        if (orderItem->idPrice().isEmpty()) {
            DialogProductItem dialog(this, orderItem);
            if (dialog.exec() == QDialog::Accepted) {
                orderItem->copy(dialog.product());
                orderItem->setId(id);
                orderItem->setModified("name");
                orderItem->setIsSetAllFields(true);
                mModelProducts->resetModel();
                onModified();
                ui->tableViewProducts->resizeColumnsToContents();
                calcSumTotal();
            }
        } else {
            FormProducts form(this, false, true);
            if (!orderItem->idsModifications().isEmpty())
                form.setSystemSearchText("?[id]=" + orderItem->idPrice());
            form.setAutoSelectModifications(!orderItem->idsModifications().isEmpty());
            setCursor(QCursor(Qt::WaitCursor));
            if (form.exec() == QDialog::Accepted && form.getSelectedProducts()->size()) {
                orderItem->copy(form.getSelectedProducts()->at(0));
                orderItem->setId(id);
                orderItem->setModified("name");
                orderItem->setIsSetAllFields(true);
                mModelProducts->resetModel();
                onModified();
                ui->tableViewProducts->resizeColumnsToContents();
                calcSumTotal();
            }
            setCursor(QCursor(Qt::ArrowCursor));
        }
    }
}

void FormOrderCard::deleteProducts()
{
    if (!ui->tableViewProducts->currentIndex().isValid())
        return;

    QModelIndexList list = ui->tableViewProducts->selectionModel()->selectedIndexes();

    mModelProducts->deleteItems(list);

    mModelProducts->resetModel();
    onModified();
    ui->tableViewProducts->resizeColumnsToContents();
    calcSumTotal();
}

void FormOrderCard::calcSumTotal()
{
    double sum = 0;
    for (int i = 0; i < mOrder->listOrderItems()->size(); i++)
        sum += mOrder->listOrderItems()->at(i)->cost();
    ui->lineEditSumList->setText(SEConfig::priceCurrency(sum, mOrder->currency()));    
    sum += mOrder->deliverySum() - mOrder->discountSum();
    if (sum < 0)
        sum = 0;
    mOrder->setSum(sum);
    ui->lineEditSumTotal->setText(mOrder->sumTitle());
    ui->lineEditSumTotal->repaint();
    onModified();
}

void FormOrderCard::getData()
{
    setCursor(Qt::WaitCursor);
    if (ApiAdapter::getApi()->getInfoOrder(mOrder))
        fillControls();    
    setCursor(Qt::ArrowCursor);
}

void FormOrderCard::delManager()
{
    ui->comboBoxManager->setCurrentIndex(-1);
}

void FormOrderCard::addPayment()
{
    FormPaymentCard card(this, mOrder);
    if (card.exec() ==  QDialog::Accepted) {
        mOrder->listPayments()->append(new DataPayment(card.payment()));
        mModelPayments->resetModel();
        for (int i = 0; i < mModelPayments->columnCount(); ++i)
            ui->treeViewPayments->resizeColumnToContents(i);
        mOrder->listPayments()->setModified();;
        onModified();
        onChangePayment();
    }
}

void FormOrderCard::deletePayment()
{
    if (!ui->treeViewPayments->currentIndex().isValid())
        return;

    QModelIndexList list = ui->treeViewPayments->selectionModel()->selectedIndexes();
    mModelPayments->deleteItems(list);
    mModelPayments->resetModel();
    mOrder->listPayments()->setModified();
    onModified();
    onChangePayment();
}

void FormOrderCard::onChangePayment()
{
    ui->toolButtonDeletePayment->setEnabled(ui->treeViewPayments->currentIndex().isValid());
}

void FormOrderCard::onChangeOrderStatus()
{
    mOrder->setStatusOrder(ui->comboBoxOrderStatuses->currentData().toString());
    ui->dateEditCredit->setVisible(mOrder->statusOrder() == "K");
}

void FormOrderCard::fillDefaultControls()
{
    ui->labelPaymentType->hide();
    ui->lineEditPaymentType->hide();
    ui->dateEditDeliveryDate->setDate(QDate::currentDate());

    ui->linePaymentType->hide();

    if (ApiAdapter::getApi()->getListUsers(Request(), mListManagers)) {
        int index = -1;
        for (int i = 0; i < mListManagers->size(); ++i) {
            ui->comboBoxManager->addItem(mListManagers->at(i)->displayName(),
                                         mListManagers->at(i)->getId());
            if ((mOrder->getId().isEmpty() && (SEConfig::idCurrentUser == mListManagers->at(i)->getId())) ||
                    (!mOrder->idManager().isEmpty() && (mOrder->idManager() == mListManagers->at(i)->getId()))) {
                index = i;
                if (mOrder->getId().isEmpty())
                    mOrder->setIdManager(mListManagers->at(i)->getId());
            }
        }
        ui->comboBoxManager->setCurrentIndex(index);
    }    

    // статусы доставок
    for (int i = 0; i < SEConfig::getDeliveriesStatuses()->size(); ++i)
        ui->comboBoxDeliveryStatuses->addItem(SEConfig::getDeliveriesStatuses()->at(i)->getName(),
                                            SEConfig::getDeliveriesStatuses()->at(i)->getId());
    ui->comboBoxDeliveryStatuses->setCurrentIndex(1);
    int index = 1;
    // статус заказа
    for (int i = 0; i < SEConfig::getOrdersStatuses()->size(); i++) {
        ui->comboBoxOrderStatuses->addItem(SEConfig::getOrdersStatuses()->at(i)->getName(),
                                           SEConfig::getOrdersStatuses()->at(i)->getId());
        if (!mOrder->getId().isEmpty() && (SEConfig::getOrdersStatuses()->at(i)->getId() == mOrder->statusOrder()))
            index = i;
    }    
    ui->comboBoxOrderStatuses->setCurrentIndex(index);

    ui->tableViewProducts->setFocus();
    ui->dateEditCredit->setDate(QDate::currentDate().addMonths(1));
}

void FormOrderCard::fillControls()
{  
    mModelProducts->resetModel();
    if (mOrder->getId().isEmpty())
          return;

    ui->lineEditOrderId->setText(mOrder->getId());
    ui->dateEditOrderDate->setDate(mOrder->dateOrder());    
    if (mOrder->property("idCompany").isNull())
        ui->lineEditCustomerId->setText(mOrder->idCustomer());
    else ui->lineEditCustomerId->setText(mOrder->property("idCompany").toString());
    ui->lineEditCustomer->setText(mOrder->customer());   
    ui->comboBoxManager->setCurrentText(mOrder->manager());
    ui->lineEditNote->setText(mOrder->property("note").toString());
    ui->doubleSpinBoxDeliverySum->setValue(mOrder->deliverySum());
    ui->doubleSpinBoxDiscountSum->setValue(mOrder->discountSum());
    ui->lineEditSumTotal->setText(mOrder->sumTitle());
    qreal sumProducts = mOrder->sum() - mOrder->discountSum() - mOrder->deliverySum();
    QString sumProductsStr = SEConfig::priceCurrency(sumProducts, mOrder->currency());
    ui->lineEditSumList->setText(sumProductsStr);
    ui->lineEditDeliveryPhone->setText(mOrder->deliveryPhone());
    ui->lineEditDeliveryAddress->setText(mOrder->deliveryAddress());
    ui->lineEditDeliveryPostIndex->setText(mOrder->deliveryPostIndex());
    ui->lineEditlDeliveryEmail->setText(mOrder->deliveryEmail());
    ui->lineEditDeliveryTimeCall->setText(mOrder->deliveryCallTime());
    ui->dateEditDeliveryDate->setDate(mOrder->deliveryDate());
    ui->lineEditDelivery->setText(mOrder->deliveryName());
    ui->comboBoxDeliveryStatuses->setCurrentText(mOrder->statusDeliveryTitle());
    ui->lineEditDeliveryNote->setText(mOrder->property("deliveryNote").toString());
    ui->tableViewProducts->resizeColumnsToContents();
    ui->lineEditOrderId->setFocus();
    ui->dateEditCredit->setVisible(mOrder->statusOrder() == "K");
    ui->dateEditCredit->setDate(mOrder->dateCredit());
    if (!mOrder->paymentTypePrimary().isEmpty()) {
        ui->linePaymentType->show();
        ui->labelPaymentType->show();
        ui->lineEditPaymentType->show();
        ui->lineEditPaymentType->setText(mOrder->paymentTypePrimary());
    }    
    if (!mOrder->property("deliveryCityId").isNull()) {
        ListDataItems cities;
        Request request;
        request.addId(mOrder->property("deliveryCityId").toString());
        if (ApiAdapter::getApi()->getListCities(request, &cities) && cities.size()) {
            ui->lineEditDeliveryCity->setText(cities.at(0)->getName());
        }
    }
    if (SEConfig::getMainInfo()->isShowDocDelivery()) {
        ui->groupBoxShipment->show();
        ui->lineEditDeliveryDocNum->setText(mOrder->deliveryDocNum());
        ui->dateEditDeliveryDocDate->setDate(mOrder->deliveryDocDate());
        ui->lineEditDeliveryServiceName->setText(mOrder->deliveryServiceName());
    }
}


void FormOrderCard::initActions()
{
    ui->toolButtonAddProduct->setDefaultAction(ui->actionAddProduct);
    ui->toolButtonAddService->setDefaultAction(ui->actionAddService);
    ui->toolButtonEditProduct->setDefaultAction(ui->actionEdit);
    ui->toolButtonDelProduct->setDefaultAction(ui->actionDelete);
}

void FormOrderCard::initSignals()
{
    connect(ui->pushButtonCancel, &QAbstractButton::clicked, this, &FormOrderCard::reject);    
    connect(ui->pushButtonMainInfo, &QAbstractButton::clicked, this, &FormOrderCard::selectItemMenu);
    connect(ui->pushButtonDelivery, &QAbstractButton::clicked, this, &FormOrderCard::selectItemMenu);
    connect(ui->pushButtonDynFields, &QAbstractButton::clicked, this, &FormOrderCard::selectItemMenu);
    connect(ui->pushButtonOrderPayments, &QAbstractButton::clicked, this, &FormOrderCard::selectItemMenu);
    connect(ui->toolButtonCustomer, &QAbstractButton::clicked, this, &FormOrderCard::selectCustomer);
    connect(ui->toolButtonCustomerInfo, &QAbstractButton::clicked, this, &FormOrderCard::showCustomer);
    connect(ui->toolButtonDelManager, &QAbstractButton::clicked, this, &FormOrderCard::delManager);
    connect(ui->actionAddProduct, &QAction::triggered, this, &FormOrderCard::addProduct);
    connect(ui->actionAddService, &QAction::triggered, this, &FormOrderCard::addService);
    connect(ui->actionEdit, &QAction::triggered, this, &FormOrderCard::editProduct);
    connect(ui->actionDelete, &QAction::triggered, this, &FormOrderCard::deleteProducts);
    connect(ui->pushButtonSave, &QAbstractButton::clicked, this, &FormOrderCard::save);    
    connect(ui->pushButtonOK, &QAbstractButton::clicked, this, &FormOrderCard::onOk);
    connect(ui->tableViewProducts->selectionModel(),
            &QItemSelectionModel::currentRowChanged, this, &FormOrderCard::onChangeCurrentItemProduct);
    connect(ui->dateEditOrderDate, &QDateEdit::dateChanged, mOrder, &DataOrder::setDateOrder);
    connect(ui->lineEditNote, &QLineEdit::textEdited, mOrder, &DataOrder::setNote);
    connect(ui->lineEditDeliveryDocNum, &QLineEdit::textEdited, mOrder, &DataOrder::setDeliveryDocNum);
    connect(ui->lineEditDeliveryServiceName, &QLineEdit::textEdited, mOrder, &DataOrder::setDeliveryServiceName);
    connect(ui->dateEditDeliveryDocDate, &QDateEdit::dateChanged, mOrder, &DataOrder::setDeliveryDocDate);
    connect(ui->lineEditDeliveryPhone, &QLineEdit::textEdited, mOrder, &DataOrder::setDeliveryPhone);
    connect(ui->lineEditlDeliveryEmail, &QLineEdit::textEdited, mOrder, &DataOrder::setDeliveryEmail);
    connect(ui->lineEditDeliveryTimeCall, &QLineEdit::textEdited, mOrder, &DataOrder::setDeliveryCallTime);
    connect(ui->lineEditDeliveryPostIndex, &QLineEdit::textEdited, mOrder, &DataOrder::setDeliveryPostIndex);
    connect(ui->lineEditDeliveryAddress, &QLineEdit::textEdited, mOrder, &DataOrder::setDeliveryAddress);
    connect(ui->comboBoxDeliveryStatuses, static_cast<void (QComboBox::*)(int)> (&QComboBox::currentIndexChanged),
            this, &FormOrderCard::onChangeDeliveryStatus);    
    connect(ui->comboBoxOrderStatuses, static_cast<void (QComboBox::*)(int)> (&QComboBox::currentIndexChanged),
            this, &FormOrderCard::onChangeOrderStatus);
    connect(ui->dateEditDeliveryDate, &QDateEdit::dateChanged, mOrder, &DataOrder::setDeliveryDate);
    connect(mOrder, &DataOrder::modified, this, &FormOrderCard::onModified);
    connect(ui->doubleSpinBoxDiscountSum, static_cast<void (QDoubleSpinBox::*)(double)>
            (&QDoubleSpinBox::valueChanged),
            mOrder, &DataOrder::setDiscountSum);
    connect(ui->doubleSpinBoxDiscountSum, static_cast<void (QDoubleSpinBox::*)(double)>
            (&QDoubleSpinBox::valueChanged),
            this, &FormOrderCard::calcSumTotal);
    connect(ui->doubleSpinBoxDeliverySum, static_cast<void (QDoubleSpinBox::*)(double)>
            (&QDoubleSpinBox::valueChanged),
            mOrder, &DataOrder::setDeliverySum);
    connect(ui->doubleSpinBoxDeliverySum, static_cast<void (QDoubleSpinBox::*)(double)>
            (&QDoubleSpinBox::valueChanged),
            this, &FormOrderCard::calcSumTotal);
    connect(ui->comboBoxManager,  static_cast<void (QComboBox::*)(int)> (&QComboBox::currentIndexChanged),
            this, &FormOrderCard::onChangeManager);
    connect(ui->tableViewProducts, SIGNAL(doubleClicked(QModelIndex)), ui->toolButtonEditProduct,
            SLOT(click()));
    connect(mModelProducts, &QAbstractItemModel::dataChanged, this, &FormOrderCard::calcSumTotal);
    connect(ui->toolButtonDelivery, &QAbstractButton::clicked, this, &FormOrderCard::onChangeDeliveryType);
    connect(ui->toolButtonAddPayment, &QAbstractButton::clicked, this, &FormOrderCard::addPayment);
    connect(ui->toolButtonDeletePayment, &QAbstractButton::clicked, this, &FormOrderCard::deletePayment);
    connect(ui->treeViewPayments->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            SLOT(onChangePayment()));
    connect(ui->dateEditCredit, &QDateEdit::dateChanged, mOrder, &DataOrder::setDateCredit);
    connect(ui->pageCustomFields, SIGNAL(modified()), SLOT(onModified()));
}

void FormOrderCard::initVariables()
{
    mIsShowedPayments = false;
    ui->groupBoxShipment->hide();
    ui->dateEditDeliveryDocDate->setDate(QDate::currentDate());
    mIsSelectOptions = SEConfig::getMainInfo()->isShowOptions();
    ui->pushButtonOrderPayments->setVisible(!mOrder->getId().isEmpty());
    ui->stackedWidgetMain->setCurrentIndex(0);
    mModelProducts = new NameTableModel(this, reinterpret_cast <ListDataItems *> (mOrder->listOrderItems()));
    mModelProducts->setEditableMode(true);    
    mModelPayments = new NameTableModel(this, reinterpret_cast <ListDataItems *> (mOrder->listPayments()));
    setFieldItems();
    setFieldPayments();
    ui->tableViewProducts->setModel(mModelProducts);
    ui->treeViewPayments->setModel(mModelPayments);
    ui->dateEditOrderDate->setDate(QDate::currentDate());
    mListManagers = new ListContacts();
    mListDeliveries = new ListDeliveries();        
    ui->actionEdit->setEnabled(false);
    ui->actionDelete->setEnabled(false);
    ui->dateEditCredit->setVisible(false);
    ui->tableViewProducts->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableViewProducts, &QWidget::customContextMenuRequested,
            this, &FormOrderCard::showContextMenu);
}

void FormOrderCard::initPermissions()
{
    ui->toolButtonCustomer->setEnabled(UserPermission::isRead(UserPermission::CONTACTS));
    ui->toolButtonCustomerInfo->setEnabled(UserPermission::isRead(UserPermission::CONTACTS));
    ui->actionAddProduct->setEnabled(UserPermission::isRead(UserPermission::PRODUCTS));
}

void FormOrderCard::loadPayments()
{
    setCursor(QCursor(Qt::WaitCursor));

    Request request;
    QString filter = "idOrder = " + mOrder->getId();
    request.setFilter(filter);
    mIsShowedPayments = ApiAdapter::getApi()->getListPayments(request, mOrder->listPayments());
    mModelPayments->resetModel();
    setCursor(QCursor(Qt::ArrowCursor));
}

void FormOrderCard::onChangeCurrentItemProduct()
{
    ui->actionEdit->setEnabled(ui->tableViewProducts->currentIndex().isValid());
    ui->actionDelete->setEnabled(ui->tableViewProducts->currentIndex().isValid());
}

void FormOrderCard::onChangeManager()
{
    mOrder->setIdManager(ui->comboBoxManager->currentData().toString());
}

void FormOrderCard::onChangeDeliveryStatus()
{
    mOrder->setStatusDelivery(ui->comboBoxDeliveryStatuses->currentData().toString());
    if (SEConfig::getMainInfo()->isShowDocDelivery() && ((ui->comboBoxDeliveryStatuses->currentData().toString() == "Y") ||
            (ui->comboBoxDeliveryStatuses->currentData().toString() == "P")))
        ui->groupBoxShipment->show();

}

void FormOrderCard::onChangeDeliveryType()
{
    FormDeliveries form(this, true);

    if (form.exec() == QDialog::Accepted) {
        mOrder->setDeliveryId(form.selectedDelivery()->getId());
        ui->lineEditDelivery->setText(form.selectedDelivery()->getName());
        ui->doubleSpinBoxDeliverySum->setValue(form.selectedDelivery()->price());
    }
}

void FormOrderCard::onModified()
{
    ui->pushButtonSave->setEnabled(mOrder->isModified());
}

void FormOrderCard::onOk()
{
    if (!mOrder->isModified() || save())
        accept();
}

bool FormOrderCard::save()
{
    if (mOrder->idCustomer().isEmpty() && mOrder->property("idCompany").toString().isEmpty()) {
        QMessageBox::warning(this, tr("Сохранение заказа"), tr("Не выбран заказчик заказа!"));
        ui->toolButtonCustomer->setFocus();
        return false;
    }

    if (!mOrder->listOrderItems()->size()) {
        QMessageBox::warning(this, tr("Сохранение заказа"), tr("Отсутствуют товары и услуги заказа!"));
        ui->toolButtonAddProduct->setFocus();
        return false;
    }

    if (!ui->lineEditDeliveryDocNum->text().isEmpty() || !ui->lineEditDeliveryServiceName->text().isEmpty())
        mOrder->setDeliveryDocDate(ui->dateEditDeliveryDocDate->date());

    ui->pushButtonSave->setEnabled(false);
    ui->pushButtonOK->setEnabled(false);
    mOrder->setIsSendToEmail(ui->checkBoxIsSendToEmail->isChecked());
    if (ApiAdapter::getApi()->saveOrder(mOrder)) {
        emit saved();
        ui->pushButtonOK->setEnabled(true);
        ui->pushButtonSave->setEnabled(false);
        return true;
    } else ui->pushButtonSave->setEnabled(true);
    ui->pushButtonOK->setEnabled(true);
    return false;
}

void FormOrderCard::showContextMenu(QPoint point)
{
    onChangeCurrentItemProduct();
    QPoint globalPos;

    if (sender()->inherits("QAbstractScrollArea"))
        globalPos = ((QAbstractScrollArea*)sender())->viewport()->mapToGlobal(point);
    else globalPos = ((QWidget*)sender())->mapToGlobal(point);

    QMenu menu;

    menu.addAction(ui->actionAddProduct);
    menu.addAction(ui->actionAddService);
    menu.addAction(ui->actionEdit);
    menu.addSeparator();
    menu.addAction(ui->actionDelete);
    menu.exec(globalPos);
}

void FormOrderCard::setFieldItems()
{
    mModelProducts->clearFields();
    mFieldsProducts = new QVector<Field *>();
    mFieldsProducts->append(new Field("article", "Артикул"));
    mFieldsProducts->append(new Field("name", "Наименование"));
    mFieldsProducts->append(new Field("count", "Кол-во", Qt::AlignRight | Qt::AlignVCenter));
    mFieldsProducts->append(new Field("price", "Цена", Qt::AlignRight | Qt::AlignVCenter));
    mFieldsProducts->append(new Field("discount", "Скидка", Qt::AlignRight | Qt::AlignVCenter));
    mFieldsProducts->append(new Field("cost", "Стоимость", Qt::AlignRight | Qt::AlignVCenter));
    mFieldsProducts->append(new Field("note", "Примечание"));
    mModelProducts->appendFields(mFieldsProducts);
}

void FormOrderCard::setFieldPayments()
{
    mModelPayments->clearFields();
    mFieldsPayments = new QVector<Field *>();
    mFieldsPayments->append(new Field("id", "Ид.", Qt::AlignRight | Qt::AlignVCenter));
    mFieldsPayments->append(new Field("docNum", "№", Qt::AlignRight | Qt::AlignVCenter));
    mFieldsPayments->append(new Field("docDate", "Дата"));
    mFieldsPayments->append(new Field("name", "Наименование"));
    mFieldsPayments->append(new Field("payerName", "Плательщик"));
    mFieldsPayments->append(new Field("amount", "Сумма", Qt::AlignRight | Qt::AlignVCenter));    
    mModelPayments->appendFields(mFieldsPayments);
}

void FormOrderCard::selectItemMenu()
{
    if (sender() == ui->pushButtonMainInfo)
        ui->stackedWidgetMain->setCurrentIndex(0);
    if (sender() == ui->pushButtonDynFields)
        ui->stackedWidgetMain->setCurrentIndex(1);
    if (sender() == ui->pushButtonDelivery)
        ui->stackedWidgetMain->setCurrentIndex(2);
    if (sender() == ui->pushButtonOrderPayments) {
        ui->stackedWidgetMain->setCurrentIndex(3);
        if (!mIsShowedPayments) {
            loadPayments();
            for (int i = 0; i < mModelPayments->columnCount() - 1; i++)
                ui->treeViewPayments->resizeColumnToContents(i);
        }
    }
}

void FormOrderCard::selectCustomer()
{
    ui->toolButtonCustomer->setEnabled(false);
    setCursor(Qt::WaitCursor);

    FormContacts form(this, false);

    if (form.exec() == QDialog::Accepted) {
        DataContact *contact = form.getSelectedContacts()->at(0);
        if (form.getSelectedTypeContacts()) {
            mOrder->setProperty("idCompany", contact->getId());
            mOrder->setCustomer(contact->getName());
            if (!contact->property("idContact").toString().isEmpty())
                mOrder->setIdCustomer(contact->property("idContact").toString());
            ui->lineEditCustomer->setText(contact->getName());
            ui->lineEditCustomerId->setText(contact->getId());
        } else {
            mOrder->setIdCustomer(contact->getId());
            mOrder->setCustomer(contact->displayName());
            ui->lineEditCustomer->setText(contact->displayName());
            ui->lineEditCustomerId->setText(contact->getId());
        }
        if (ui->lineEditDeliveryAddress->text().isEmpty())
            ui->lineEditDeliveryAddress->setText((contact->address()));
        if (ui->lineEditDeliveryPhone->text().isEmpty())
            ui->lineEditDeliveryPhone->setText(contact->phone());
        if (ui->lineEditlDeliveryEmail->text().isEmpty())
            ui->lineEditlDeliveryEmail->setText(contact->email());
    }

    ui->toolButtonCustomer->setEnabled(true);
    setCursor(Qt::ArrowCursor);
}

void FormOrderCard::showCustomer()
{
    if (mOrder->idCustomer().isEmpty() && mOrder->property("idCompany").toString().isEmpty())
        return;

    if (mOrder->property("idCompany").toString().isEmpty()) {
        FormContactCard card(this);
        card.setIdContact(mOrder->idCustomer(), true);
        if (card.exec() == QDialog::Accepted)
            ui->lineEditCustomer->setText(card.getContact()->displayName());
    } else {
        FormCompanyCard card(this);
        card.setIdCompany(mOrder->property("idCompany").toString());
        if (card.exec() == QDialog::Accepted)
            ui->lineEditCustomer->setText(card.getCompany()->displayName());
    }

}
