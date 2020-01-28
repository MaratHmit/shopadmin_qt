#ifndef DIALOGCARD_H
#define DIALOGCARD_H

#include <QDialog>

#include "dataitem.h"

class DialogCard : public QDialog
{
    Q_OBJECT

public:
    explicit DialogCard(QWidget *parent = 0);
    ~DialogCard();

    void setItem(const DataItem *item = 0);
    const DataItem *getItem() const;

    virtual void setLocalMode();

protected:    
    void initVariables();
    void initSignals();

    virtual void initControls();

    void showEvent(QShowEvent *);

    virtual void fillControls();
    virtual bool setData();
    virtual bool save();

    DataItem *item;
    bool isLocalMode;

    void accept();

};

#endif // DIALOGCARD_H
