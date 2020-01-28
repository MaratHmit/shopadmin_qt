#ifndef DIALOGCARDROLE_H
#define DIALOGCARDROLE_H

#include <QDialog>

#include "dialogcard.h"

namespace Ui {
class DialogCardRole;
}

class DialogCardRole : public DialogCard
{
    Q_OBJECT

public:
    explicit DialogCardRole(QWidget *parent = 0);
    ~DialogCardRole();

private:
    Ui::DialogCardRole *ui;

protected:

    virtual void initControls();
    virtual void fillControls();

    bool setData();
    bool save();

};

#endif // DIALOGCARDROLE_H
