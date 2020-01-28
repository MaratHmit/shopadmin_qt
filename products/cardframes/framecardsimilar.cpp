#include "framecardsimilar.h"
#include "ui_framecardsimilar.h"
#include "formproducts.h"
#include "formproductsgroups.h"

FrameCardSimilar::FrameCardSimilar(
        QWidget *parent, ListProducts *products, const bool &isDialogMode) :
    QDialog(parent),
    ui(new Ui::FrameCardSimilar)
{
    ui->setupUi(this);    
    mProducts = products;
    mIsFilled = false;
    mIsDialogMode = isDialogMode;
    mModel = new NameTableModel(this, reinterpret_cast<ListDataItems *>(mProducts));
    mFields = new QVector<Field *>();
//    mFields->append(new Field("id", "Ид.", Qt::AlignRight | Qt::AlignVCenter));
//    mFields->append(new Field("code", "Код"));
//    mFields->append(new Field("article", "Артикул"));
    mFields->append(new Field("note", "Тип"));
    mFields->append(new Field("name", "Наименование"));    
//    mFields->append(new Field("price", "Цена", Qt::AlignRight | Qt::AlignVCenter));
    mModel->appendFields(mFields);

    mFormProducts = new FormProducts(this, true);
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
    initTree();
    initActions();
    initSignals();
}

FrameCardSimilar::~FrameCardSimilar()
{
    mModel->unBindItems();
    delete ui;
}

void FrameCardSimilar::setIdProduct(const QString &id)
{
    mIdProduct = id;
}

void FrameCardSimilar::fillControls()
{
    if (mIsFilled)
        return;
    initProducts();
    mIsFilled = true;
}

void FrameCardSimilar::onAddProduct()
{
    FormProducts *form = qobject_cast<FormProducts *>(mFormProducts);
    if (mFormProducts->exec() == QDialog::Accepted) {
        mModel->beginReset();
        for (int i = 0; i < form->getSelectedProducts()->size(); i++)
            if (!mProducts->existById(form->getSelectedProducts()->at(i)) &&
                    form->getSelectedProducts()->at(i)->getId() != mIdProduct) {
                DataProduct *product = new DataProduct(form->getSelectedProducts()->at(i));
                product->setNote("Товар");
                mProducts->append(product);
            }
        mModel->endReset();
        for (int i = 0; i < mModel->columnCount() - 1; i++)
            ui->treeViewSimilarProducts->resizeColumnToContents(i);
        mProducts->setModified();
        emit modified();
    }
}

void FrameCardSimilar::onAddGroup()
{
    FormProductsGroups *form = new FormProductsGroups(this);
    form->setSelectMode(true);
    if (form->exec() == QDialog::Accepted) {
        mModel->beginReset();
        QListIterator<DataProductsGroup *> it(form->listSelected());
        while (it.hasNext()) {
            DataProductsGroup *gr = (it.next());
            bool isExist = false;
            for (int i = 0; i < mProducts->size(); i++) {
                isExist = mProducts->at(i)->getIdGroup() == gr->getId();
                if (isExist)
                    break;
            }
            if (!isExist) {
                DataProduct *pr = new DataProduct;
                pr->setIdGroup(gr->getId());
                pr->setName(gr->getName());
                pr->setNote("Группа");
                mProducts->append(pr);
            }
        }
        mModel->endReset();
        for (int i = 0; i < mModel->columnCount() - 1; i++)
            ui->treeViewSimilarProducts->resizeColumnToContents(i);
        mProducts->setModified();
        emit modified();
    }
    delete form;
}

void FrameCardSimilar::onDeleteProducts()
{
    QModelIndexList list =
            ui->treeViewSimilarProducts->selectionModel()->selectedRows();
    if (!list.size())
        return;

    mModel->beginReset();    
    QStringList idsDelete;
    for (int i = 0; i < list.size(); i++)
        idsDelete.append(mProducts->at(list.at(i).row())->getId());
    for (int i = mProducts->size() - 1; i >= 0; i--)
        if (idsDelete.contains(mProducts->at(i)->getId()))
            delete mProducts->takeAt(i);    
    mModel->endReset();
    emit modified();
    mProducts->setModified();
}

void FrameCardSimilar::onChange()
{
    ui->actionDelete->setEnabled(
                ui->treeViewSimilarProducts->selectionModel()->currentIndex().isValid());
}

void FrameCardSimilar::showContextMenu(QPoint point)
{
    onChange();
    QPoint globalPos;

    if (sender()->inherits("QAbstractScrollArea"))
        globalPos = ((QAbstractScrollArea*)sender())->viewport()->mapToGlobal(point);
    else globalPos = ((QWidget*)sender())->mapToGlobal(point);

    QMenu menu;

    menu.addAction(ui->actionAddProduct);
    menu.addAction(ui->actionDelete);
    menu.exec(globalPos);
}

void FrameCardSimilar::initProducts()
{
    mModel->resetModel();
    for (int i = 0; i < mModel->columnCount() - 1; i++)
        ui->treeViewSimilarProducts->resizeColumnToContents(i);
}

void FrameCardSimilar::initTree()
{
    ui->treeViewSimilarProducts->setModel(mModel);
    ui->treeViewSimilarProducts->setContextMenuPolicy(Qt::CustomContextMenu);
}

void FrameCardSimilar::initSignals()
{
    connect(ui->toolButtonAddGroup, SIGNAL(clicked(bool)), SLOT(onAddGroup()));

    connect(ui->actionAddProduct, SIGNAL(triggered()), SLOT(onAddProduct()));
    connect(ui->actionDelete, SIGNAL(triggered()), SLOT(onDeleteProducts()));
    connect(ui->treeViewSimilarProducts->selectionModel(),
            SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            SLOT(onChange()));
    connect(ui->treeViewSimilarProducts, SIGNAL(customContextMenuRequested(QPoint)),
            SLOT(showContextMenu(QPoint)));
}

void FrameCardSimilar::initActions()
{
    ui->toolButtonDelete->setDefaultAction(ui->actionDelete);
    ui->toolButtonAdd->setDefaultAction(ui->actionAddProduct);
}
