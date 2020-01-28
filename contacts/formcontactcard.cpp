#include "formcontactcard.h"
#include "ui_formcontactcard.h"
#include "apiadapter.h"
#include "dataitem.h"
#include "seconfig.h"
#include "field.h"
#include "request.h"
#include "formordercard.h"
#include "dialogaddeditaccount.h"
#include "userpermission.h"
#include "formcontacts.h"

FormContactCard::FormContactCard(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormContactCard)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window);
    isShownGroups = false;
    isShownAccounts = false;    

    QRegExp regExpr("[\\w-_\\s]*");
    QRegExpValidator *regSimpleText = new QRegExpValidator(regExpr, this);

    contact = new DataContact;    
    groupsContact = new ListDataItems();
    orders = 0;
    modelOrders = 0;
    modelRequisites = 0;

    modelAccounts = new NameTableModel(this, reinterpret_cast <ListDataItems *> (contact->personalAccount()));
    initFieldsAccountModel();
    ui->treeViewPersonaAccount->setModel(modelAccounts);

    ui->pageCustomFields->setDataType("contact");
    ui->pageCustomFields->setItem(contact);
    ui->pushButtonCustomFields->hide();
    connect(ui->pageCustomFields, SIGNAL(modified()), SLOT(onChanged()));

    ui->lineEditLastName->setValidator(regSimpleText);
    ui->lineEditFirstName->setValidator(regSimpleText);
    ui->lineEditSecondName->setValidator(regSimpleText);

    ui->dateEditRegDate->setDate(QDate::currentDate());
    ui->stackedWidget->setCurrentIndex(0);
    ui->lineEditLastName->setFocus();

    ui->pageImage->hideImageAlt();
    ui->pageImage->setSection(SEConsts::Section_Contacts);

    if (!UserPermission::isAdmin) {
        ui->lineEditLogin->hide();
        ui->lineEditPassword->hide();
        ui->labelPassword->hide();
        ui->labelLogin->hide();
        ui->checkBoxIsAdmin->hide();
    }

    if (!SEConfig::isUserAdmin) {
        ui->labelManager->hide();
        ui->comboBoxManagers->hide();
    } else {
        ListContacts listManagers;
        if (ApiAdapter::getApi()->getListUsers(Request(), &listManagers)) {
            for (int i = 0; i < listManagers.size(); ++i) {
                ui->comboBoxManagers->addItem(listManagers.at(i)->displayName(),
                                             listManagers.at(i)->getId());
            }
        }
        ui->comboBoxManagers->setCurrentIndex(-1);
    }

    initSignals();
    initPermission();
}

FormContactCard::~FormContactCard()
{
    delete ui;
    delete contact;
    delete groupsContact;
    if (modelOrders)
        delete modelOrders;
    if (orders)
        delete orders;
}

QString FormContactCard::idContact()
{
    return contact->getId();
}

void FormContactCard::setIdContact(const QString &idContact, const bool &isInfoMode)
{
    contact->setId(idContact);
    if (isInfoMode)
        ui->buttonSave->hide();
}

const DataContact *FormContactCard::getContact() const
{
    return contact;
}

void FormContactCard::on_buttonSave_clicked()
{
    setCursor(QCursor(Qt::WaitCursor));
    ui->buttonCancel->setEnabled(false);
    ui->buttonSave->setEnabled(false);
    ui->buttonOK->setEnabled(false);
    if (saveContact())
       ui->buttonSave->setEnabled(false);
    else ui->buttonSave->setEnabled(true);
    ui->buttonCancel->setEnabled(true);
    ui->buttonOK->setEnabled(true);
    setCursor(QCursor(Qt::ArrowCursor));
}

bool FormContactCard::saveContact()
{
    if (ui->lineEditFirstName->text().trimmed().isEmpty()) {
        ui->pushButtonMainInfo->click();
        QMessageBox::warning(this, tr("Сохранение контакта"),
                             tr("Не заполнено обязателное поле: <Имя>!"));
        ui->lineEditFirstName->setFocus();
        return false;
    }

    bool isNew = contact->getId().isEmpty();
    getParams();        
    if (!contact->isModified())
        return true;

    bool result;
    result = ApiAdapter::getApi()->saveContact(contact);
    if (result) {
        ui->pageImage->saveImage();
        DataContact infoContact(contact);
        if (ApiAdapter::getApi()->getInfoContact(&infoContact)) {
            contact->setLogin(infoContact.getLogin());
            ui->lineEditLogin->setText(contact->getLogin());
        }        
        emit saved(contact, isNew);
        return true;
    }

    return false;
}

