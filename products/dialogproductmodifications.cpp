#include "dialogproductmodifications.h"
#include "ui_dialogproductmodifications.h"
#include "apiadapter.h"
#include "field.h"

#include <QMenu>

DialogProductModifications::DialogProductModifications(QWidget *parent, DataProduct *product) :
    QDialog(parent),
    ui(new Ui::DialogProductModifications)
{
    ui->setupUi(this);

    mProduct = product;
    ui->groupBox->setTitle(tr("Модификации - ") + product->getName());

    initVariables();
    initActions();
    initSignals();
}

DialogProductModifications::~DialogProductModifications()
{
    delete ui;
    delete mSelectedModifications;
}

const ListProducts *DialogProductModifications::selectedModifications()
{
    if (mSelectedModifications->size())
        return mSelectedModifications;

    QModelIndexList list = ui->treeView->selectionModel()->selectedRows();
    for (int i = 0; i < list.size(); i++)
        mSelectedModifications->append(dynamic_cast <DataProduct *> (mModel->dataItem(list.at(i))));

    return mSelectedModifications;
}

void DialogProductModifications::showEvent(QShowEvent *)
{
    refreshData();
}

void DialogProductModifications::showContextMenu(QPoint point)
{
    QPoint globalPos;

    if (sender()->inherits("QAbstractScrollArea"))
        globalPos = ((QAbstractScrollArea*)sender())->viewport()->mapToGlobal(point);
    else globalPos = ((QWidget*)sender())->mapToGlobal(point);

    QMenu menu;
    menu.addAction(ui->actionSelect);
    menu.exec(globalPos);
}

void DialogProductModifications::initVariables()
{    
    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeView, &QWidget::customContextMenuRequested, this, &DialogProductModifications::showContextMenu);
    mModifications = new ListProducts();
    mSelectedModifications = new ListProducts();
    mModel = new NameTableModel(this, reinterpret_cast <ListDataItems *> (mModifications), true);
    initFields();    
    ui->treeView->setModel(mModel);
}

void DialogProductModifications::initActions()
{
    ui->toolButtonSelect->setDefaultAction(ui->actionSelect);
}

void DialogProductModifications::initFields()
{
    mModel->appendField(new Field("count", "Кол-во", Qt::AlignRight | Qt::AlignVCenter));
    mModel->appendField(new Field("price", "Цена", Qt::AlignRight | Qt::AlignVCenter));
}

void DialogProductModifications::initSignals()
{
    connect(ui->actionSelect, &QAction::triggered, this, &DialogProductModifications::accept);
    connect(ui->treeView, &QTreeView::doubleClicked, this, &DialogProductModifications::accept);
}

void DialogProductModifications::fillModifications()
{
    QVector<int> index;
    QVector<double> prices;
    QVector<double> pricesPurchase;
    int countItems = 1;

    for (int i = 0; i < mProduct->listModificationsGroups()->size(); i++) {
        index.append(0);
        prices.append(0);
        pricesPurchase.append(0);
        countItems *= mProduct->listModificationsGroups()->at(i)->modifications()->size();
    }
    int lastColumn = mProduct->listModificationsGroups()->size() - 1;

    for (int i = 0; i < countItems; i++) {
        DataProduct *product = new DataProduct();
        QString name;
        QString article;
        QStringList idsModifications;
        double count = 0.0;
        double price = 0;
        double pricePurchase = 0;

        for (int j = 0; j < mProduct->listModificationsGroups()->size(); j++) {
            if (j)
                name += ", ";
            prices[j] = 0;
            pricesPurchase[j] = 0;
            for (int k = 0; k < mProduct->listModificationsGroups()->at(j)->modifications()->
                 at(index.at(j))->values()->size(); k++) {
                if (k)
                    name += ", ";
                name += mProduct->listModificationsGroups()->at(j)->columns()->at(k)->getName() + ": " +
                        mProduct->listModificationsGroups()->at(j)->modifications()->at(index.at(j))->
                        values()->at(k)->getName();                
            }            
            if (!idsModifications.contains(mProduct->listModificationsGroups()->at(j)->
                                           modifications()->at(index.at(j))->getId()))
                idsModifications.append(mProduct->listModificationsGroups()->at(j)->
                                        modifications()->at(index.at(j))->getId());            
            count = mProduct->listModificationsGroups()->at(j)->modifications()->at(index.at(j))->count();
            article = mProduct->listModificationsGroups()->at(j)->modifications()->at(index.at(j))->article();

            if (mProduct->listModificationsGroups()->at(j)->type().isEmpty() || (mProduct->listModificationsGroups()->at(j)->type() == "0")) {
                prices[j] = mProduct->price() + mProduct->listModificationsGroups()->at(j)->modifications()->at(index.at(j))->price();
                pricesPurchase[j] = mProduct->pricePurchase() + mProduct->listModificationsGroups()->at(j)->modifications()->at(index.at(j))->pricePurchase();
            }
            if (mProduct->listModificationsGroups()->at(j)->type() == "1") {
                prices[j] = mProduct->price() * mProduct->listModificationsGroups()->at(j)->modifications()->at(index.at(j))->price();
                pricesPurchase[j] = mProduct->pricePurchase() * mProduct->listModificationsGroups()->at(j)->modifications()->at(index.at(j))->pricePurchase();
            }
            if (mProduct->listModificationsGroups()->at(j)->type() == "2") {
                prices[j] = mProduct->listModificationsGroups()->at(j)->modifications()->at(index.at(j))->price();
                pricesPurchase[j] = mProduct->listModificationsGroups()->at(j)->modifications()->at(index.at(j))->pricePurchase();
            }


            price += prices[j];
            pricePurchase += pricesPurchase[j];
        }

        index[lastColumn]++;
        for (int j = mProduct->listModificationsGroups()->size() - 1; j > 0; j--) {
            if (index.at(j) >= mProduct->listModificationsGroups()->at(j)->modifications()->size()) {
                index[j] = 0;
                index[j - 1]++;
            } else break;
        }

        if (article.isEmpty())
            article = mProduct->getArticle();
        product->setArticle(article);
        product->setCurrency(mProduct->getCurrency());
        product->setPrice(price);
        product->setPricePurchase(pricePurchase);
        product->setName(mProduct->getName() + " (" + name + ")");        
        product->setCount(count);
        product->setIsInfinitely(count < 0);
        product->setIdsModifications(idsModifications.join(","));
        mModifications->append(product);
    }

    mModel->resetModel();
    ui->treeView->resizeColumnToContents(0);
    ui->treeView->resizeColumnToContents(1);
    ui->treeView->selectionModel()->setCurrentIndex(
                mModel->index(0, 0), QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);
    ui->treeView->setFocus();
}

void DialogProductModifications::refreshData()
{
    if (ApiAdapter::getApi()->getInfoProduct(mProduct))
        fillModifications();
}
