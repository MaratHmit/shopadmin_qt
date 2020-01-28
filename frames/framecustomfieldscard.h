#ifndef FRAMECUSTOMFIELDSCARD_H
#define FRAMECUSTOMFIELDSCARD_H

#include "dataimages.h"

#include <QWidget>

namespace Ui {
class FrameCustomFieldsCard;
}

class FrameCustomFieldsCard : public QWidget
{
    Q_OBJECT

public:
    explicit FrameCustomFieldsCard(QWidget *parent = 0);
    ~FrameCustomFieldsCard();

    void setItem(const DataImages *item);
    void setDataType(const QString &dataType);
    int countFields() const;

signals:
    void modified();

private slots:
    void onChangeDynFields();

private:
    Ui::FrameCustomFieldsCard *ui;

    ListDataItems *mListDynFields;
    ListDataItems *mListDynFieldGroups;
    QString dataType;
    const DataImages *item;
    bool isShowed;

    const DataItem *getDynFieldById(const QString &id) const;

    void showEvent(QShowEvent *event);

    void addDynFieldGroup(const DataItem *fieldGroup);
    void addDynField(const DataItem *field);

    void createDynFields();
    void createDynFieldsGroups();

    void initVariables();
    void initDynFields();

};

#endif // FRAMECUSTOMFIELDSCARD_H
