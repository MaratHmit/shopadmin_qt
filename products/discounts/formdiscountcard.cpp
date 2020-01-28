#include "formdiscountcard.h"
#include "ui_formdiscountcard.h"
#include "apiadapter.h"
#include "field.h"
#include "formproducts.h"
#include "formproductsgroups.h"
#include "formcontacts.h"


FormDiscountCard::FormDiscountCard(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormDiscountCard)
{
    ui->setupUi(this);    
    ui->tabWidget->removeTab(3);
    initVariables();
    initSignals();
}

FormDiscountCard::~FormDiscountCard()
{
    delete ui;
}

void FormDiscountCard::setDiscount(const DataDiscount *discount)
{
    mDiscount->copy(discount);
}

const DataDiscount *FormDiscountCard::discount() const
{
    return mDiscount;
}

void FormDiscountCard::showEvent(QShowEvent *)
{    
    getData();
    mMainInfo = SEConfig::getMainInfo();
    if (!mMainInfo->baseCurrencySuffix().isEmpty())
        ui->doubleSpinBoxSumFrom->setSuffix(mMainInfo->baseCurrencySuffix());
    else ui->doubleSpinBoxSumFrom->setPrefix(mMainInfo->baseCurrencyPrefix());    
    fillControls();
}

void FormDiscountCard::onOK()
{
    setCursor(QCursor(Qt::WaitCursor));
    ui->pushButtonOK->setEnabled(false);
    ui->pushButtonCancel->setEnabled(false);
    fillDataDiscount();
    mDiscount->setIsSetAllFields(true);
    if (ApiAdapter::getApi()->saveDiscount(mDiscount))
        accept();
    else {
        ui->pushButtonOK->setEnabled(true);
        ui->pushButtonCancel->setEnabled(true);
    }
    setCursor(QCursor(Qt::ArrowCursor));
}

void FormDiscountCard::onAddGroupsProducts()
{
    setCursor(QCursor(Qt::WaitCursor));
    FormProductsGroups *form = new FormProductsGroups(this);
    form->setSelectMode(true);
    setCursor(QCursor(Qt::ArrowCursor));
    if (form->exec() ==  QDialog::Accepted) {
        QListIterator<DataProductsGroup *> it(form->listSelected());
        while (it.hasNext()) {
            DataProductsGroup *gr = (it.next());
            if (!mDiscount->listGroupsProducts()->existById(gr))
                mDiscount->listGroupsProducts()->append(new DataItem(gr));
        }
        mModelGroupsProducts->resetModel();
    }
    delete form;
}

void FormDiscountCard::onAddProducts()
{
    setCursor(QCursor(Qt::WaitCursor));
    FormProducts *form = new FormProducts(this, true);
    setCursor(QCursor(Qt::ArrowCursor));
    if (form->exec() == QDialog::Accepted) {
        for (int i = 0; i < form->getSelectedProducts()->size(); i++)
            if (!mDiscount->listProducts()->existById(form->getSelectedProducts()->at(i)))
                mDiscount->listProducts()->append(
                            dynamic_cast<DataItem *> (new DataProduct(form->getSelectedProducts()->at(i))));
        mModelProducts->resetModel();
        for (int i = 0; i < mModelProducts->columnCount() - 1; i++)
            ui->treeViewProducts->resizeColumnToContents(i);
    }
    delete form;
}

void FormDiscountCard::onAddModifications()
{
    setCursor(QCursor(Qt::WaitCursor));
    FormProducts *form = new FormProducts(this, false);
    setCursor(QCursor(Qt::ArrowCursor));
    if (form->exec() == QDialog::Accepted && form->getSelectedProducts()->size()) {
        DataProduct *product = form->getSelectedProducts()->at(0);
        ApiAdapter::getApi()->getInfoProduct(product);
        FrameCardModifications *form = new FrameCardModifications(this, product->listModificationsGroups(), true);
        form->fillControls();
        if (form->exec() ==  QDialog::Accepted) {
            for (int i = 0; i < form->selectedModifications()->size(); i++) {
                DataItem *item = new DataItem();
                QString s;
                for (int j = 0; j < form->selectedModifications()->at(i)->values()->size(); j++) {
                    if (!s.isEmpty())
                        s += ",";
                    s += form->selectedModifications()->at(i)->values()->at(j)->getName();
                }
                item->setId(form->selectedModifications()->at(i)->getId());
                item->setName(product->getName() + "(" + s + ")");
                mDiscount->listModificationsProducts()->append(item);
            }
            mModelModifications->resetModel();
        }
        delete form;
        delete product;
    }
    delete form;
}

