#ifndef FRAMEIMAGEDESCRIPTION_H
#define FRAMEIMAGEDESCRIPTION_H

#include <QWidget>

#include "dataitem.h"

namespace Ui {
class FrameImageDescription;
}

class FrameImageDescription : public QWidget
{
    Q_OBJECT

public:
    explicit FrameImageDescription(QWidget *parent = 0);
    ~FrameImageDescription();

    void setImage(DataItem *image);
    DataItem *image() const;

signals:
    void modified();

private slots:
    void onChangeImageAlt(const QString &arg);

private:
    Ui::FrameImageDescription *ui;
    DataItem *mImage;

    QString fileShortName(QString const &fileName);
};

#endif // FRAMEIMAGEDESCRIPTION_H
