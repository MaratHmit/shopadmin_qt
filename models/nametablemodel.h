#ifndef NAMETABLEMODEL_H
#define NAMETABLEMODEL_H

#include <QAbstractTableModel>
#include <QColor>
#include <QBrush>
#include <QVector>
#include <QMap>

#include "field.h"
#include "dataitem.h"
#include "request.h"

class NameTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    NameTableModel(QObject *parent = 0);
    NameTableModel(QObject *parent, ListDataItems *items, const bool &isSelectMode = false);
    ~NameTableModel();

    int rowCount(const QModelIndex &parent) const;
    int rowCount() const;
    int columnCount(const QModelIndex &parent) const;
    int columnCount() const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;    
    bool hasChildren(const QModelIndex &parent) const;    
    Qt::DropActions supportedDropActions() const;
    QMimeData *mimeData(const QModelIndexList &indexes) const;
    QStringList mimeTypes() const;
    bool dropMimeData(const QMimeData *data,
         Qt::DropAction action, int row, int column, const QModelIndex &parent);
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;        

    QString idData(const QModelIndex &index) const;
    QString nameData(const QModelIndex &index) const;
    virtual DataItem *dataItem(const QModelIndex &index) const;
    QModelIndex getIndexByName(const QString &name) const;
    QModelIndexList indexList() const;
    const QModelIndex getModelIndexById(const QString &id);    
    void addItem(DataItem *item, const bool &isLast = true);
    void moveItem(const QModelIndex &sourceIndex, DataItem *distItem);
    void deleteItem(DataItem *item);

    void setImageSize(const QSize &sizeImage);
    void setEditableMode(const bool &isOn);
    void setTitleHeader(const int &index, const QString &title);
    void setIsHandleEmtyIditem(const bool isHandle);

    virtual QStringList idsChecked() const;
    Request getSortParamsForApi();
    void setListData(ListDataItems *list);

    ListDataItems *getItems() const;

    void appendField(Field *field);
    void prependField(Field *field);
    void appendFields(const QVector<Field *> *fields);
    void clearFields();
    void setCheckedMode(const bool &isCheckable);
    void beginReset();
    void endReset();            
    void unBindItems();

    void deleteItems(QModelIndexList &list);

    const QModelIndexList getPersistentIndexList() const;
    const QVector<Field *> getFields() const;

signals:
    void checked(const QString &id, const bool &isChecked);

public slots:
    void resetModel();

protected slots:    
    void finishDownloadImage(const QString &fileImage, const QModelIndex &modelIndex);

protected:
    bool isCheckable;
    ListDataItems *items;
    virtual const QPixmap getImage(const DataItem &item, const QModelIndex &index) const;
    void downloadImage(const QString &urlImage, const QModelIndex &modelIndex) const;
    virtual void initVariables();

    QStringList idsCheckedChilds(DataItem *data) const;
    const DataItem *findItem(const DataItem *parent, const QString &id) const;    

    void unselectedChilds(QModelIndexList *list);
    bool existParentInSelectedList(const QModelIndex &parent, const QModelIndexList &list);

    QSize sizeImage;
    bool isSelectMode;
    bool isEditableMode;    
    bool isHandleEmtyIdItem;

    const DataItem *rootItem;
    QVector<Field *> fields;

};

#endif // NAMETABLEMODEL_H