void FormContactCard::getParams()
{
    contact->setProperty("firstName", ui->lineEditFirstName->text());
    contact->setProperty("lastName", ui->lineEditLastName->text());
    contact->setProperty("secondName", ui->lineEditSecondName->text());
    contact->setEmail(ui->lineEditEmail->text());
    contact->setPhone(ui->lineEditPhone->text());
    contact->setPostIndex(ui->lineEditPostIndex->text());
    contact->setAddress(ui->lineEditAddress->text());
    contact->setRegDate(ui->dateEditRegDate->date());
    contact->setBirthDate(ui->dateEditBirthDate->date());    
    contact->setDocSer(ui->editDocSer->text());
    contact->setDocNum(ui->editDocNum->text());
    contact->setDocRegistr(ui->textEditDocReg->document()->toPlainText());    
    contact->setProperty("note", ui->textEditNote->document()->toPlainText());
    contact->setLogin(ui->lineEditLogin->text());
    contact->setSkype(ui->lineEditSkype->text().trimmed());
    contact->setLoyalty(ui->horizontalSliderLoyalty->value());
    contact->setImageFile(ui->pageImage->fileImageName());
    contact->setPriceType(ui->comboBoxPriceType->currentIndex());

    contact->setCompanyName(ui->lineEditCompanyName->text().trimmed());
    contact->setCompanyDirector(ui->lineEditCompanyDirector->text().trimmed());
    contact->setCompanyPhone(ui->lineEditCompanyPhone->text().trimmed());
    contact->setCompanyFax(ui->lineEditCompanyFax->text().trimmed());
    contact->setCompanyMailAddress(ui->lineEditCompanyMailAddress->text().trimmed());
    contact->setCompanyOfficialAddress(ui->lineEditCompanyOfficialAddress->text().trimmed());
    contact->setIsAdmin(ui->checkBoxIsAdmin->isChecked());
    contact->setIsActive(ui->checkBoxIsActive->isChecked());

    if (ui->comboBoxManagers->currentIndex() >= 0)
        contact->setIdManager(ui->comboBoxManagers->currentData().toString());

    if (ui->lineEditPassword->text() != SEConsts::ECHO_PASSWORDTEXT &&
            !ui->lineEditPassword->text().trimmed().isEmpty())
        contact->setPassword(ui->lineEditPassword->text().trimmed());

    if (ui->comboBox->currentIndex() == -1)
        contact->setGender(DataContact::Unknown);
    if (ui->comboBox->currentIndex() == 0)
        contact->setGender(DataContact::Men);
    if (ui->comboBox->currentIndex() == 1)
        contact->setGender(DataContact::Female);    

    if (isShownGroups) {
        contact->clearGroups();
        for (int i = 0; i < modelGroupsContact->getItems()->size(); i++) {
              if (modelGroupsContact->getItems()->at(i)->getIsChecked())
                   contact->addIdGroup(modelGroupsContact->getItems()->at(i)->getId());
          }
    }
}

