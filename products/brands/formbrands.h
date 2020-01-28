#ifndef FORMBRANDS_H
#define FORMBRANDS_H

#include <QDialog>
#include <QMenu>
#include <QTimer>

#include "nametablemodel.h"
#include "request.h"
#include "field.h"

namespace Ui {
class FormBrands;
}

class FormBrands : public QDialog
{
    Q_OBJECT
public:
    explicit FormBrands(QWidget *parent = 0);
    ~FormBrands();

    void setSelectMode(const bool &isMultiSelect);
    ListDataItems *selectedBrands() const;

signals:

protected:
    void showEvent(QShowEvent *e);    

public slots:
    void refreshData();
    void onSaveBrand(const QString &id);

private slots:
    void onChangeBrand();
    void onAddEditBrand();    
    void showContextMenu(QPoint point);

    void onSort();
    void saveSort();

    void onRemoveBrand();

private:
    Ui::FormBrands *ui;
    bool mIsShowed;
    bool mIsLoadedLayouts;
    bool mIsSelectMode;
    Request mRequest;
    ListDataItems *mSelectedBrands;
    QVector<Field *> mFields;
    QTimer *timerSort;

    bool isAddPermission;
    bool isEditPermission;
    bool isDeletePermission;

    void resetRequest();
    NameTableModel *model;
    ListDataItems *brands;

    void initFields();
    void initTreeBrands();
    void initActions();
    void initActionsSignals();
    void initPermission();
    void goToRowById(QString id);

    QString currentId();

};

#endif // FORMBRANDS_H
