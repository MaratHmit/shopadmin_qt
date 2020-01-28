#ifndef FRAMEIMAGE_H
#define FRAMEIMAGE_H

#include <QWidget>
#include <seconsts.h>
#include <seconfig.h>


namespace Ui {
class FrameImage;
}

class FrameImage : public QWidget
{
    Q_OBJECT

public:
    explicit FrameImage(QWidget *parent = 0);
    ~FrameImage();

    void setSection(const int &section);
    void setImage(const QString &fileName);
    void setImageAlt(const QString &strAlt);
    void hideImageAlt();
    const QSize imageSize() const;

    static bool isScaledOriginImage(int *w, int *h);    

    QString fileImageName() const;
    QString fileImagePathName() const;
    QString altImage() const;

    void saveImage();

public slots:
    void editImage(const QString &fileName);

signals:
    void modified();
    void imageChanged(const QString &imageFile);

private slots:
    void loadImage();    
    void onImageNameChange();

    void on_toolButtonOpenImage_clicked();
    void on_toolButtonDeleteImage_clicked();
    void on_toolButtonOpenImageFromLib_clicked();

    void showImageByFullName(const QString &fullFileName);

private:
    Ui::FrameImage *ui;    
    QByteArray *bufferImage;    
    QSize mImageSize;
    void showEvent(QShowEvent *e);

    bool mIsModified;
    int mSection;        

    void loadFromServer();    

};

#endif // FRAMEIMAGE_H
