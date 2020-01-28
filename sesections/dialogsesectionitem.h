#ifndef DIALOGSESECTIONITEM_H
#define DIALOGSESECTIONITEM_H

#include <QDialog>

#include "dataitem.h"
#include "formproducts.h"
#include "formproductsgroups.h"
#include "formbrands.h"
#include "formpublications.h"

namespace Ui {
class DialogSESectionItem;
}

class DialogSESectionItem : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSESectionItem(QWidget *parent, const DataItem *item);
    void setIdSection(const QString &idSection);
    const DataItem *sectionItem() const;

    ~DialogSESectionItem();

private slots:
    void onChangeType();
    void onSelectItem();
    void onChangedData();
    void onChangedImage();
    void onChangedTab(int index);

private:
    Ui::DialogSESectionItem *ui;
    DataItem *mSectionItem;
    bool isModifiedImage;
    bool isShowedNote;
    QString mIdSection;

    void accept();
    void showEvent(QShowEvent *);
    void timerEvent(QTimerEvent *event);

    void fillControls();
    void fillData();
    void initVariables();
    void initSignals();
    bool save();
    void setImageFromItem(const DataItem &item);
};

#endif // DIALOGSESECTIONITEM_H
