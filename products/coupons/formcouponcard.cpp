#include "formcouponcard.h"
#include "ui_formcouponcard.h"
#include "apiadapter.h"
#include "seutils.h"
#include "seconfig.h"
#include "formproducts.h"
#include "formproductsgroups.h"
#include "formcontacts.h"

FormCouponCard::FormCouponCard(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormCouponCard)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window);

    initVariables();
    initSignals();
}

FormCouponCard::~FormCouponCard()
{
    SEConfig::saveStateGeometry(this);
    delete ui; 
}

void FormCouponCard::setCoupon(const DataCoupon *coupon)
{
    mCoupon->copy(coupon);
}

const DataCoupon *FormCouponCard::coupon() const
{
    return mCoupon;
}

void FormCouponCard::showEvent(QShowEvent *)
{
    SEConfig::restoreStateGeometry(this);
    getData();
    initCurrencies();
    fillControls();
    connectOnChangeSignals();
    onChangeDiscountType();
}

void FormCouponCard::onOK()
{
    setCursor(QCursor(Qt::WaitCursor));
    ui->pushButtonCancel->setEnabled(false);
    ui->pushButtonOK->setEnabled(false);

    mCoupon->setIsSetAllFields(1);
    if (ApiAdapter::getApi()->saveCoupon(mCoupon))
        accept();

    setCursor(QCursor(Qt::ArrowCursor));
    ui->pushButtonCancel->setEnabled(true);
    ui->pushButtonOK->setEnabled(true);
}

void FormCouponCard::onAddProducts()
{
    setCursor(QCursor(Qt::WaitCursor));
    FormProducts *form = new FormProducts(this, true);
    setCursor(QCursor(Qt::ArrowCursor));
    if (form->exec() == QDialog::Accepted) {
        for (int i = 0; i < form->getSelectedProducts()->size(); i++)
            if (!mCoupon->listProducts()->existById(form->getSelectedProducts()->at(i)))
                mCoupon->listProducts()->append(new DataProduct(form->getSelectedProducts()->at(i)));
        mModelProducts->resetModel();
        for (int i = 0; i < mModelProducts->columnCount() - 1; i++)
            ui->treeViewProducts->resizeColumnToContents(i);        
        onChangeData();
    }
    delete form;
}

void FormCouponCard::onDeleteProducts()
{
    QModelIndexList list = ui->treeViewProducts->selectionModel()->selectedRows();

    for (int i = 0; i < list.size(); i++)
        mCoupon->listProducts()->at(list.at(i).row())->setIsChecked(true);
    for (int i = mCoupon->listProducts()->size() - 1; i >= 0; --i)
        if (mCoupon->listProducts()->at(i)->getIsChecked())
            delete mCoupon->listProducts()->takeAt(i);

    mModelProducts->resetModel();
    mCoupon->listProducts()->setModified();
    onChangeData();
}

void FormCouponCard::onAddGroups()
{
    setCursor(QCursor(Qt::WaitCursor));
    FormProductsGroups *form = new FormProductsGroups(this);
    form->setSelectMode(true);
    setCursor(QCursor(Qt::ArrowCursor));
    if (form->exec() == QDialog::Accepted) {
        for (int i = 0; i < form->listSelected().size(); i++)
            if (!mCoupon->listGroups()->existById(form->listSelected().at(i)))
                mCoupon->listGroups()->append(new DataProductsGroup(form->listSelected().at(i)));
        mModelGroups->resetModel();
        for (int i = 0; i < mModelGroups->columnCount() - 1; i++)
            ui->treeViewGroups->resizeColumnToContents(i);
        onChangeData();
    }
    delete form;
}

void FormCouponCard::onDeleteGroups()
{
    QModelIndexList list = ui->treeViewGroups->selectionModel()->selectedRows();

    for (int i = 0; i < list.size(); i++)
        mCoupon->listGroups()->at(list.at(i).row())->setIsChecked(true);
    for (int i = mCoupon->listGroups()->size() - 1; i >= 0; --i)
        if (mCoupon->listGroups()->at(i)->getIsChecked())
            delete mCoupon->listGroups()->takeAt(i);

    mModelGroups->resetModel();
    mCoupon->listGroups()->setModified();
    onChangeData();
}

