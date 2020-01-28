#ifndef DIALOGINSERTIMAGE_H
#define DIALOGINSERTIMAGE_H

#include <QDialog>

namespace Ui {
class DialogInsertImage;
}

class DialogInsertImage : public QDialog
{
    Q_OBJECT

public:
    explicit DialogInsertImage(QWidget *parent = 0);
    ~DialogInsertImage();

    void setSection(const int &section);
    void setImageCode(const QString &imageSrc, const QString &imageHeight,
                      const QString &imageWidth, const QString &imageStyle);
    const QString getURL() const;

private slots:
    void saveImage();
    void onImageChange();
    void onTabChange(const int &index);

private:
    Ui::DialogInsertImage *ui;        
    QSize mImageSize;
    int mSection;

    void initSignals();
    void setValidators();
};

#endif // DIALOGINSERTIMAGE_H