void FormDiscountCard::onAddContacts()
{
    setCursor(QCursor(Qt::WaitCursor));
    FormContacts *form = new FormContacts(this, true);
    setCursor(QCursor(Qt::ArrowCursor));
    if (form->exec() ==  QDialog::Accepted) {
        for (int i = 0; i < form->getSelectedContacts()->size(); i++)
            if (!mDiscount->listContacts()->existById(form->getSelectedContacts()->at(i)))
                mDiscount->listContacts()->append(
                            dynamic_cast<DataContact *> (new DataContact(form->getSelectedContacts()->at(i))));
        mModelContacts->resetModel();
        for (int i = 0; i < mModelContacts->columnCount() - 1; i++)
            ui->treeViewContacts->resizeColumnToContents(i);
    }
    delete form;
}

void FormDiscountCard::onDeleteItems()
{
    QTreeView *tree;
    ListDataItems *items = 0;
    if (sender() == ui->toolButtonDeleteProduct) {
        tree = ui->treeViewProducts;
        items = reinterpret_cast <ListDataItems *> (mDiscount->listProducts());
    }
    if (sender() == ui->toolButtonDeleteGroupProduct) {
        tree = ui->treeViewGroupProducts;
        items = mDiscount->listGroupsProducts();
    }
    if (sender() == ui->toolButtonDeleteModificationProduct) {
        tree = ui->treeViewModificationsProducts;
        items = mDiscount->listModificationsProducts();
    }
    if (sender() == ui->toolButtonDeleteContact) {
        tree = ui->treeViewContacts;
        items = mDiscount->listContacts();
    }
    if (items) {
        QModelIndexList list = tree->selectionModel()->selectedRows();
        for (int i = 0; i < list.size(); i++)
            items->at(list.at(i).row())->setIsChecked(true);
        for (int i = items->size() - 1; i >= 0; --i)
            if (items->at(i)->getIsChecked())
                delete items->takeAt(i);

        dynamic_cast<NameTableModel *> (tree->model())->resetModel();
    }
}

void FormDiscountCard::onChangeItem()
{
    QItemSelectionModel *model = dynamic_cast<QItemSelectionModel *> (sender());
    if (model == ui->treeViewProducts->selectionModel())
        ui->toolButtonDeleteProduct->setEnabled(model->currentIndex().isValid());
    if (model == ui->treeViewGroupProducts->selectionModel())
        ui->toolButtonDeleteGroupProduct->setEnabled(model->currentIndex().isValid());
    if (model == ui->treeViewModificationsProducts->selectionModel())
        ui->toolButtonDeleteModificationProduct->setEnabled(model->currentIndex().isValid());
    if (model == ui->treeViewContacts->selectionModel())
        ui->toolButtonDeleteContact->setEnabled(model->currentIndex().isValid());
}

void FormDiscountCard::fillDataDiscount()
{
    mDiscount->setName(ui->lineEditName->text().trimmed());
    if (ui->comboBoxTypeValue->currentIndex() == 0)
        mDiscount->setTypeDiscount("percent");
    else mDiscount->setTypeDiscount("absolute");
    mDiscount->setDiscount(ui->doubleSpinBoxDiscount->value());
    if (ui->groupBoxIncDiscount->isChecked()) {
        mDiscount->setStepTime(ui->spinBoxStepTime->value());
        mDiscount->setStepDiscount(ui->doubleSpinBoxStepDiscount->value());
    } else {
        mDiscount->setStepTime(0);
        mDiscount->setStepDiscount(0);
    }
    mDiscount->setProperty("isDateTimeFrom", ui->checkBoxIsFromDate->isChecked());
    mDiscount->setProperty("isDateTimeTo", ui->checkBoxIsToDate->isChecked());
    if (ui->checkBoxIsFromDate->isChecked())
        mDiscount->setDateTimeFrom(ui->dateTimeEditFrom->dateTime());
    if (ui->checkBoxIsToDate->isChecked())
        mDiscount->setDateTimeTo(ui->dateTimeEditTo->dateTime());
    QString s("0000000");
    if (ui->checkBoxMonday->isChecked())
        s[0] = '1';
    if (ui->checkBoxTuesday->isChecked())
        s[1] = '1';
    if (ui->checkBoxWednesday->isChecked())
        s[2] = '1';
    if (ui->checkBoxThursday->isChecked())
        s[3] = '1';
    if (ui->checkBoxFriday->isChecked())
        s[4] = '1';
    if (ui->checkBoxSaturday->isChecked())
        s[5] = '1';
    if (ui->checkBoxSunday->isChecked())
        s[6] = '1';
    mDiscount->setWeek(s);
    mDiscount->setSumFrom(ui->doubleSpinBoxSumFrom->value());
    mDiscount->setSumTo(ui->doubleSpinBoxSumTo->value());
    mDiscount->setCountFrom(ui->doubleSpinBoxCountFrom->value());
    mDiscount->setCountTo(ui->doubleSpinBoxCountTo->value());
    mDiscount->setTypeSum(ui->comboBoxSumType->currentIndex());
    mDiscount->setProperty("customerType", ui->comboBoxCustomerType->currentIndex());
}

