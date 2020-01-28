#ifndef CSVREADER_H
#define CSVREADER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QFile>


class QTextStream;

class CsvReader: public QObject
{
    Q_OBJECT

public:
    CsvReader(QObject *parent = 0, const QString& fileName = QString(""));
    ~CsvReader();

    void setFileName(const QString& name) { mFile.setFileName(name); }
    bool open();
    QList<QStringList> CSVRead();
    void close() { if(isOpen()) mFile.close(); }
    bool isOpen() const { return mFile.isOpen(); }

private:
    QFile mFile;
    QChar mSeparator;
    QList<QStringList> mLinesList;
};
#endif // CSVREADER_H
