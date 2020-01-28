#include "dialogcard.h"

DialogCard::DialogCard(QWidget *parent) :
    QDialog(parent)
{
    initVariables();
    initSignals();
}

DialogCard::~DialogCard()
{
    item->deleteLater();
}

void DialogCard::setItem(const DataItem *item)
{
    if (item)
        this->item->copy(item);
    else this->item->clearData();
}

const DataItem *DialogCard::getItem() const
{
    return item;
}

void DialogCard::initVariables()
{
    item = new DataItem;
}

void DialogCard::initSignals()
{

}

void DialogCard::setLocalMode()
{
    isLocalMode = true;
}

void DialogCard::initControls()
{

}

void DialogCard::showEvent(QShowEvent *)
{
    initControls();
    fillControls();
}

void DialogCard::fillControls()
{

}

bool DialogCard::setData()
{
    return false;
}

bool DialogCard::save()
{
    return false;
}

void DialogCard::accept()
{
    if (setData() && save())
        QDialog::accept();
}

