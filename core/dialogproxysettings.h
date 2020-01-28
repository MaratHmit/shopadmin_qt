#ifndef DIALOGPROXYSETTINGS_H
#define DIALOGPROXYSETTINGS_H

#include <QDialog>

namespace Ui {
class DialogProxySettings;
}

class DialogProxySettings : public QDialog
{
    Q_OBJECT

public:
    explicit DialogProxySettings(QWidget *parent = 0);
    ~DialogProxySettings();

private:
    Ui::DialogProxySettings *ui;

    void showEvent(QShowEvent *);

    void initSignals();
    void save();
};

#endif // DIALOGPROXYSETTINGS_H
