#ifndef FRAMECARDOPTIONS_H
#define FRAMECARDOPTIONS_H

#include <QDialog>

#include "nametablemodel.h"
#include "dataproduct.h"
#include "widgetpageoptions.h"
#include "dataoption.h"

namespace Ui {
class FrameCardOptions;
}

class FrameCardOptions : public QDialog
{
    Q_OBJECT

public:
    explicit FrameCardOptions(QWidget *parent, DataProduct *item,
                              const int &typeDialog = 0);
    ~FrameCardOptions();

    bool isAddOptions() const;
    void fillControls();

signals:
    void modified();

private slots:
    void onAddOptions();
    void onCopyFromOtherProduct();
    void onRemoveOption(const int &index);
    void onChecked();

    void onOk();


private:
    Ui::FrameCardOptions *ui;
    DataProduct *product;    
    int mTypeDialog;
    bool mIsFilled;

    QList<WidgetPageOptions *> *mListPages;

    void addPage(DataOption *option, const bool &isNew = false);
    WidgetPageOptions *getPageById(const QString &id) const;

    void initOptions();
    ListOptions *mOptions;

    void initSignals();
};

#endif // FRAMECARDOPTIONS_H
