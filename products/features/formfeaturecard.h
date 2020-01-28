#ifndef FORMFEATURECARD_H
#define FORMFEATURECARD_H

#include <QDialog>

#include "datafeature.h"

namespace Ui {
class FormFeatureCard;
}

class FormFeatureCard : public QDialog
{
    Q_OBJECT

public:
    FormFeatureCard(QWidget *parent = 0);
    FormFeatureCard(QWidget *parent, const bool &isModificationMode);
    ~FormFeatureCard();
    void setId(const QString &id);

private slots:
    void on_pushButtonMainInfo_clicked();
    void on_pushButtonNote_clicked();
    void on_buttonSave_clicked();
    void on_toolButtonClearParentGroup_clicked();
    void on_toolButtonParentGroup_clicked();
    void on_lineEditName_textChanged(const QString &arg1);

private:
    void showEvent(QShowEvent *e);
    void initialization();

    Ui::FormFeatureCard *ui;
    ListDataItems *mListTypesFeatures;

    void getData();
    void setData();
    void fillControls();
    bool save();
    bool mIsModificationsMode;
    int getIndexTypeFeature(const QString &valueType) const;
    QString getTypeFeature(const QString &id) const;

    void initTypesFeatures();

    DataFeature *mFeature;
};

#endif // FORMFEATURECARD_H
