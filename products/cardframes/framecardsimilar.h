#ifndef FRAMECARDSIMILAR_H
#define FRAMECARDSIMILAR_H

#include "dataproduct.h"
#include "nametablemodel.h"
#include "field.h"

#include <QDialog>

namespace Ui {
class FrameCardSimilar;
}

class FrameCardSimilar : public QDialog
{
    Q_OBJECT

public:
    explicit FrameCardSimilar(QWidget *parent, ListProducts *products,
                               const bool &isDialogMode = false);
    ~FrameCardSimilar();
    void setIdProduct(const QString &id);

    void fillControls();    

signals:
    void modified();

private slots:
    void onAddProduct();
    void onAddGroup();
    void onDeleteProducts();
    void onChange();
    void showContextMenu(QPoint point);

private:
    Ui::FrameCardSimilar *ui;
    QString mIdProduct;
    ListProducts *mProducts;
    NameTableModel *mModel;
    bool mIsFilled;
    bool mIsDialogMode;
    QDialog *mFormProducts;
    QVector<Field *> *mFields;

    void initProducts();
    void initTree();
    void initSignals();
    void initActions();
};

#endif // FRAMECARDSIMILAR_H
