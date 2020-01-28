#ifndef DIALOGSETIMAGEURL_H
#define DIALOGSETIMAGEURL_H

#include <QDialog>

namespace Ui {
class DialogSetImageUrl;
}

class DialogSetImageUrl : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSetImageUrl(QWidget *parent = 0);
    ~DialogSetImageUrl();

    const QString imageURL() const;

private:
    Ui::DialogSetImageUrl *ui;

    void checkEnabled();
};

#endif // DIALOGSETIMAGEURL_H