void FormDiscountCard::onChangeTypeValue(int index)
{
    if (index == 0) {
        ui->labelTypeValue->setText(tr("% скидки"));
        ui->doubleSpinBoxDiscount->setSuffix("%");
        ui->doubleSpinBoxStepDiscount->setSuffix("%");
        ui->doubleSpinBoxDiscount->setPrefix(QString());
        ui->doubleSpinBoxStepDiscount->setPrefix(QString());
    }
    else {
        ui->labelTypeValue->setText(tr("Сумма скидки"));
        if (!mMainInfo->baseCurrencySuffix().isEmpty()) {
            ui->doubleSpinBoxDiscount->setSuffix(mMainInfo->baseCurrencySuffix());
            ui->doubleSpinBoxStepDiscount->setSuffix(mMainInfo->baseCurrencySuffix());
            ui->doubleSpinBoxDiscount->setPrefix(QString());
            ui->doubleSpinBoxStepDiscount->setPrefix(QString());

        } else {
            ui->doubleSpinBoxDiscount->setPrefix(mMainInfo->baseCurrencyPrefix());
            ui->doubleSpinBoxStepDiscount->setPrefix(mMainInfo->baseCurrencyPrefix());
            ui->doubleSpinBoxDiscount->setSuffix(QString());
            ui->doubleSpinBoxStepDiscount->setSuffix(QString());
        }
    }
}

void FormDiscountCard::getData()
{
    if (mDiscount->getId().isEmpty())
        return;

    setCursor(QCursor(Qt::WaitCursor));
    ApiAdapter::getApi()->getInfoDiscounts(mDiscount);
    setCursor(QCursor(Qt::ArrowCursor));
}

void FormDiscountCard::initSignals()
{
    connect(ui->pushButtonCancel, &QPushButton::clicked, this, &FormDiscountCard::reject);
    connect(ui->pushButtonOK, &QPushButton::clicked, this, &FormDiscountCard::onOK);
    connect(ui->comboBoxTypeValue,  static_cast<void (QComboBox::*)(int)> (&QComboBox::currentIndexChanged),
            this, &FormDiscountCard::onChangeTypeValue);    
    connect(ui->toolButtonAddGroupProduct, &QAbstractButton::clicked, this, &FormDiscountCard::onAddGroupsProducts);
    connect(ui->toolButtonAddProduct, &QAbstractButton::clicked, this, &FormDiscountCard::onAddProducts);
    connect(ui->toolButtonAddModificationProduct, &QAbstractButton::clicked, this, &FormDiscountCard::onAddModifications);
    connect(ui->toolButtonAddContact, &QAbstractButton::clicked, this, &FormDiscountCard::onAddContacts);
    connect(ui->toolButtonDeleteProduct, &QAbstractButton::clicked, this, &FormDiscountCard::onDeleteItems);
    connect(ui->toolButtonDeleteGroupProduct, &QAbstractButton::clicked, this, &FormDiscountCard::onDeleteItems);
    connect(ui->toolButtonDeleteModificationProduct, &QAbstractButton::clicked, this, &FormDiscountCard::onDeleteItems);
    connect(ui->toolButtonDeleteContact, &QAbstractButton::clicked, this, &FormDiscountCard::onDeleteItems);
    connect(ui->treeViewProducts->selectionModel(), &QItemSelectionModel::currentRowChanged, this,
            &FormDiscountCard::onChangeItem);
    connect(ui->treeViewGroupProducts->selectionModel(), &QItemSelectionModel::currentRowChanged, this,
            &FormDiscountCard::onChangeItem);
    connect(ui->treeViewModificationsProducts->selectionModel(), &QItemSelectionModel::currentRowChanged, this,
            &FormDiscountCard::onChangeItem);
    connect(ui->treeViewContacts->selectionModel(), &QItemSelectionModel::currentRowChanged, this,
            &FormDiscountCard::onChangeItem);
    connect(ui->checkBoxIsFromDate, SIGNAL(clicked(bool)), ui->dateTimeEditFrom, SLOT(setEnabled(bool)));
    connect(ui->checkBoxIsToDate, SIGNAL(clicked(bool)), ui->dateTimeEditTo, SLOT(setEnabled(bool)));
}

