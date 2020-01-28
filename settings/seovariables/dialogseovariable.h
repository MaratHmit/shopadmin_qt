#ifndef DIALOGSEOVARIABLE_H
#define DIALOGSEOVARIABLE_H

#include <QDialog>

#include "dataitem.h"

namespace Ui {
class DialogSEOVariable;
}

class DialogSEOVariable : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSEOVariable(QWidget *parent, const DataItem *item = 0);
    ~DialogSEOVariable();

    const DataItem *item() const;

public slots:
    void accept();

private:
    Ui::DialogSEOVariable *ui;

    DataItem *mItem;

    void initSignals();
    void fillControls();
};

#endif // DIALOGSEOVARIABLE_H
