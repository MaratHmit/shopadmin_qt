#ifndef FORMSETVISIBLEHEADERCOLUMNS_H
#define FORMSETVISIBLEHEADERCOLUMNS_H

#include <QDialog>
#include <QHeaderView>

#include "nametablemodel.h"

namespace Ui {
class FormSetVisibleHeaderColumns;
}

class FormSetVisibleHeaderColumns : public QDialog
{
    Q_OBJECT

public:
    explicit FormSetVisibleHeaderColumns(QWidget *parent, QHeaderView *header);
    ~FormSetVisibleHeaderColumns();

private:
    void showEvent(QShowEvent *e);
    Ui::FormSetVisibleHeaderColumns *ui;
    QHeaderView *mHeader;

    void setColumns();
    void initList();
    void initSignals();
};

#endif // FORMSETVISIBLEHEADERCOLUMNS_H
