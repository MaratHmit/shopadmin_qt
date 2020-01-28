#ifndef UPDATER_H
#define UPDATER_H

#include <QString>

class Updater
{
public:
    Updater();  

    bool isHaveUpdate();

private:
  QString mNewVersion;

};

#endif // UPDATER_H