void FormCouponCard::onChangeCurrentItemGroups()
{
     ui->toolButtonDeleteGroup->setEnabled(ui->treeViewGroups->currentIndex().isValid());
}

void FormCouponCard::onChangeCurrentItemProducts()
{
    ui->toolButtonDeleteProduct->setEnabled(ui->treeViewProducts->currentIndex().isValid());
}

void FormCouponCard::onGenerateCode()
{
    ui->lineEditCode->setText(getShortGUID());
}

void FormCouponCard::onChangeData()
{
    ui->pushButtonOK->setEnabled(!mCoupon->getCode().isEmpty());
    if (sender() == ui->comboBoxType) {
        if (ui->comboBoxType->currentIndex() == 0)
            mCoupon->setType("p");
        if (ui->comboBoxType->currentIndex() == 1)
            mCoupon->setType("a");
        if (ui->comboBoxType->currentIndex() == 2)
            mCoupon->setType("g");
    }
    if (sender() == ui->comboBoxCurrency)
        mCoupon->setCurrencyCode(ui->comboBoxCurrency->currentData().toString());

}

void FormCouponCard::onChangeDiscountType()
{    
    // Процент на всю корзину
    if (ui->comboBoxType->currentIndex() == 0) {

        if (ui->doubleSpinBoxValue->value() == 0) {
            ui->doubleSpinBoxValue->setValue(mCoupon->discount());
        }
        ui->doubleSpinBoxValue->setSuffix("%");
        ui->doubleSpinBoxValue->setPrefix(QString());
        ui->comboBoxCurrency->hide();
        ui->labelCurrency->hide();
        if (ui->tabWidgetCoupon->count() > 1) {
            ui->tabWidgetCoupon->removeTab(2);
            ui->tabWidgetCoupon->removeTab(1);
        }

        return;
    }

    // Абсолютная скидка на корзину
    if (ui->comboBoxType->currentIndex() == 1) {

        if (ui->tabWidgetCoupon->count() > 1) {
            ui->tabWidgetCoupon->removeTab(2);
            ui->tabWidgetCoupon->removeTab(1);
        }

        QString prefix, suffix;
        for (int i = 0; i < SEConfig::getMainInfo()->listCurrency()->size(); i++)
            if (SEConfig::getMainInfo()->listCurrency()->at(i)->getCode() == ui->comboBoxCurrency->currentData().toString()) {
                prefix = SEConfig::getMainInfo()->listCurrency()->at(i)->prefix();
                suffix = SEConfig::getMainInfo()->listCurrency()->at(i)->suffix();
                break;
            }
        ui->doubleSpinBoxValue->setPrefix(QString());
        ui->doubleSpinBoxValue->setSuffix(QString());
        if (!prefix.isEmpty())
            ui->doubleSpinBoxValue->setPrefix(prefix);
        else ui->doubleSpinBoxValue->setSuffix(suffix);
        ui->comboBoxCurrency->show();
        ui->labelCurrency->show();

        return;
    }

    // Процент на позиции товаров
    if (ui->comboBoxType->currentIndex() == 2) {

        if (ui->doubleSpinBoxValue->value() == 0) {
            ui->doubleSpinBoxValue->setValue(mCoupon->discount());
        }
        if (ui->tabWidgetCoupon->count() == 1) {
            ui->tabWidgetCoupon->addTab(ui->tabProducts, "Товары");
            ui->tabWidgetCoupon->addTab(ui->tabGroups, "Группы товаров");
        }
        ui->doubleSpinBoxValue->setSuffix("%");
        ui->doubleSpinBoxValue->setPrefix(QString());
        ui->comboBoxCurrency->hide();
        ui->labelCurrency->hide();

        return;
    }
}

void FormCouponCard::onChangeTabCoupons()
{
    if (ui->tabWidgetCoupon->currentIndex() == 1)
        for (int i = 0; i < mModelProducts->columnCount(); ++i)
            ui->treeViewProducts->resizeColumnToContents(i);
    if (ui->tabWidgetCoupon->currentIndex() == 2)
        for (int i = 0; i < mModelGroups->columnCount(); ++i)
            ui->treeViewGroups->resizeColumnToContents(i);
}

