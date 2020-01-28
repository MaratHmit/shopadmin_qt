#ifndef SEUTILS_H
#define SEUTILS_H

#include <QCryptographicHash>
#include <QString>

class SeUtils
{
public:
    SeUtils();
};

QString QMD5(QString data);

  // Запаролить строку
QByteArray setCryptoLatin(QByteArray &arg1);
QByteArray getCryptoLatin(QByteArray &arg1);

  // Уникальное имя
QString getShortGUID();

  // Проверка имени файла на корректность
bool correctFileName(const QString &arg1);

  // Общий размер файлов
qint64 getSizeFiles(const QStringList *list);

QString tranlsitGOST(const QString &str);

QString base64Encode(QString string);
QString base64Decode(QString string);

QString escapedReservsChars(const QString &string);

QString formatNumberString(const QString &string);


#endif // SEUTILS_H
