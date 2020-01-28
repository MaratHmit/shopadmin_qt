#include "framecarddiscounts.h"
#include "ui_framecarddiscounts.h"
#include "formdiscounts.h"

FrameCardDiscounts::FrameCardDiscounts(
        QWidget *parent, const DataProduct *product,
        const bool &isDialogMode) :
    QDialog(parent),
    ui(new Ui::FrameCardDiscounts)
{
    ui->setupUi(this);
    mIsFilled = false;
    mIsDialogMode = isDialogMode;
    mProduct = product;
    mDiscounts = product->listDiscounts();
    initDiscountsModel();

    if (isDialogMode) {
        setWindowFlags(Qt::Window);
        connect(ui->buttonOK, &QPushButton::clicked, this,
                &QDialog::accept);
        connect(ui->buttonCancel, &QPushButton::clicked, this,
                &QDialog::reject);
    }
    else {
        setWindowFlags(Qt::Widget);
        ui->buttonOK->hide();
        ui->buttonCancel->hide();
    }
    initSignals();
}

FrameCardDiscounts::~FrameCardDiscounts()
{
    mModel->unBindItems();
    delete ui;
}

void FrameCardDiscounts::setIdProduct(const QString &id)
{
    mIdProduct = id;
}

void FrameCardDiscounts::fillControls()
{
    if (mIsFilled)
        return;

    ui->checkBoxIsDiscount->setChecked(mProduct->isDiscount());
    ui->doubleSpinBoxMaxDiscount->setValue(mProduct->maxDiscount());
    connect(ui->checkBoxIsDiscount, static_cast<void (QCheckBox::*)(bool)>(&QCheckBox::clicked),
            mProduct, &DataProduct::setIsDiscount);
    connect(ui->doubleSpinBoxMaxDiscount,
            static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            mProduct, &DataProduct::setMaxDiscount);

    initDiscounts();
    mIsFilled = true;
}

void FrameCardDiscounts::initDiscountsModel()
{
    mModel = new NameTableModel(this, reinterpret_cast <ListDataItems *> (mDiscounts));
    mModel->clearFields();
    mModel->appendField(new Field("id", "Ид."));
    mModel->appendField(new Field("name", "Наименование"));
    mModel->appendField(new Field("dateTimeFrom", "Старт"));
    mModel->appendField(new Field("dateTimeTo", "Завершение"));
    mModel->appendField(new Field("stepTime", "Шаг времени", Qt::AlignRight | Qt::AlignVCenter));
    mModel->appendField(new Field("stepDiscount", "Шаг скидки", Qt::AlignRight | Qt::AlignVCenter));
    mModel->appendField(new Field("sumFrom", "От суммы", Qt::AlignRight | Qt::AlignVCenter));
    mModel->appendField(new Field("sumTo", "До суммы", Qt::AlignRight | Qt::AlignVCenter));
    mModel->appendField(new Field("countFrom", "От кол-во", Qt::AlignRight | Qt::AlignVCenter));
    mModel->appendField(new Field("countTo", "До кол-во", Qt::AlignRight | Qt::AlignVCenter));
    mModel->appendField(new Field("discount", "Скидка", Qt::AlignRight | Qt::AlignVCenter));
    mModel->appendField(new Field("week", "Дни недели"));
    ui->treeViewDiscounts->setModel(mModel);
}

void FrameCardDiscounts::initDiscounts()
{
    mModel->resetModel();
    for (int i = 0; i < mModel->columnCount() - 1; i++)
        ui->treeViewDiscounts->resizeColumnToContents(i);
}

void FrameCardDiscounts::initSignals()
{
    connect(ui->toolButtonAddDiscount, &QAbstractButton::clicked, this,
            &FrameCardDiscounts::addDiscount);
    connect(ui->toolButtonDelDiscount, &QAbstractButton::clicked, this,
            &FrameCardDiscounts::delDiscounts);
    connect(ui->treeViewDiscounts->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &FrameCardDiscounts::onChangeDiscountItem);

}

void FrameCardDiscounts::addDiscount()
{
    FormDiscounts form(this);
    form.setSelectMode(true);
    if (form.exec() ==  QDialog::Accepted) {
        const ListDiscounts *list = form.selectedDiscounts();
        for (int i = 0; i < list->size(); i++)
            if (!mDiscounts->existById(list->at(i)))
               mDiscounts->append(new DataDiscount(list->at(i)));
        mModel->resetModel();
        emit modified();
    }
}

void FrameCardDiscounts::delDiscounts()
{
    QModelIndexList list =
            ui->treeViewDiscounts->selectionModel()->selectedRows();
    if (!list.size())
        return;

    QStringList idsDelete;
    for (int i = 0; i < list.size(); i++)
        idsDelete.append(mDiscounts->at(list.at(i).row())->getId());
    for (int i = mDiscounts->size() - 1; i >= 0; i--)
        if (idsDelete.contains(mDiscounts->at(i)->getId()))
            delete mDiscounts->takeAt(i);
    mModel->resetModel();
    emit modified();
    mDiscounts->setModified();
    onChangeDiscountItem();
}

void FrameCardDiscounts::onChangeDiscountItem()
{
    ui->toolButtonDelDiscount->setEnabled(ui->treeViewDiscounts->currentIndex().isValid());
}
