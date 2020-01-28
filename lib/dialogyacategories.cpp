#include "dialogyacategories.h"
#include "ui_dialogyacategories.h"

DialogYACategories::DialogYACategories(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogYACategories)
{
    ui->setupUi(this);

    initVariables();
    initSignals();
}

DialogYACategories::~DialogYACategories()
{
    delete ui;
}

const DataItem *DialogYACategories::selectedItem() const
{
    if (ui->treeViewGroups->currentIndex().isValid())
        return mModel->dataItem(ui->treeViewGroups->currentIndex());

    return 0;
}

void DialogYACategories::accept()
{
    if (ui->treeViewGroups->currentIndex().isValid())
        QDialog::accept();
}

void DialogYACategories::showEvent(QShowEvent *)
{
    refreshData();
}

void DialogYACategories::initVariables()
{
    mItems = new ListDataItems;
    mModel = new NameTableModel(this, mItems);    
    ui->treeViewGroups->setModel(mModel);
}

void DialogYACategories::initSignals()
{
    connect(ui->treeViewGroups, SIGNAL(doubleClicked(QModelIndex)), SLOT(accept()));
}

void DialogYACategories::refreshData()
{
   QVariantMap map = SEConfig::getMainInfo()->property("yaMarketCategories").toMap();

   DataItem *root = 0;
   QMap<QString, QString> links;
   QMap<QString, QVariant>::const_iterator it = map.constBegin();
   for(;it != map.constEnd(); ++it)
   {
        DataItem *item = new DataItem;
        item->setId(it.key());
        item->setCode(it.value().toString());
        QStringList path = item->getCode().split("/");
        item->setName(path.at(path.size() - 1));
        path.removeLast();
        item->setValue(path.join("/"));
        links.insert(item->getCode(), item->getId());
        if (item->getValue().isEmpty())
            root = item;
        mItems->append(item);
   }

   for (int i = 0; i < mItems->size(); ++i)
        if (!mItems->at(i)->getValue().isEmpty() && links.keys().contains(mItems->at(i)->getValue()))
            mItems->at(i)->setIdParent(links[mItems->at(i)->getValue()]);

    if (root)
        setChildrens(root, mItems);

   mModel->resetModel();
}

void DialogYACategories::setChildrens(DataItem *data, ListDataItems *list)
{
    for (int i = 0; i < list->size(); i++) {
        if (!data->getId().isEmpty() && data->getId() == list->at(i)->getIdParent()) {
            DataItem *obj = list->at(i);
            obj->setParent(data);
            data->setIsFilledChilds(true);
            setChildrens(list->at(i), list);
        }
    }
}