void FormContactCard::fillControls()
{
    if (idContact() == 0) {
        if (!SEConfig::idCurrentUser.isEmpty()) {
            for (int i = 0; i < ui->comboBoxManagers->count(); ++i) {
                if (ui->comboBoxManagers->itemData(i).toString() == SEConfig::idCurrentUser) {
                    ui->comboBoxManagers->setCurrentIndex(i);
                    break;
                }
            }
        }
        return;
    }


    if (!ApiAdapter::getApi()->getInfoContact(contact))
        return;

    ui->lineEditFirstName->setText(contact->property("firstName").toString());
    ui->lineEditLastName->setText(contact->property("lastName").toString());
    ui->lineEditSecondName->setText(contact->property("secondName").toString());
    ui->lineEditEmail->setText(contact->email());
    ui->lineEditPhone->setText(contact->phone());
    ui->lineEditSkype->setText(contact->skype());
    ui->lineEditPostIndex->setText(contact->postIndex());
    ui->lineEditAddress->setText(contact->address());
    ui->dateEditRegDate->setDate(contact->regDate());
    ui->dateEditBirthDate->setDate(contact->birthDate());    
    ui->editDocSer->setText(contact->docSer());
    ui->editDocNum->setText(contact->docNum());
    ui->textEditDocReg->setText(contact->docRegistr());
    ui->textEditNote->setText(contact->property("note").toString());
    ui->lineEditLogin->setText(contact->getLogin());
    ui->checkBoxIsActive->setChecked(contact->getIsActive());
    ui->lineEditPassword->setText(SEConsts::ECHO_PASSWORDTEXT);
    ui->horizontalSliderLoyalty->setValue(contact->loyalty());
    ui->pageImage->setImage(contact->getImageFile());
    ui->lineEditCompanyName->setText(contact->companyName());
    ui->lineEditCompanyDirector->setText(contact->companyDirector());
    ui->lineEditCompanyPhone->setText(contact->companyPhone());
    ui->lineEditCompanyFax->setText(contact->companyFax());
    ui->lineEditCompanyMailAddress->setText(contact->companyMailAddress());
    ui->lineEditCompanyOfficialAddress->setText(contact->companyOfficialAddress());
    ui->checkBoxIsAdmin->setChecked(contact->isAdmin());
    ui->lineEditRefer->setText(contact->property("referContact").toString());
    ui->comboBoxPriceType->setCurrentIndex(contact->priceType());
    if (!contact->idManager().isEmpty()) {
        for (int i = 0; i < ui->comboBoxManagers->count(); ++i) {
            if (ui->comboBoxManagers->itemData(i).toString() == contact->idManager()) {
                ui->comboBoxManagers->setCurrentIndex(i);
                break;
            }
        }
    }

    if (contact->gender() == DataContact::Men)
        ui->comboBox->setCurrentIndex(0);
    if (contact->gender() == DataContact::Female)
        ui->comboBox->setCurrentIndex(1);
}

void FormContactCard::showGroups()
{
    if (isShownGroups)
        return;
    isShownGroups = true;    

    modelGroupsContact = new NameTableModel(this, groupsContact);
    ui->listViewGroups->setModel(modelGroupsContact);
    modelGroupsContact->beginReset();
    ApiAdapter::getApi()->getListContactsGroups(groupsContact);
    modelGroupsContact->setCheckedMode(true);
    for (int i = 0; i < contact->getIdsGroups()->size(); ++i) {
        for (int j = 0; j < groupsContact->size(); ++j)
            if (groupsContact->at(j)->getId() == contact->getIdsGroups()->at(i))
                groupsContact->at(j)->setIsChecked(true);
    }
    modelGroupsContact->endReset();
    connect(modelGroupsContact, SIGNAL(checked(QString,bool)), SLOT(onChanged()));
}

void FormContactCard::showPersonalAccount()
{
    if (isShownAccounts)
        return;

    isShownAccounts = true;
    contact->recalcAccountsBalance();
    modelAccounts->resetModel();
    for (int i = 0; i < modelAccounts->columnCount(); ++i)
        ui->treeViewPersonaAccount->resizeColumnToContents(i);
    if (contact->personalAccount()->size())
        ui->treeViewPersonaAccount->selectionModel()->setCurrentIndex(modelAccounts->index(0, 0),
                                                                      QItemSelectionModel::SelectCurrent |
                                                                      QItemSelectionModel::Rows);
    connect(ui->treeViewPersonaAccount, SIGNAL(doubleClicked(QModelIndex)), SLOT(addEditAccount()));
}

void FormContactCard::initSignals()
{
    initSignalsChanged();
    connect(ui->checkBoxIsAdmin, &QCheckBox::clicked, this, &FormContactCard::onChanged);
    connect(ui->checkBoxIsActive, &QCheckBox::clicked, this, &FormContactCard::onChanged);
    connect(ui->pageImage, SIGNAL(modified()), SLOT(onChanged()));
    connect(ui->buttonOK, SIGNAL(clicked()), SLOT(onOK()));
    connect(ui->toolButtonAddAccount, SIGNAL(clicked()), SLOT(addEditAccount()));
    connect(ui->toolButtonEditAccount, SIGNAL(clicked()), SLOT(addEditAccount()));
    connect(ui->toolButtonSelectRefer, SIGNAL(clicked()), SLOT(selectReferContact()));
    connect(ui->toolButtonDeleteRefer, SIGNAL(clicked()), SLOT(deleteReferContact()));
}

