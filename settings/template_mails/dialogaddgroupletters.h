#ifndef DIALOGADDGROUPLETTERS_H
#define DIALOGADDGROUPLETTERS_H

#include <QDialog>

#include "datalettergroup.h"

namespace Ui {
class DialogAddGroupLetters;
}

class DialogAddGroupLetters : public QDialog
{
    Q_OBJECT

public:
    explicit DialogAddGroupLetters(QWidget *parent = 0);
    ~DialogAddGroupLetters();

    const DataLetterGroup *group() const;

private:
    Ui::DialogAddGroupLetters *ui;
    DataLetterGroup *mGroup;

    void onOK();
    void onChangeData();
    void initVariables();
    void initSignals();
};

#endif // DIALOGADDGROUPLETTERS_H