void FormCouponCard::onCopyCode()
{
    QApplication::clipboard()->setText(ui->lineEditCode->text().trimmed());
}

void FormCouponCard::onSelectUser()
{
    ui->toolButtonUser->setEnabled(false);
    setCursor(Qt::WaitCursor);

    FormContacts form(this, false);

    if (form.exec() == QDialog::Accepted) {
        DataContact *contact = form.getSelectedContacts()->at(0);
        mCoupon->setIdUser(contact->getId());
        ui->lineEditUser->setText(contact->displayName());
    }

    ui->toolButtonUser->setEnabled(true);
    setCursor(Qt::ArrowCursor);
}

void FormCouponCard::onClearUser()
{
    ui->lineEditUser->clear();
    mCoupon->setIdUser(QString());
}

void FormCouponCard::getData()
{
    if (mCoupon->getId().isEmpty())
        return;

    setCursor(Qt::WaitCursor);
    ApiAdapter::getApi()->getInfoCoupon(mCoupon);
    setCursor(Qt::ArrowCursor);
}

void FormCouponCard::connectOnChangeSignals()
{
    connect(ui->lineEditCode, &QLineEdit::textChanged, mCoupon, &DataCoupon::setCode);
    connect(ui->comboBoxType, static_cast<void (QComboBox::*)(int)> (&QComboBox::currentIndexChanged),
            this, &FormCouponCard::onChangeData);
    connect(ui->doubleSpinBoxValue,  static_cast<void (QDoubleSpinBox::*)(double)> (&QDoubleSpinBox::valueChanged),
            mCoupon, &DataCoupon::setDiscount);
    connect(ui->dateTimeEditEnd, &QDateTimeEdit::dateChanged, mCoupon,
            &DataCoupon::setTimeEnd);
    connect(ui->spinBoxCount, static_cast<void (QSpinBox::*)(int)> (&QSpinBox::valueChanged),
            mCoupon, &DataCoupon::setCount);
    connect(ui->comboBoxCurrency, static_cast<void (QComboBox::*)(int)> (&QComboBox::currentIndexChanged),
            this, &FormCouponCard::onChangeData);
    connect(ui->doubleSpinBoxMinAmountOrder, static_cast<void (QDoubleSpinBox::*)(double)> (&QDoubleSpinBox::valueChanged),
            mCoupon, &DataCoupon::setMinSum);
    connect(ui->checkBoxIsRegUser, static_cast<void (QCheckBox::*)(bool)> (&QCheckBox::clicked),
            mCoupon, &DataCoupon::setIsRegUser);
    connect(ui->checkBoxIsActive, static_cast<void (QCheckBox::*)(bool)> (&QCheckBox::clicked),
            mCoupon, &DataCoupon::setIsActive);
    connect(mCoupon, &DataCoupon::modified, this, &FormCouponCard::onChangeData);    
}

void FormCouponCard::initCurrencies()
{
    int j = -1;
    for (int i = 0; i < SEConfig::getMainInfo()->listCurrency()->size(); i++) {
        ui->comboBoxCurrency->addItem(SEConfig::getMainInfo()->listCurrency()->at(i)->getName(),
                                      SEConfig::getMainInfo()->listCurrency()->at(i)->getCode());
        if (SEConfig::getMainInfo()->listCurrency()->at(i)->getCode() == SEConfig::getMainInfo()->baseCurrency())
            j = i;
    }
    ui->comboBoxCurrency->setCurrentIndex(j);
}

