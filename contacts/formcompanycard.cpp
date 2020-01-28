#include "formcompanycard.h"
#include "ui_formcompanycard.h"

#include "apiadapter.h"
#include "dataorder.h"
#include "formcontacts.h"
#include "formordercard.h"
#include "userpermission.h"

#include <QComboBox>
#include <QDateEdit>

FormCompanyCard::FormCompanyCard(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormCompanyCard)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window);

    initVariables();
    initSignals();
}

FormCompanyCard::~FormCompanyCard()
{
    delete ui;
    delete company;
}

QString FormCompanyCard::idCompany()
{
    return company->getId();
}

void FormCompanyCard::setIdCompany(const QString &idCompany, const bool &isInfoMode)
{
    company->setId(idCompany);
    if (isInfoMode)
        ui->buttonSave->hide();
}

const DataCompany *FormCompanyCard::getCompany() const
{
    return company;
}

void FormCompanyCard::showEvent(QShowEvent *)
{
    fillControls();
    ui->buttonSave->setEnabled(false);
    ui->lineEditCompanyName->setFocus();
    if (!company->getId().isEmpty())
        ui->pushButtonCustomFields->show();
}

void FormCompanyCard::initVariables()
{
    modelOrders = 0;
    modelGroups = 0;
    company = new DataCompany;
    groupsContact = new ListDataItems();
    ui->stackedWidget->setCurrentIndex(0);
    initModelContacts();    
    ui->pageCustomFields->setDataType("company");
    ui->pageCustomFields->setItem(company);
    ui->pushButtonCustomFields->hide();
    if (!UserPermission::isAdmin) {
        ui->lineEditLogin->hide();
        ui->lineEditPassword->hide();
        ui->labelPassword->hide();
        ui->labelLogin->hide();
    }
}

void FormCompanyCard::initModelContacts()
{
    modelContacts = new NameTableModel(this, reinterpret_cast <ListDataItems *> (company->contacts()));
    modelContacts->clearFields();
    modelContacts->appendField(new Field("id", "Ид.", Qt::AlignRight | Qt::AlignVCenter));
    modelContacts->appendField(new Field("display", "Фамилия Имя Отчество"));
    modelContacts->appendField(new Field("email", "E-mail"));
    modelContacts->appendField(new Field("phone", "Телефоны"));
    ui->treeViewContacts->setModel(modelContacts);
}

void FormCompanyCard::uncheckedButtons()
{
    for (int i = 0; i < ui->frame->children().size(); i++) {
        QPushButton *button = qobject_cast<QPushButton *> (ui->frame->children().at(i));
        if (button)
            button->setChecked(false);
    }
}

void FormCompanyCard::showOrders()
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
    request.setFilter("[idCompany] = '" + company->getId() + "'");

    if (ApiAdapter::getApi()->getListOrders(request, orders))
        ui->treeViewOrders->setModel(modelOrders);
    for (int i = 0; i < modelOrders->columnCount() - 1; ++i)
        ui->treeViewOrders->resizeColumnToContents(i);

    connect(ui->treeViewOrders, SIGNAL(doubleClicked(QModelIndex)), SLOT(showOrderCard()));
}

void FormCompanyCard::showGroups()
{
    if (modelGroups)
        return;

    modelGroups = new NameTableModel(this, groupsContact);
    ui->listViewGroups->setModel(modelGroups);
    modelGroups->beginReset();
    ApiAdapter::getApi()->getListContactsGroups(groupsContact);
    modelGroups->setCheckedMode(true);
    for (int i = 0; i < company->getIdsGroups()->size(); ++i) {
        for (int j = 0; j < groupsContact->size(); ++j)
            if (groupsContact->at(j)->getId() == company->getIdsGroups()->at(i))
                groupsContact->at(j)->setIsChecked(true);
    }
    modelGroups->endReset();
    connect(modelGroups, SIGNAL(checked(QString,bool)), SLOT(onChanged()));
}

bool FormCompanyCard::saveCompany()
{
    if (ui->lineEditCompanyName->text().trimmed().isEmpty()) {
        ui->pushButtonMainInfo->click();
        QMessageBox::warning(this, tr("Сохранение компании"),
                             tr("Не заполнено обязателное поле: <Название компании>!"));
        ui->lineEditCompanyName->setFocus();
        return false;
    }

    bool isNew = company->getId().isEmpty();
    getParams();
    if (!company->isModified())
        return true;

    bool result;
    result = ApiAdapter::getApi()->save(SEConsts::API_COMPANIES, company);
    if (result) {
        if (isNew)
            company->setRegDate(QDate::currentDate());
        emit saved(company, isNew);
        return true;
    }

    return false;
}

