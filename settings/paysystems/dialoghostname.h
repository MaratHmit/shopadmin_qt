#ifndef DIALOGHOSTNAME_H
#define DIALOGHOSTNAME_H

#include <QDialog>

namespace Ui {
class DialogHostName;
}

class DialogHostName : public QDialog
{
    Q_OBJECT

public:
    explicit DialogHostName(QWidget *parent = 0);
    ~DialogHostName();

    void setHostName(const QString &hostName);
    QString hostName() const;

private:
    Ui::DialogHostName *ui;
};

#endif // DIALOGHOSTNAME_H
