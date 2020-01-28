#ifndef GROUPSPECIFICATIONS_H
#define GROUPSPECIFICATIONS_H

#include <QHBoxLayout>
#include <QGroupBox>
#include <QTableView>

#include "datafeature.h"
#include "integrationsmodel.h"
#include "integrationsdelegate.h"
#include "dataintegration.h"

class GroupSpecifications : public QWidget
{
    Q_OBJECT
public:
    explicit GroupSpecifications(QWidget *parent = 0);    

    void setId(const QString &id);
    QString id() const;
    void addSpecification(DataFeature*specification, const bool &isResetModel);
    void deleteSpecification();
    void resetModel();
    void clear();
    const ListFeatures *listItems() const;

signals:    
    void modified();
    void showContextMenu(const QPoint &pos);
    void deleteGroupSpecification(GroupSpecifications *);

public slots:

private slots:
    void onModified();    

private:  
    QString mId;        
    QTableView *mTableSpecifications;
    IntegrationsModel *mModelItems;
    IntegrationsDelegate *mDelegateItems;
    ListFeatures *mListItems;    

    void createTreeView();
};

#endif // GROUPSPECIFICATIONS_H
