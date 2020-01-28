#ifndef FORMFEATUREVALUECARD_H
#define FORMFEATUREVALUECARD_H

#include <QColorDialog>

#include "datafeature.h"

namespace Ui {
class FormFeatureValueCard;
}

class FormFeatureValueCard : public QDialog
{
    Q_OBJECT

public:
    explicit FormFeatureValueCard(QWidget *parent, const bool &isColorMode = false);
    ~FormFeatureValueCard();

    void setIdFeature(const QString &idFeature);
    void setId(const QString &id);
    QString idValue() const;

private slots:
    void on_pushButtonMainInfo_clicked();
    void on_pushButtonImage_clicked();
    void on_toolButton_clicked();
    void on_lineEditName_textChanged(const QString &arg1);
    void on_buttonSave_clicked();
    void on_buttonCancel_clicked();
    void on_lineEditColor_textChanged(const QString &arg1);

private:
    void showEvent(QShowEvent *e);

    Ui::FormFeatureValueCard *ui;
    QColorDialog *dialogColor;
    QColor mCurrentColor;
    bool mIsColorMode;

    DataFeature *mFeatureValue;

    void getData();
    void setData();
    void fillControls();
    bool save();

    void initRegValidators();
    void setColor(const QColor &color);

};

#endif // FORMFEATUREVALUECARD_H
