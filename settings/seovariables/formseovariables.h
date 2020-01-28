#ifndef FORMSEOVARIABLES_H
#define FORMSEOVARIABLES_H

#include <QDialog>

#include "dataitem.h"
#include "nametablemodel.h"

namespace Ui {
class FormSEOVariables;
}

class FormSEOVariables : public QDialog
{
    Q_OBJECT

public:
    explicit FormSEOVariables(QWidget *parent, const bool &isSelectMode = false);
    ~FormSEOVariables();

public slots:
    void accept();

private slots:
    void showContextMenu(QPoint point);
    void currentRowChanged();
    void refreshData();

    void addEditItem();
    void deleteItem();

private:
    Ui::FormSEOVariables *ui;
    bool mIsSelectMode;
    ListDataItems *mItems;
    NameTableModel *mModel;

    void showEvent(QShowEvent *e);

    void goToRowById(const QString &id);

    void initControls();
    void initVariables();
    void initSignals();
};

#endif // FORMSEOVARIABLES_H
