#ifndef PERMISSIONSMODEL_H
#define PERMISSIONSMODEL_H

#include <QAbstractTableModel>

#include "dataitem.h"
#include "api.h"
#include "request.h"


class PermissionsModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit PermissionsModel(QObject *parent = 0);
    ~PermissionsModel();

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole) override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    void refreshData(const QString &idRole);

signals:
    void completedRefreshData(const bool status, const QString message);

public slots:
    void clicked(const QModelIndex &index);
    void headerClicked(int column);

private slots:
    void finishedRefreshData(const bool status, const QString message);

private:
    ListDataItems *items;
    Api *apiFetch;
    Api *apiSave;
    Request request;
    QString idRole;
    void save(DataItem &item);

};

#endif // PERMISSIONSMODEL_H
