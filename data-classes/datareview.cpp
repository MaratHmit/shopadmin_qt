#include "datareview.h"

DataReview::DataReview()
{
    initialization();
}

DataReview::DataReview(const DataReview *d)
{
    initialization();
    copy(d);
}

DataReview::DataReview(const QJsonObject &jsonobject)
{
    initialization();
    setFromJSONObject(jsonobject);
}

void DataReview::initialization()
{
    DataItem::initialization();

    mMark = 0;
    mUseTime = 1;
    mCountLikes = 0;
    mCountDislikes = 0;
    mDateTime = QDateTime::currentDateTime();
    initStars();
}

void DataReview::setFromJSONObject(const QJsonObject &jsonobject)
{
    DataItem::setFromJSONObject(jsonobject);

    if (!jsonobject["idProduct"].isNull())
        mIdProduct = jsonobject["idProduct"].toString();
    if (!jsonobject["idUser"].isNull())
        mIdUser = jsonobject["idUser"].toString();
    if (!jsonobject["nameProduct"].isNull())
        mNameProduct = jsonobject["nameProduct"].toString();
    if (!jsonobject["nameUser"].isNull())
        mNameUser = jsonobject["nameUser"].toString();
    if (!jsonobject["mark"].isNull())
        mMark = jsonobject["mark"].toInt();
    if (!jsonobject["merits"].isNull())
        mMerits = jsonobject["merits"].toString();
    if (!jsonobject["demerits"].isNull())
        mDemerits = jsonobject["demerits"].toString();
    if (!jsonobject["comment"].isNull())
        mComment = jsonobject["comment"].toString();
    if (!jsonobject["useTime"].isNull())
        mUseTime = jsonobject["useTime"].toInt();
    if (!jsonobject["dateTime"].isNull())
        mDateTime = QDateTime::fromString(jsonobject["dateTime"].toString(), "yyyy-MM-dd hh:mm:ss");
    if (!jsonobject["countLikes"].isNull())
        mCountLikes = jsonobject["countLikes"].toInt();
    if (!jsonobject["countDislikes"].isNull())
        mCountDislikes = jsonobject["countDislikes"].toInt();
}

QJsonObject DataReview::getAsJsonObject() const
{
    QJsonObject obj = DataItem::getAsJsonObject();

    if (isSetAllFields || modifiedFields.contains("idProduct") || modifiedFields.contains("idUser"))
        obj.insert("idProduct", QJsonValue(mIdProduct));
    if (isSetAllFields || modifiedFields.contains("idUser") || modifiedFields.contains("idProduct"))
        obj.insert("idUser", QJsonValue(mIdUser));
    if (isSetAllFields || modifiedFields.contains("mark"))
        obj.insert("mark", QJsonValue(mMark));
    if (isSetAllFields || modifiedFields.contains("merits"))
        obj.insert("merits", QJsonValue(mMerits));
    if (isSetAllFields || modifiedFields.contains("demerits"))
        obj.insert("demerits", QJsonValue(mDemerits));
    if (isSetAllFields || modifiedFields.contains("comment"))
        obj.insert("comment", QJsonValue(mComment));
    if (isSetAllFields || modifiedFields.contains("useTime"))
        obj.insert("useTime", QJsonValue(mUseTime));
    if (isSetAllFields || modifiedFields.contains("dateTime"))
        obj.insert("dateTime", QJsonValue(mDateTime.toString("yyyy-MM-dd hh:mm:ss")));
    if (isSetAllFields || modifiedFields.contains("countLikes"))
        obj.insert("countLikes", QJsonValue(mCountLikes));
    if (isSetAllFields || modifiedFields.contains("countDislikes"))
        obj.insert("countDislikes", QJsonValue(mCountDislikes));

    return obj;
}

void DataReview::clearData()
{
    DataItem::clearData();

    mIdProduct.clear();
    mIdUser.clear();
    mNameUser.clear();
    mNameProduct.clear();
    mMark = 0;
    mMerits.clear();
    mDemerits.clear();
    mComment.clear();
    mUseTime = 0;
    mDateTime = QDateTime::currentDateTime();
    mCountLikes = 0;
    mCountDislikes = 0;
}

