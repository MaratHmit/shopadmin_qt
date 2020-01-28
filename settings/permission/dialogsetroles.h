#ifndef DIALOGSETROLES_H
#define DIALOGSETROLES_H

#include "nametablemodel.h"
#include "dataitem.h"
#include "seconsts.h"
#include "seconfig.h"

#include <QDialog>

namespace Ui {
class DialogSetRoles;
}

class DialogSetRoles : public QDialog
{
    Q_OBJECT

public:
    DialogSetRoles(QWidget *parent = 0);
    void setModel(const NameTableModel &model, const QStringList &idsCheckedRoles);
    ~DialogSetRoles();

    const QStringList selectedIdsRoles() const;

private:
    Ui::DialogSetRoles *ui;    
    NameTableModel *model;

    void showEvent(QShowEvent *e);

};

#endif // DIALOGSETROLES_H
