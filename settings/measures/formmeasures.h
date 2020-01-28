#ifndef FORMMEASURES_H
#define FORMMEASURES_H

#include "formcatalog.h"
#include "dialogcardmeasure.h"

#include <QWidget>

namespace Ui {
class FormMeasures;
}

class FormMeasures : public QWidget
{
    Q_OBJECT

public:
    explicit FormMeasures(QWidget *parent = 0);
    ~FormMeasures();

private:
    Ui::FormMeasures *ui;

    void initSignals();
    void initCatalogs();

    FormCatalog *mFormMeasuresWeight;
    FormCatalog *mFormMeasuresVolume;
};

#endif // FORMMEASURES_H