void FormCompanyCard::getParams()
{
    company->setName(ui->lineEditCompanyName->text().trimmed());
    company->setPhone(ui->lineEditPhone->text().trimmed());
    company->setEmail(ui->lineEditEmail->text().trimmed());
    company->setAddress(ui->lineEditAddress->text().trimmed());
    company->setLogin(ui->lineEditLogin->text());
    company->setProperty("inn", ui->lineEditINN->text().trimmed());
    company->setProperty("kpp", ui->lineEditKPP->text().trimmed());
    company->setProperty("note", ui->textEditNote->document()->toPlainText());    

    if (ui->lineEditPassword->text() != SEConsts::ECHO_PASSWORDTEXT &&
            !ui->lineEditPassword->text().trimmed().isEmpty())
        company->setPassword(ui->lineEditPassword->text().trimmed());

    if (modelGroups) {
        company->clearGroups();
        for (int i = 0; i < modelGroups->getItems()->size(); i++) {
              if (modelGroups->getItems()->at(i)->getIsChecked())
                   company->addIdGroup(modelGroups->getItems()->at(i)->getId());
          }
    }
}

void FormCompanyCard::fillControls()
{
    if (company->getId().isEmpty())
        return;

    if (!ApiAdapter::getApi()->getInfoData(SEConsts::API_COMPANIES, company))
        return;

    ui->lineEditCompanyName->setText(company->getName());
    ui->lineEditINN->setText(company->property("inn").toString());
    ui->lineEditKPP->setText(company->property("kpp").toString());
    ui->lineEditPhone->setText(company->phone());
    ui->lineEditEmail->setText(company->email());
    ui->textEditNote->setText(company->property("note").toString());
    ui->lineEditAddress->setText(company->address());
    ui->lineEditLogin->setText(company->getLogin());
    ui->lineEditPassword->setText(SEConsts::ECHO_PASSWORDTEXT);
}

void FormCompanyCard::initSignals()
{
    initSignalsChanged();
    connect(ui->toolButtonAddContact, SIGNAL(clicked(bool)), SLOT(addContact()));
    connect(ui->toolButtonDeleteContact, SIGNAL(clicked(bool)), SLOT(deleteContact()));
    connect(ui->treeViewContacts->selectionModel(),
            SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), SLOT(onChangeContact()));
    connect(ui->pageCustomFields, SIGNAL(modified()), SLOT(onChanged()));
}

void FormCompanyCard::initSignalsChanged(const QWidget *widget)
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

void FormCompanyCard::on_buttonCancel_clicked()
{
    reject();
}

void FormCompanyCard::on_buttonSave_clicked()
{
    setCursor(QCursor(Qt::WaitCursor));
    ui->buttonCancel->setEnabled(false);
    ui->buttonSave->setEnabled(false);
    ui->buttonOK->setEnabled(false);
    if (saveCompany())
       ui->buttonSave->setEnabled(false);
    else ui->buttonSave->setEnabled(true);
    ui->buttonCancel->setEnabled(true);
    ui->buttonOK->setEnabled(true);
    setCursor(QCursor(Qt::ArrowCursor));
}

void FormCompanyCard::on_buttonOK_clicked()
{
    setCursor(QCursor(Qt::WaitCursor));
    ui->buttonCancel->setEnabled(false);
    ui->buttonSave->setEnabled(false);
    ui->buttonOK->setEnabled(false);
    if (saveCompany())
        accept();
    ui->buttonCancel->setEnabled(true);
    ui->buttonOK->setEnabled(true);
    setCursor(QCursor(Qt::ArrowCursor));
}

void FormCompanyCard::onChanged()
{
    ui->buttonSave->setEnabled(true);
    if (sender() == modelGroups)
        company->setModified("idsGroups");
}

void FormCompanyCard::on_pushButtonMainInfo_clicked()
{
    uncheckedButtons();
    ui->pushButtonMainInfo->setChecked(true);
    ui->stackedWidget->setCurrentIndex(0);
    ui->lineEditCompanyName->setFocus();
}

