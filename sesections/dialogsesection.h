#ifndef DIALOGSESECTION_H
#define DIALOGSESECTION_H

#include <QDialog>

#include "seconfig.h"
#include "apiadapter.h"
#include "nametablemodel.h"

namespace Ui {
class DialogSESection;
}

class DialogSESection : public QDialog
{
    Q_OBJECT

public:
    DialogSESection(QWidget *parent, const DataItem *section);
    ~DialogSESection();

    const DataItem *section() const;

protected slots:
    void accept();

private slots:
    void onEditName();

private:
    void showEvent(QShowEvent *);

    Ui::DialogSESection *ui;
    DataItem *mSection;

    void initSignals();
    void fillData();
    void fillControls();
    bool save();
};

#endif // DIALOGSESECTION_H
