#include "seutils.h"

#include <QUuid>
#include <QFileInfo>
#include <QStringList>
#include <QByteArray>

SeUtils::SeUtils()
{
}

const char asv[] = {'C', '6', 'd', '7', 'D', '8', 'e', '9', 'E', 'h', 'f', 'H', 'F',
                    '0', 'a', '1', 'A', '2', 'b', '3', 'B', '4', 'c', '5', 'u', 'U',
                    'g', 'G', 's', 'S', 'i', 'I', 'o', 'O', 'p', 'P', 'w', 'W', 'j',
                    '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '=',
                    '{', '}', '|', '\\', '/', ',', '.', '>', '<', '?','"', '\''};


QString QMD5(QString data)
{
    QCryptographicHash md5(QCryptographicHash::Md5);
    md5.addData(data.toUtf8());
    return md5.result().toHex().constData();
}

QByteArray setCryptoLatin(QByteArray &arg1)
{
    QByteArray d;
    int r, r1, r2;

    for (int i = 0; i < arg1.size(); i++) {
        r = 20 + qrand() % 43;
        r2 = arg1.at(i) % r;
        r1 = int ((arg1.at(i) - r2) / r);
        d.append(asv[r]);
        d.append(asv[r1]);
        d.append(asv[r2]);
    }
    return d;
}


QByteArray getCryptoLatin(QByteArray &arg1)
{
    QByteArray d;
    int j;
    int r = 0, r1 = 0, r2 = 0;

    for (int i = 0; i < arg1.size() - 1; i = i + 3) {
        for (j = 0; j < 64; j++)
            if (arg1[i] == asv[j]) {
                r = j;
                break;
            }
        for (j = 0; j < 64; j++)
            if (arg1[i + 1] == asv[j]) {
                r1 = j;
                break;
            }
        for (j = 0; j < 64; j++)
            if (arg1[i + 2] == asv[j]) {
                r2 = j;
                break;
            }
        d.append(r1 * r + r2);
    }
    return d;
}

QString getShortGUID()
{
    QString guidStr = QUuid::createUuid().toString();
    QString answer("");

    for (int i = 0; i < guidStr.size(); i++) {
        if ((guidStr.at(i) != '-') && (guidStr.at(i) != '{')
                && (guidStr.at(i) != '}'))
            answer += guidStr.at(i);
    }

    return answer;
}


bool correctFileName(const QString &arg1)
{
    bool flag(true);

    for (int i = 0; i < arg1.size(); i++) {
        flag = flag && (arg1.at(i) < 'z') && (arg1.at(i) != ' ');
        if (!flag)
            return false;
    }

    return true;
}

qint64 getSizeFiles(const QStringList *list)
{
    qint64 answer(0);

    foreach (QString fileName, *list) {
        QFileInfo fileInfo(fileName);
        answer += fileInfo.size();
    }

    return answer;
}


QString tranlsitGOST(const QString &str)
{
    QString result("");
    int rU, rL;
    QString validChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890-_,.()[]{}<>~!@#$%^&*+=?";
    QString rusUpper = "АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ";
    QString rusLower = "абвгдеёжзийклмнопрстуфхцчшщъыьэюя";
    QStringList latUpper, latLower;
    latUpper <<"A"<<"B"<<"V"<<"G"<<"D"<<"E"<<"JO"<<"ZH"<<"Z"<<"I"<<"J"<<"K"<<"L"<<"M"<<"N"
            <<"O"<<"P"<<"R"<<"S"<<"T"<<"U"<<"F"<<"H"<<"CZ"<<"CH"<<"SH"<<"SHH"<<"``"<<"Y"
           <<"`"<<"E`"<<"YU"<<"YA";
    latLower <<"a"<<"b"<<"v"<<"g"<<"d"<<"e"<<"jo"<<"zh"<<"z"<<"i"<<"j"<<"k"<<"l"<<"m"<<"n"
            <<"o"<<"p"<<"r"<<"s"<<"t"<<"u"<<"f"<<"h"<<"cz"<<"ch"<<"sh"<<"shh"<<"``"<<"y"
           <<"`"<<"e`"<<"ju"<<"ja";
    for (int i = 0; i < str.size(); i++){
        if ( validChars.contains(str[i]) ){
            result += str[i];
        } else if (str[i] == ' ') {
            result += "-";
        } else {
            rU = rusUpper.indexOf(str[i]);
            rL = rusLower.indexOf(str[i]);
            if (rU > 0)
                result += latUpper[rU];
            else if (rL > 0)
                result += latLower[rL];
        }
    }
    return result;
}


QString base64Encode(QString string)
{
    QByteArray ba;
    ba.append(string);
    return ba.toBase64();
}


QString base64Decode(QString string)
{
    QByteArray ba;
    ba.append(string);
    return QByteArray::fromBase64(ba);
}


QString escapedReservsChars(const QString &string)
{
    QString result = string;
    result.replace("+", "%2B");
    result.replace("=", "%3D");

    return result;
}


QString formatNumberString(const QString &string)
{
    QString s("");
    for (int i = 0; i < string.length(); ++i) {
        if (i && (string.length() - i) % 3 == 0)
            s += " ";
        s += string.at(i);
    }

    return s;
}
