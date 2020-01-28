#ifndef DIALOGMIGRATIONIMAGES_H
#define DIALOGMIGRATIONIMAGES_H

#include <QDialog>

#include "dataitem.h"
#include "api.h"
#include "dialogauthcheck.h"

namespace Ui {
class DialogMigrationImages;
}

class DialogMigrationImages : public QDialog
{
    Q_OBJECT

public:
    explicit DialogMigrationImages(QWidget *parent = 0);
    ~DialogMigrationImages();

    const QString &getProjectMigration() const;

private slots:
    void finishedMigration(bool status, QString message);

private:
    Ui::DialogMigrationImages *ui;
    Api *api;

    ListDataItems itemsTrusted;
    QString projectMigration;

    void accept();

    bool auth();

    void execImagesMigration();

    void initControls();
    void initVariables();
    void initSignals();
};

#endif // DIALOGMIGRATIONIMAGES_H