void DataReview::copy(const DataReview *d)
{
    DataItem::copy(d);

    mIdProduct = d->mIdProduct;
    mIdUser = d->mIdUser;
    mNameUser = d->mNameUser;
    mNameProduct = d->mNameProduct;
    mMark = d->mMark;
    mMerits = d->mMerits;
    mDemerits = d->mDemerits;
    mComment = d->mComment;
    mUseTime = d->mUseTime;
    mDateTime = d->mDateTime;
    mCountLikes = d->mCountLikes;
    mCountDislikes = d->mCountDislikes;

    setUnModified();
}

void DataReview::copy(const DataItem *d)
{
    copy(dynamic_cast <const DataReview *> (d));
}

const QString DataReview::idProduct() const
{
    return mIdProduct;
}

const QString DataReview::idUser() const
{
    return mIdUser;
}

const QString DataReview::nameUser() const
{
    return mNameUser;
}

const QString DataReview::nameProduct() const
{
    return mNameProduct;
}

int DataReview::mark() const
{
    return mMark;
}

const QString DataReview::merits() const
{
    return mMerits;
}

const QString DataReview::demerits() const
{
    return mDemerits;
}

const QString DataReview::comment() const
{
    return mComment;
}

int DataReview::useTime() const
{
    return mUseTime;
}

const QDateTime DataReview::dateTime() const
{
    return mDateTime;
}

int DataReview::countLikes() const
{
    return mCountLikes;
}

int DataReview::countDislikes() const
{
    return mCountDislikes;
}

void DataReview::setIdProduct(const QString &idProduct)
{
    if (mIdProduct != idProduct) {
        mIdProduct = idProduct;
        setModified("idProduct");
    }
}

void DataReview::setIdUser(const QString &idUser)
{
    if (mIdUser != idUser) {
        mIdUser = idUser;
        setModified("idUser");
    }
}

void DataReview::setMark(int mark)
{
    if (mMark != mark) {
        mMark = mark;
        setModified("mark");
    }
}

void DataReview::setMerits(const QString &merits)
{
    if (mMerits != merits) {
        mMerits = merits;
        setModified("merits");
    }
}

void DataReview::setDemerits(const QString &demerits)
{
    if (mDemerits != demerits) {
        mDemerits = demerits;
        setModified("demerits");
    }
}

void DataReview::setComment(const QString &comment)
{
    if (mComment != comment) {
        mComment = comment;
        setModified("comment");
    }
}

void DataReview::setUseTime(int useTime)
{
    if (mUseTime != useTime) {
        mUseTime = useTime;
        setModified("useTime");
    }
}

void DataReview::setDateTime(const QDateTime &dateTime)
{
    if (mDateTime != dateTime) {
        mDateTime = dateTime;
        setModified("dateTime");
    }
}

void DataReview::setCountLikes(int countLikes)
{
    if (mCountLikes != countLikes) {
        mCountLikes = countLikes;
        setModified("countLikes");
    }
}

void DataReview::setCountDislikes(int countDislikes)
{
    if (mCountDislikes != countDislikes) {
        mCountDislikes = countDislikes;
        setModified("countDislikes");
    }
}

const QString DataReview::dateTimeDisplay() const
{
    return mDateTime.toString("dd.MM.yyyy hh:mm");
}

const QPixmap DataReview::markIco() const
{
    if (mMark < mStars.size())
        return mStars.at(mMark);
    return QPixmap();
}

const QString DataReview::useTimeDisplay() const
{
    switch (mUseTime) {
    case 1:
        return tr("Менее месяца");
    case 2:
        return tr("Несколько месяцев");
    case 3:
        return tr("Более года");
    default:
        break;
    }
    return QString();
}

void DataReview::initStars()
{
    for (int i = 0; i < 6; i++) {
        QString nameIco = ":/16/icons/16x16/start_" + QString::number(i) + ".png";
        mStars.append(QPixmap(nameIco));
    }
}

