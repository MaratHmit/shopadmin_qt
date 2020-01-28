#ifndef INTEGRATIONSMODEL_H
#define INTEGRATIONSMODEL_H

#include "dataitem.h"

#include <QAbstractTableModel>

class IntegrationsModel : public QAbstractTableModel
{
    Q_OBJECT

public:    
    IntegrationsModel(QObject *, ListDataItems *);
    ~IntegrationsModel();

    void setListData(ListDataItems *);
    void setIsParametersOnOffMode();
    void setIsFeaturesMode();

    int columnCount(const QModelIndex &) const;
    int rowCount(const QModelIndex &) const;
    QVariant data(const QModelIndex &, int ) const;    
    QVariant headerData(int , Qt::Orientation orientation, int ) const;
    bool setData(const QModelIndex &, const QVariant &, int );
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QModelIndex index(int , int , const QModelIndex &parent = QModelIndex()) const;
    DataItem *dataItem(const QModelIndex &index) const;

    void resetModel();
    void beginReset();
    void endReset();

    void repaintModel();

signals:
    void modified();

private:
    ListDataItems *mParameters;
    const DataItem *mRootItem;
    bool mIsParametersOnOffMode;
    bool mIsFeaturesMode;

    enum HEADER
    {        
        HEADER_NAME = 0,
        HEADER_VALUE = 1,
        HEADER_ISACTIVE = 2
    };

};

#endif // INTEGRATIONSMODEL_H
