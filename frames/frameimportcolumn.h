#ifndef FRAMEIMPORTCOLUMN_H
#define FRAMEIMPORTCOLUMN_H

#include "dataitem.h"

#include <QWidget>

namespace Ui {
class FrameImportColumn;
}

class FrameImportColumn : public QWidget
{
    Q_OBJECT

public:
    explicit FrameImportColumn(QWidget *parent, DataItem *item, QStringList fields);
    ~FrameImportColumn();

    const QString getFieldName();
    void setCurrentValue(const QString &value);

private:
    Ui::FrameImportColumn *ui;

    DataItem *item;
};

#endif // FRAMEIMPORTCOLUMN_H
