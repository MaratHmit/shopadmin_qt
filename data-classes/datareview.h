#ifndef DATAREVIEW_H
#define DATAREVIEW_H

#include <QDateTime>

#include "dataitem.h"

class DataReview : public DataItem
{
    Q_OBJECT
    Q_PROPERTY(QString dateTime READ dateTimeDisplay)
    Q_PROPERTY(QString nameProduct READ nameProduct)
    Q_PROPERTY(QString nameUser READ nameUser)
    Q_PROPERTY(QPixmap mark READ markIco)
    Q_PROPERTY(QString merits READ merits)
    Q_PROPERTY(QString demerits READ demerits)
    Q_PROPERTY(QString comment READ comment)
    Q_PROPERTY(QString useTime READ useTimeDisplay)
    Q_PROPERTY(int countLikes READ countLikes)
    Q_PROPERTY(int countDislikes READ countDislikes)

public:
    DataReview();
    DataReview(const DataReview *d);
    DataReview(const QJsonObject &jsonobject);

    void initialization();
    void setFromJSONObject(const QJsonObject &jsonobject);
    QJsonObject getAsJsonObject() const;
    void clearData();
    void copy(const DataReview *d);
    void copy(const DataItem *d);

    const QString idProduct() const;
    const QString idUser() const;
    const QString nameUser() const;
    const QString nameProduct() const;
    int mark() const;
    const QString merits() const;
    const QString demerits() const;
    const QString comment() const;
    int useTime() const;
    const QDateTime dateTime() const;
    int countLikes() const;
    int countDislikes() const;

    const QString dateTimeDisplay() const;
    const QPixmap markIco() const;
    const QString useTimeDisplay() const;

public slots:
    void setIdProduct(const QString &idProduct);
    void setIdUser(const QString &idUser);
    void setMark(int mark);
    void setMerits(const QString &merits);
    void setDemerits(const QString &demerits);
    void setComment(const QString &comment);
    void setUseTime(int useTime);
    void setDateTime(const QDateTime &dateTime);
    void setCountLikes(int countLikes);
    void setCountDislikes(int counDislikes);

private:
    QString mIdProduct;
    QString mIdUser;
    QString mNameUser;
    QString mNameProduct;
    int mMark;
    QString mMerits;
    QString mDemerits;
    QString mComment;
    int mUseTime;
    QDateTime mDateTime;
    int mCountLikes;
    int mCountDislikes;
    QVector<QPixmap> mStars;

    void initStars();

};

typedef SEVector<DataReview *> ListReviews;

#endif // DATAREVIEW_H