void FormCompanyCard::on_pushButtonContacts_clicked()
{
    uncheckedButtons();
    ui->pushButtonContacts->setChecked(true);
    ui->stackedWidget->setCurrentIndex(1);
    for (int i = 0; i < modelContacts->columnCount() - 1; ++i)
        ui->treeViewContacts->resizeColumnToContents(i);
}

void FormCompanyCard::on_pushButtonGroups_clicked()
{
    uncheckedButtons();
    ui->pushButtonGroups->setChecked(true);
    ui->stackedWidget->setCurrentIndex(2);
    showGroups();
}

void FormCompanyCard::on_pushButtonOrders_clicked()
{
    uncheckedButtons();
    ui->pushButtonOrders->setChecked(true);
    ui->stackedWidget->setCurrentIndex(3);
    if (modelOrders)
        return;

    showOrders();
}

void FormCompanyCard::addContact()
{
    ui->toolButtonAddContact->setEnabled(false);
    setCursor(Qt::WaitCursor);

    FormContacts form(this, true, FormContacts::Contacts);

    if (form.exec() == QDialog::Accepted) {
        const ListContacts *contacts = form.getSelectedContacts();
        for (int i = 0; i < contacts->size(); ++i) {
            modelContacts->addItem(new DataContact(contacts->at(i)));
            if (!i) {
                DataContact contact;
                contact.setId(contacts->at(i)->getId());
                if (ApiAdapter::getApi()->getInfoContact(&contact)) {
                    if (!contact.companyName().isEmpty()) {
                        if (ui->lineEditCompanyName->text().trimmed().isEmpty())
                            ui->lineEditCompanyName->setText(contact.companyName());
                        if (ui->lineEditEmail->text().trimmed().isEmpty() && !contact.email().isEmpty())
                            ui->lineEditEmail->setText(contact.email());
                        if (ui->lineEditPhone->text().trimmed().isEmpty() && !contact.phone().isEmpty())
                            ui->lineEditPhone->setText(contact.phone());
                        if (ui->lineEditAddress->text().trimmed().isEmpty() && !contact.address().isEmpty())
                            ui->lineEditAddress->setText(contact.address());
                        for (int j = 0; j < contact.companyRequisites()->size(); ++j) {
                            if ((contact.companyRequisites()->at(j)->getCode() == "inn") &&
                                    (!contact.companyRequisites()->at(j)->getValue().isEmpty()))
                                        ui->lineEditINN->setText(contact.companyRequisites()->at(j)->getValue());
                            if ((contact.companyRequisites()->at(j)->getCode() == "kpp") &&
                                    (!contact.companyRequisites()->at(j)->getValue().isEmpty()))
                                        ui->lineEditKPP->setText(contact.companyRequisites()->at(j)->getValue());
                        }
                    }
                }
            }
        }
        for (int i = 0; i < modelContacts->columnCount() - 1; ++i)
            ui->treeViewContacts->resizeColumnToContents(i);
        onChanged();
    }

    ui->toolButtonAddContact->setEnabled(true);
    setCursor(Qt::ArrowCursor);
}

void FormCompanyCard::deleteContact()
{
    if (!ui->treeViewContacts->selectionModel()->selectedRows().size())
        return;

    QModelIndexList list =
            ui->treeViewContacts->selectionModel()->selectedRows();
    modelContacts->deleteItems(list);
    company->contacts()->setModified();
    onChanged();
}

void FormCompanyCard::onChangeContact()
{
    ui->toolButtonDeleteContact->setEnabled(ui->treeViewContacts->currentIndex().isValid());
}

void FormCompanyCard::showOrderCard()
{
    if (!ui->treeViewOrders->currentIndex().isValid())
        return;

    setCursor(Qt::WaitCursor);
    FormOrderCard card(this, dynamic_cast <DataOrder *> (modelOrders->dataItem(ui->treeViewOrders->currentIndex())));
    card.setInfoMode();
    card.exec();
    setCursor(Qt::ArrowCursor);
}

void FormCompanyCard::on_pushButtonCustomFields_clicked()
{
    uncheckedButtons();
    ui->pushButtonCustomFields->setChecked(true);
    ui->stackedWidget->setCurrentIndex(4);
}