void FormCouponCard::initSignals()
{
    connect(ui->pushButtonCancel, &QPushButton::clicked, this, &FormCouponCard::reject);
    connect(ui->pushButtonOK, &QPushButton::clicked, this, &FormCouponCard::onOK);
    connect(ui->pushButtonGenerateCode, &QPushButton::clicked, this, &FormCouponCard::onGenerateCode);
    connect(ui->comboBoxType, &QComboBox::currentTextChanged, this, &FormCouponCard::onChangeDiscountType);
    connect(ui->comboBoxCurrency, &QComboBox::currentTextChanged, this, &FormCouponCard::onChangeDiscountType);
    connect(ui->tabWidgetCoupon, &QTabWidget::currentChanged, this, &FormCouponCard::onChangeTabCoupons);
    connect(ui->toolButtonAddProduct, &QAbstractButton::clicked, this, &FormCouponCard::onAddProducts);
    connect(ui->toolButtonDeleteProduct, &QAbstractButton::clicked, this, &FormCouponCard::onDeleteProducts);
    connect(ui->toolButtonAddGroup, &QAbstractButton::clicked, this, &FormCouponCard::onAddGroups);
    connect(ui->toolButtonDeleteGroup, &QAbstractButton::clicked, this, &FormCouponCard::onDeleteGroups);
    connect(ui->treeViewProducts->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &FormCouponCard::onChangeCurrentItemProducts);
    connect(ui->treeViewGroups->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &FormCouponCard::onChangeCurrentItemGroups);
    connect(ui->toolButtonUser, &QAbstractButton::clicked, this, &FormCouponCard::onSelectUser);
    connect(ui->toolButtonUserClear, &QAbstractButton::clicked, this, &FormCouponCard::onClearUser);
}

void FormCouponCard::initVariables()
{
    mCoupon = new DataCoupon();    

    if (!SEConfig::getMainInfo()->baseCurrencyPrefix().isEmpty())
        ui->doubleSpinBoxMinAmountOrder->setPrefix(SEConfig::getMainInfo()->baseCurrencyPrefix());
    else ui->doubleSpinBoxMinAmountOrder->setSuffix(SEConfig::getMainInfo()->baseCurrencySuffix());

    mModelProducts = new NameTableModel(this, reinterpret_cast <ListDataItems *> (mCoupon->listProducts()));
    mModelProducts->prependField(new Field("id", "Ид."));
    mModelProducts->appendField(new Field("price", "Цена"));
    mModelProducts->appendField(new Field("count", "Кол-во"));
    ui->treeViewProducts->setModel(mModelProducts);    
    mModelGroups = new NameTableModel(this, reinterpret_cast <ListDataItems *> (mCoupon->listGroups()));
    mModelGroups->prependField(new Field("id", "Ид."));
    ui->treeViewGroups->setModel(mModelGroups);
    ui->tabWidgetCoupon->setCurrentIndex(0);

    connect(ui->pushButtonCopyCode, &QAbstractButton::clicked, this, &FormCouponCard::onCopyCode);
}

void FormCouponCard::fillControls()
{
    ui->dateTimeEditEnd->setDate(mCoupon->timeEnd());
    if (mCoupon->getId().isEmpty()) {
        mCoupon->setIsActive(true);
        mCoupon->setTimeEnd(ui->dateTimeEditEnd->date());
        return;
    }

    ui->lineEditUser->setText(mCoupon->userName());
    ui->lineEditCode->setText(mCoupon->getCode());
    ui->doubleSpinBoxValue->setValue(mCoupon->discount());
    ui->spinBoxCount->setValue(mCoupon->count());
    if (mCoupon->type() == "p")
        ui->comboBoxType->setCurrentIndex(0);
    if (mCoupon->type() == "a")
        ui->comboBoxType->setCurrentIndex(1);
    if (mCoupon->type() == "g")
        ui->comboBoxType->setCurrentIndex(2);
    ui->doubleSpinBoxMinAmountOrder->setValue(mCoupon->minSum());
    ui->checkBoxIsActive->setChecked(mCoupon->getIsActive());
    ui->checkBoxIsRegUser->setChecked(mCoupon->isRegUser());
    int j = -1;
    for (int i = 0; i < ui->comboBoxCurrency->count(); i++)
        if (ui->comboBoxCurrency->itemData(i).toString() == mCoupon->currencyCode()) {
            j = i;
            break;
    }
    if (j >= 0)
        ui->comboBoxCurrency->setCurrentIndex(j);
    ui->pushButtonGenerateCode->hide();
    ui->lineEditCode->setEnabled(false);

}
