#include "csvreader.h"
#include <QTextStream>

CsvReader::CsvReader(QObject *parent, const QString& fileName):
    QObject(parent), mFile(fileName), mSeparator(';')
{ }

CsvReader::~CsvReader()
{
    close();
}

bool CsvReader::open(){
    if(!mFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;
    else return true;
}

QString trimCSV(QString item){
    if((!item.isEmpty()) && (item[0] == QChar(34)))
        item.remove(0,1);
    if(!item.isEmpty() && !item.isNull() && (item[item.count() - 1] == QChar(34)))
        item.remove(item.count() - 1, 1);
    if(!item.isEmpty())
        item = item.replace("\"\"","\"");
    return item;
}

QList<QStringList> CsvReader::CSVRead()
{
    if (mFile.isOpen()){
        bool quote = false;
        QList<QString> itemList;
        QString item = "";
        QTextStream out(&mFile);
        while (!out.atEnd()) {
            QString line(out.readLine().simplified());
            int count = line.count();
            if (count) {
                for (int i = 0; i < count; i++){
                    if (line[i] == QChar(34))
                        quote = (quote) ? false : true;
                    if ((quote != true)&(line[i] == mSeparator)){
                        itemList.append(trimCSV(item));
                        item = "";
                    } else {
                        item += line[i];
                    }

                    if ((count - 1 == i) & (quote != true)){
                        item = trimCSV(item);
                        if (item != "")
                            itemList.append(item);
                        mLinesList.append(itemList);
                        itemList.clear();
                        item = "";
                    }
                }
            } else {
                if (!mLinesList.size())
                    mLinesList.append(QStringList());
            }
        }
    }
    close();
    return mLinesList;
}
