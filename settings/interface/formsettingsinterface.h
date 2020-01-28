#ifndef FORMSETTINGSINTERFACE_H
#define FORMSETTINGSINTERFACE_H

#include <QWidget>

#include "apiadapter.h"
#include "seconfig.h"
#include "seconsts.h"
#include "nametablemodel.h"

namespace Ui {
class FormSettingsInterface;
}

class FormSettingsInterface : public QWidget
{
    Q_OBJECT

public:
    explicit FormSettingsInterface(QWidget *parent = 0);
    ~FormSettingsInterface();

private slots:

private:
    Ui::FormSettingsInterface *ui;


};

#endif // FORMSETTINGSINTERFACE_H
