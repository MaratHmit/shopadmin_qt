#ifndef INTEGRATIONSDELEGATE_H
#define INTEGRATIONSDELEGATE_H

#include "integrationsmodel.h"
#include "dataintegration.h"
#include "datafeature.h"

#include <QItemDelegate>
#include <QVector>

class IntegrationsDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit IntegrationsDelegate(QObject *parent, const IntegrationsModel *model);
    ~IntegrationsDelegate();

    void setIsParametersOnOffMode();
    void setIsFeaturesMode();

    QWidget *createEditor(QWidget *, const QStyleOptionViewItem &, const QModelIndex &) const;
    void setEditorData(QWidget *, const QModelIndex &) const;
    void setModelData(QWidget *, QAbstractItemModel *, const QModelIndex &) const;
    void paint(QPainter *, const QStyleOptionViewItem &, const QModelIndex &) const;

signals:
    void modified();

private slots:
    void onChecked(bool checked);
    void onChangeComboBox(const QString &);    

private:
    const IntegrationsModel *mModel;
    bool mIsParametersOnOffMode;
    bool mIsFeaturesMode;    

};

#endif // INTEGRATIONSDELEGATE_H
