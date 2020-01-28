#ifndef DIALOGSETURL_H
#define DIALOGSETURL_H

#include <QDialog>

namespace Ui {
class DialogSetURL;
}

class DialogSetURL : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSetURL(QWidget *parent = 0);
    ~DialogSetURL();

    void setURLName(const QString &str);
    QString URL() const;

private slots:
    void onNameEdited(const QString &arg1);
    void onURLEdited(const QString &arg1);

private:
    Ui::DialogSetURL *ui;
    bool mIsHasName;
};

#endif // DIALOGSETURL_H
