#ifndef PRODUCTSGROUPSMODEL_H
#define PRODUCTSGROUPSMODEL_H

#include "nametablemodel.h"
#include "dataproductsgroup.h"

class ProductsGroupsModel : public NameTableModel
{
public:
    explicit ProductsGroupsModel(QObject *parent = 0);
    ~ProductsGroupsModel();

    void setSimpleMode(const bool &simpleMode);

    int columnCount(const QModelIndex &parent) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    bool hasChildren(const QModelIndex &parent) const;

    void setupModelData();
    QStringList idsChecked() const;

    ListProductsGroups *listGroups() const;
    QModelIndexList indexList() const;
    QModelIndex getModelIndexById(const QString &id);
    DataProductsGroup *currentGroup(QModelIndex &index) const;

    void insertGroup(const DataProductsGroup *group);
    void editGroup(const DataProductsGroup *group);

private:
    bool mSimpleMode;
    DataProductsGroup *mRootItem;
    DataProductsGroup *findGroup(DataProductsGroup *parent, const QString &id) const;
    void removeItems(DataProductsGroup *parent, QList<QString> &listIds);
    QStringList idsCheckedChilds(DataProductsGroup *data) const;

    enum HEADER
    {
        HEADER_NAME = 0,
        HEADER_CODE = 1,
        HEADER_NOTE = 2
    };
};

#endif // PRODUCTSGROUPSMODEL_H