void FormContactCard::initSignalsChanged(const QWidget *widget)
{
    if (!widget)
        widget = this;
    for (int i = 0; i < widget->children().size(); i++) {
        QLineEdit *edit = qobject_cast<QLineEdit *> (widget->children().at(i));
        if (edit)
            connect(edit, SIGNAL(textEdited(QString)),
                    SLOT(onChanged()));
        QTextEdit *textEdit = qobject_cast<QTextEdit *> (widget->children().at(i));        
        if (textEdit)
            connect(textEdit, SIGNAL(textChanged()), SLOT(onChanged()));
        QComboBox *box = qobject_cast<QComboBox *> (widget->children().at(i));
        if (box)
            connect(box, SIGNAL(currentIndexChanged(int)), SLOT(onChanged()));
        QDateEdit *dateEdit = qobject_cast <QDateEdit *> (widget->children().at(i));
        if (dateEdit)
            connect(dateEdit, SIGNAL(dateChanged(QDate)), SLOT(onChanged()));                
        if (QWidget *widgetC = qobject_cast<QWidget *> (widget->children().at(i)))
            initSignalsChanged(widgetC);
        QSlider *slider = qobject_cast <QSlider *> (widget->children().at(i));
        if (slider)
            connect(slider, SIGNAL(valueChanged(int)), SLOT(onChanged()));
    }
}

void FormContactCard::initFieldsAccountModel()
{
    modelAccounts->clearFields();
    QVector<Field *>  *fields = new QVector<Field *>();
    fields->append(new Field("datePayee", "Дата"));
    fields->append(new Field("inPayeeTitle", "Приход", Qt::AlignRight | Qt::AlignVCenter));
    fields->append(new Field("outPayeeTitle", "Расход", Qt::AlignRight | Qt::AlignVCenter));
    fields->append(new Field("balanceTitle", "Баланс", Qt::AlignRight | Qt::AlignVCenter));
    fields->append(new Field("note", "Примечание"));
    modelAccounts->appendFields(fields);
}

void FormContactCard::initPermission()
{
    ui->pushButtonOrders->setVisible(UserPermission::isRead(UserPermission::ORDERS));
}

void FormContactCard::showImage()
{

}

void FormContactCard::showOrders()
{
    orders = new ListOrders();
    modelOrders = new NameTableModel(this, reinterpret_cast <ListDataItems *> (orders));
    modelOrders->clearFields();
    QVector<Field *>  *fields = new QVector<Field *>();
    fields->append(new Field("id", "Ид.", Qt::AlignRight | Qt::AlignVCenter));
    fields->append(new Field("dateOrder", "Дата заказа"));
    fields->append(new Field("sum", "Сумма", Qt::AlignRight | Qt::AlignVCenter));
    fields->append(new Field("dateDelivery", "Дата доставки"));
    fields->append(new Field("statusOrder", "Статус заказы"));
    fields->append(new Field("statusDelivery", "Статус доставки"));
    fields->append(new Field("note", "Примечание"));
    modelOrders->appendFields(fields);

    Request request;
    request.setFilter("[idCustomer] = '" + contact->getId() + "'");

    if (ApiAdapter::getApi()->getListOrders(request, orders))
        ui->treeViewOrders->setModel(modelOrders);
    for (int i = 0; i < modelOrders->columnCount() - 1; ++i)
        ui->treeViewOrders->resizeColumnToContents(i);

    connect(ui->treeViewOrders, SIGNAL(doubleClicked(QModelIndex)), SLOT(showOrderCard()));
}

void FormContactCard::on_pushButtonMainInfo_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->lineEditLastName->setFocus();
}

void FormContactCard::on_pushButtonNote_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
    ui->textEditNote->setFocus();;
}

void FormContactCard::on_pushButtonContactInfo_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
    ui->lineEditPhone->setFocus();
}

void FormContactCard::on_pushButtonDocInfo_clicked()
{
    ui->stackedWidget->setCurrentIndex(5);
}

void FormContactCard::on_pushButtonGroups_clicked()
{
    ui->stackedWidget->setCurrentIndex(7);
    showGroups();
}

