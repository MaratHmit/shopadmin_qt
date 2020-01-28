#include "formsetgroups.h"
#include "ui_formsetgroups.h"
#include "apiadapter.h"

#include <QListWidget>
#include <QDebug>

FormSetGroups::FormSetGroups(
        QWidget *parent, ListContacts *contacts, int typeContacts) :
    QDialog(parent),
    ui(new Ui::FormSetGroups)
{
    ui->setupUi(this);    
    this->contacts = contacts;
    this->typeContacts = typeContacts;
    initListView();
    initSignals();
}

FormSetGroups::~FormSetGroups()
{    
    delete ui;    
}

void FormSetGroups::showEvent(QShowEvent *)
{
    loadGroups();
}

void FormSetGroups::initListView()
{
    groups = new ListDataItems();
    modelGroups = new NameTableModel(this, groups);    
    modelGroups->setCheckedMode(true);
    ui->listView->setModel(modelGroups);
}

void FormSetGroups::initSignals()
{
    connect(ui->pushButtonCancel, &QPushButton::clicked, this,
            &FormSetGroups::reject);
    connect(ui->pushButtonOK, &QPushButton::clicked, this,
            &FormSetGroups::onOK);
}

void FormSetGroups::loadGroups()
{
    modelGroups->beginReset();
    if (ApiAdapter::getApi()->getListContactsGroups(groups)) {
        QVector<QString> *groupsChecked = new QVector<QString>;
        fillGroupsChecked(groupsChecked);
        for (int i = 0; i < groups->size(); i++)
            groups->at(i)->setIsChecked(groupsChecked->contains(groups->at(i)->getId()));
        delete groupsChecked;
    }
    modelGroups->endReset();
}

void FormSetGroups::fillGroupsChecked(QVector<QString> *groupsChecked)
{
    for (int i = 0; i < contacts->size(); i++) {
        for (int j = 0; j < contacts->at(i)->getIdsGroups()->size(); j++)
             if (!groupsChecked->contains(contacts->at(i)->getIdsGroups()->at(j)))
                 groupsChecked->append(contacts->at(i)->getIdsGroups()->at(j));
    }
}

void FormSetGroups::onOK()
{
    DataContact *contact = new DataContact();
    for (int i = 0; i < contacts->size(); i++)
        contact->addId(contacts->at(i)->getId());
    for (int i = 0; i < groups->size(); i++)
        if (groups->at(i)->getIsChecked())
            contact->addIdGroup(groups->at(i)->getId());
    contact->setModified("idsGroups");
    if ((typeContacts && ApiAdapter::getApi()->save(SEConsts::API_COMPANIES, contact)) ||
            ApiAdapter::getApi()->saveContact(contact))
        accept();
}
