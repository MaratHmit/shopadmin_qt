#ifndef WIDGETPAGEOPTIONS_H
#define WIDGETPAGEOPTIONS_H

#include <QWidget>

#include "dataoption.h"
#include "formoptions.h"
#include "nametablemodel.h"

namespace Ui {
class WidgetPageOptions;
}

class WidgetPageOptions : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetPageOptions(QWidget *parent, DataOption *option, bool isSelectMode = false);
    ~WidgetPageOptions();

    DataOption *optionInfo() const;
    QString currentId() const;    

    void setPosition(int pos);

signals:
    void modified();
    void checked();

private slots:
    void onUpDown();
    void onChange();

    void on_toolButtonSelectOptionValues_clicked();
    void on_toolButtonDelete_clicked();

    void changePosition(int pos);

private:
    Ui::WidgetPageOptions *ui;

    DataOption *mOption;
    bool mIsSelectMode;

    FormOptions *mFormOptions;
    NameTableModel *mModel;

    void initTreeView();
    void goToRowById(QString id);

};

#endif // WIDGETPAGEOPTIONS_H
