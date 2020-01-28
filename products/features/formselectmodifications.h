#ifndef FORMSELECTMODIFICATIONS_H
#define FORMSELECTMODIFICATIONS_H

#include <QDialog>

#include "datamodificationgroup.h"
#include "formfeatureslist.h"

namespace Ui {
class FormSelectModifications;
}

class FormSelectModifications : public QDialog
{
    Q_OBJECT

public:
    explicit FormSelectModifications(QWidget *parent,
        DataModificationGroup *group, const bool isStealthMode = false);
    ~FormSelectModifications();

    const ListModifications *selectedList() const;    
    bool setSelectedList();

private slots:
    void onApply();

private:
    Ui::FormSelectModifications *ui;
    DataModificationGroup *mGroup;
    QList<FormFeaturesList *> *mFormsColumns;
    ListModifications *mSelectedList;

    bool mIsStealthMode;

    void initSignals();
    void initColumns();   
};

#endif // FORMSELECTMODIFICATIONS_H
