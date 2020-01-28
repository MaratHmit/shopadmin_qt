#ifndef DIALOGAUTHCHECK_H
#define DIALOGAUTHCHECK_H

#include <QDialog>

namespace Ui {
class DialogAuthCheck;
}

class DialogAuthCheck : public QDialog
{
    Q_OBJECT

public:
    DialogAuthCheck(QWidget *parent, const QString &project, const QString &serial);
    ~DialogAuthCheck();

private:
    Ui::DialogAuthCheck *ui;

    void accept();
};

#endif // DIALOGAUTHCHECK_H