void FormContactCard::on_buttonCancel_clicked()
{
    reject();
}

void FormContactCard::onChanged()
{    
    ui->buttonSave->setEnabled(true);
    if (sender() == modelGroupsContact)
        contact->setModified("idsGroups");
}

void FormContactCard::onOK()
{
    setCursor(QCursor(Qt::WaitCursor));
    ui->buttonCancel->setEnabled(false);
    ui->buttonSave->setEnabled(false);
    ui->buttonOK->setEnabled(false);
    if (saveContact())
        accept();
    ui->buttonCancel->setEnabled(true);
    ui->buttonOK->setEnabled(true);
    setCursor(QCursor(Qt::ArrowCursor));
}

void FormContactCard::selectReferContact()
{
    ui->toolButtonSelectRefer->setEnabled(false);
    setCursor(Qt::WaitCursor);

    FormContacts form(this, false, FormContacts::Contacts);

    if (form.exec() == QDialog::Accepted) {
        const ListContacts *contacts = form.getSelectedContacts();
        ui->lineEditRefer->setText(contacts->at(0)->displayName());
        contact->setProperty("idReferContact", contacts->at(0)->getId());
        onChanged();
    }

    ui->toolButtonSelectRefer->setEnabled(true);
    setCursor(Qt::ArrowCursor);
}

void FormContactCard::deleteReferContact()
{
    ui->lineEditRefer->clear();
    contact->setProperty("idReferContact", 0);
    onChanged();
}

void FormContactCard::showEvent(QShowEvent *){

    fillControls();
    ui->buttonSave->setEnabled(false);
    if (!contact->getId().isEmpty())
        ui->pushButtonCustomFields->show();
}

void FormContactCard::on_pushButtonPersonalAccount_clicked()
{
     ui->stackedWidget->setCurrentIndex(6);
     showPersonalAccount();
}

void FormContactCard::on_pushButtonOrders_clicked()
{
    ui->stackedWidget->setCurrentIndex(8);
    if (modelOrders)
        return;

    showOrders();
}

void FormContactCard::showOrderCard()
{
    if (!ui->treeViewOrders->currentIndex().isValid())
        return;

    setCursor(Qt::WaitCursor);
    FormOrderCard
            card(this, dynamic_cast <DataOrder *> (modelOrders->dataItem(ui->treeViewOrders->currentIndex())));
    card.setInfoMode();
    card.exec();
    setCursor(Qt::ArrowCursor);
}

void FormContactCard::on_pushButtonImage_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    showImage();
}

void FormContactCard::on_pushButtonRequisites_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
    if (ui->lineEditCompanyName->text().isEmpty())
        ui->lineEditCompanyName->setFocus();

    if (modelRequisites)
        return;

    if (!contact->companyRequisites()->size())
        ApiAdapter::getApi()->getListCompanyRequisites(contact->companyRequisites());

    modelRequisites = new NameTableModel(this, reinterpret_cast <ListDataItems *> (contact->companyRequisites()));
    modelRequisites->setEditableMode(true);
    ui->treeViewRequisites->setModel(modelRequisites);
    modelRequisites->appendField(new Field("value", "Значение"));
    contact->companyRequisites()->setUnModified();
    connect(modelRequisites, &QAbstractItemModel::dataChanged, this, &FormContactCard::onChanged);
}

void FormContactCard::addEditAccount()
{
    bool isNew = true;
    DataPersonalAccount *account = 0;
    if (sender() != ui->toolButtonAddAccount) {
        if (!ui->treeViewPersonaAccount->currentIndex().isValid())
            return;
        account = dynamic_cast <DataPersonalAccount *>
                (modelAccounts->dataItem(ui->treeViewPersonaAccount->currentIndex()));
        isNew = false;
    }
    DialogAddEditAccount dialogAccount(this, account);

    if (dialogAccount.exec() == QDialog::Accepted) {
        if (isNew)
            contact->personalAccount()->append(new DataPersonalAccount(dialogAccount.getAccount()));
        else account->copy(dialogAccount.getAccount());
        contact->recalcAccountsBalance();
        modelAccounts->resetModel();
        onChanged();
    }
}

void FormContactCard::on_pushButtonCustomFields_clicked()
{
     ui->stackedWidget->setCurrentIndex(9);
}
