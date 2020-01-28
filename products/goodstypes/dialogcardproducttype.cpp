#include "dialogcardproducttype.h"
#include "ui_dialogcardproducttype.h"

#include "apiadapter.h"
#include "seconsts.h"
#include "formfeatures.h"

#include <QMessageBox>

DialogCardProductType::DialogCardProductType(QWidget *parent) :
    DialogCard(parent),
    ui(new Ui::DialogCardProductType)
{
    ui->setupUi(this);    
    initVariables();
    initSignals();
}

DialogCardProductType::~DialogCardProductType()
{
    delete ui;
}

void DialogCardProductType::initSignals()
{
    connect(ui->toolButtonAdd, SIGNAL(clicked(bool)), SLOT(selectFeatures()));
    connect(ui->toolButtonDelete, SIGNAL(clicked(bool)), SLOT(deleteFeatures()));
    connect(ui->treeView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), SLOT(currentRowChanged()));

}

void DialogCardProductType::initVariables()
{
    mFeatures = new ListFeatures;
    mModelFeatures = new NameTableModel(this, reinterpret_cast <ListDataItems *> (mFeatures));    
    ui->treeView->setModel(mModelFeatures);
}

void DialogCardProductType::initControls()
{
    mIsModifiedFeatures = false;
    ui->lineEdit->clear();
    mFeatures->deleteItems();
    mModelFeatures->resetModel();
    if (item->getId().isEmpty())
        setWindowTitle("Новый тип товара");
    else setWindowTitle(QString("Редактирование типа: " + item->getName()));
    ui->lineEdit->setFocus();
}

void DialogCardProductType::fillControls()
{
    if (item->getId().isEmpty() || !ApiAdapter::getApi()->getInfoData(SEConsts::API_PRODUCTS_TYPES, item))
        return;

    ui->lineEdit->setText(item->getName());
    if (!item->property("featuresList").isNull()) {
        QList<QVariant> list = item->property("featuresList").toList();
        for (int i = 0; i < list.size(); ++i) {
            DataFeature *feature = new DataFeature;
            QMap<QString, QVariant> map = list.at(i).toMap();
            feature->setId(map["id"].toString());
            feature->setName(map["name"].toString());
            mFeatures->append(feature);
        }
        mModelFeatures->resetModel();
        ui->treeView->selectionModel()->
                setCurrentIndex(mModelFeatures->index(0, 0), QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);
        ui->treeView->setFocus();
    }
}

bool DialogCardProductType::setData()
{
    if (ui->lineEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Сохранение типа товара", "Не заполнено обязателное поле: <Наименование>!");
        ui->lineEdit->setFocus();
        return false;
    }

    item->setName(ui->lineEdit->text().trimmed());
    if (mIsModifiedFeatures) {
        QVariantList list;
        QStringList features;
        for (int i = 0; i < mFeatures->size(); ++i) {
            QMap<QString, QVariant> map;
            map["id"] = mFeatures->at(i)->getId();
            map["name"]= mFeatures->at(i)->getName();
            list.append(map);
            features.append(mFeatures->at(i)->getName());
        }
        item->setProperty("featuresList", list);
        item->setProperty("features", QString(features.join("; ")));
    }

    return true;
}

bool DialogCardProductType::save()
{
    return ApiAdapter::getApi()->save(SEConsts::API_PRODUCTS_TYPES, item);
}

void DialogCardProductType::selectFeatures()
{
    FormFeatures formFeatures(this);
    formFeatures.setSelectMode();
    if (formFeatures.exec() == QDialog::Accepted) {
        const ListFeatures *list = formFeatures.selectedList();
        for (int i = 0; i < list->size(); ++i)
            mModelFeatures->addItem(new DataFeature(list->at(i)));
        mIsModifiedFeatures = true;
    }
}

void DialogCardProductType::deleteFeatures()
{
    QModelIndexList list = ui->treeView->selectionModel()->selectedRows();
    if (!list.size())
        return;

    QStringList idsDelete;
    for (int i = 0; i < list.size(); i++)
        idsDelete.append(mFeatures->at(list.at(i).row())->getId());
    for (int i = mFeatures->size() - 1; i >= 0; i--)
        if (idsDelete.contains(mFeatures->at(i)->getId()))
            delete mFeatures->takeAt(i);
    mModelFeatures->resetModel();
    mIsModifiedFeatures = true;
    if (mFeatures->size())
        ui->treeView->selectionModel()->
                setCurrentIndex(mModelFeatures->index(0, 0), QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);
}

void DialogCardProductType::currentRowChanged()
{
    ui->toolButtonDelete->setEnabled(ui->treeView->currentIndex().isValid());
}