void FormDiscountCard::initVariables()
{    
    mDiscount = new DataDiscount();
    ui->tabWidget->setCurrentIndex(0);
    mModelProducts = new NameTableModel(this, mDiscount->listProducts());
    mModelProducts->prependField(new Field("id", "Ид."));
    mModelProducts->appendField(new Field("price", "Цена"));
    ui->treeViewProducts->setModel(mModelProducts);
    ui->treeViewProducts->setAlternatingRowColors(true);
    mModelGroupsProducts = new NameTableModel(this, mDiscount->listGroupsProducts());
    mModelGroupsProducts->prependField(new Field("id", "Ид."));
    ui->treeViewGroupProducts->setModel(mModelGroupsProducts);
    ui->treeViewGroupProducts->setAlternatingRowColors(true);
    mModelModifications = new NameTableModel(this, mDiscount->listModificationsProducts());
    mModelModifications->prependField(new Field("id", "Ид."));
    ui->treeViewModificationsProducts->setModel(mModelModifications);
    ui->treeViewModificationsProducts->setAlternatingRowColors(true);
    mModelContacts = new NameTableModel(this, mDiscount->listContacts());
    mModelContacts->clearFields();
    mModelContacts->appendField(new Field("id", "Ид."));
    mModelContacts->appendField(new Field("display", "Наименование"));    
    ui->treeViewContacts->setModel(mModelContacts);
    ui->treeViewContacts->setAlternatingRowColors(true);
}

void FormDiscountCard::fillControls()
{
    ui->lineEditName->setText(mDiscount->getName());
    if (mDiscount->typeDiscount() == "percent")
        ui->comboBoxTypeValue->setCurrentIndex(0);
    else ui->comboBoxTypeValue->setCurrentIndex(1);
    ui->doubleSpinBoxDiscount->setValue(mDiscount->discount());
    if (mDiscount->stepTime() > 0 || mDiscount->stepDiscount() != 0) {
        ui->groupBoxIncDiscount->setChecked(true);
        ui->spinBoxStepTime->setValue(mDiscount->stepTime());
        ui->doubleSpinBoxStepDiscount->setValue(mDiscount->stepDiscount());
    }
    if (mDiscount->property("isDateTimeFrom").toBool()) {
        ui->dateTimeEditFrom->setDateTime(mDiscount->dateTimeFrom());
        ui->dateTimeEditFrom->setEnabled(true);
        ui->checkBoxIsFromDate->setChecked(true);
    }
    if (mDiscount->property("isDateTimeTo").toBool()) {
        ui->dateTimeEditTo->setDateTime(mDiscount->dateTimeTo());
        ui->dateTimeEditTo->setEnabled(true);
        ui->checkBoxIsToDate->setChecked(true);
    }
    if (mDiscount->week().size()) {
        ui->checkBoxMonday->setChecked(mDiscount->week().at(0) == '1');
        ui->checkBoxTuesday->setChecked(mDiscount->week().at(1) == '1');
        ui->checkBoxWednesday->setChecked(mDiscount->week().at(2) == '1');
        ui->checkBoxThursday->setChecked(mDiscount->week().at(3) == '1');
        ui->checkBoxFriday->setChecked(mDiscount->week().at(4) == '1');
        ui->checkBoxSaturday->setChecked(mDiscount->week().at(5) == '1');
        ui->checkBoxSunday->setChecked(mDiscount->week().at(6) == '1');
    }
    ui->doubleSpinBoxSumFrom->setValue(mDiscount->sumFrom());
    ui->doubleSpinBoxSumTo->setValue(mDiscount->sumTo());
    ui->doubleSpinBoxCountFrom->setValue(mDiscount->countFrom());
    ui->doubleSpinBoxCountTo->setValue(mDiscount->countTo());
    mModelProducts->resetModel();
    for (int i = 0; i < mModelProducts->columnCount() - 1; i++)
        ui->treeViewProducts->resizeColumnToContents(i);    
    ui->comboBoxSumType->setCurrentIndex(mDiscount->typeSum());
    ui->comboBoxCustomerType->setCurrentIndex(mDiscount->property("customerType").toInt());
}


