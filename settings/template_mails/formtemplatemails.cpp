#include "formtemplatemails.h"
#include "ui_formtemplatemails.h"
#include "apiadapter.h"
#include "dialogaddgroupletters.h"
#include "userpermission.h"

FormTemplateMails::FormTemplateMails(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormTemplateMails)
{
    ui->setupUi(this);
    initVariables();
    initSignals();
    initPermissions();
}

FormTemplateMails::~FormTemplateMails()
{    
    delete ui;    
    mGroups->deleteItems();
    delete mGroups;
}

void FormTemplateMails::onAddEditItem()
{
    bool isNew = sender() == ui->toolButtonAdd;
    if ((isNew && !isAddPermission) || (!isNew && !isEditPermission))
        return;

    DialogLetterCard *card = new DialogLetterCard(this);
    if (isNew)
        card->setWindowTitle(tr("Новый шаблон письма"));
    else card->setWindowTitle(tr("Редактирование шаблона письма"));
    const QTreeView *tree = currentTree();
    card->setGroup(tree->objectName(), ui->tabWidget->tabText(ui->tabWidget->currentIndex()));
    if (!isNew) {
        const DataItem *letter = currentLetter();
        if (letter)
            card->setLetter(letter);
    }
    connect(card, SIGNAL(save(bool,DataItem*)), SLOT(onSaveItem(bool,DataItem*)));
    card->show();
}

void FormTemplateMails::onDeleteItem()
{
    const QTreeView *tree = currentTree();
    NameTableModel *model = currentModel();

    if (!tree->currentIndex().isValid())
        return;

    QString msg;
    if (tree->selectionModel()->selectedRows().count() > 1)
        msg = tr("Удалить выбранные шаблоны писем?");
    else msg = tr("Удалить шаблон письма") + ": " +
            model->nameData(tree->currentIndex()) + "?";

    QMessageBox::StandardButton mesRepl;

    mesRepl = QMessageBox::question(this, tr("Подтверждение удаления"),
                                    msg, QMessageBox::Yes | QMessageBox::No);
    if(mesRepl == QMessageBox::Yes)    {
        Request request;
        QModelIndexList list = tree->selectionModel()->selectedRows();

        for (int i = 0; i < list.count(); i++) {
            request.addId(model->getItems()->at(list.at(i).row())->getId());
            model->getItems()->at(list.at(i).row())->setIsSelected(true);
        }
        ApiAdapter::getApi()->deleteTemplateLetter(request);
        for (int i = model->getItems()->size() - 1; i >= 0; i--)
            if (model->getItems()->at(i)->getIsSelected())
                delete model->getItems()->takeAt(i);
        model->resetModel();
    }
}

void FormTemplateMails::onAddGroup()
{
    if (!isAddPermission)
        return;

    DialogAddGroupLetters form(this);

    if (form.exec() == QDialog::Accepted)
        addGroup(new DataLetterGroup(form.group()));
}

void FormTemplateMails::onDeleteGroup(int index)
{
    if (!isDeletePermission)
        return;

    QTreeView *tree = qobject_cast <QTreeView *> (ui->tabWidget->widget(index));
    DataLetterGroup *group = 0;
    for (int i = 0; i < mGroups->size(); i++)
        if (mGroups->at(i)->getId() == tree->objectName()) {
            group = mGroups->at(i);
            break;
        }
    if (!group)
        return;

    QString msg = tr("Удалить группу для шаблонов писем: ") + group->getName() + "?";
    QMessageBox::StandardButton mesRepl;
    mesRepl = QMessageBox::question(this, tr("Подтверждение удаления"),
                                    msg, QMessageBox::Yes | QMessageBox::No);
   if(mesRepl == QMessageBox::No)
       return;

    if (group->templates()->size()) {
        msg = tr("Группа содержит записи - удаление невозможно!");
        QMessageBox::information(this, tr("Информация"), msg, QMessageBox::Ok);
        return;
    }

    Request request;
    request.addId(group->getId());
    if (ApiAdapter::getApi()->deleteTemplateLetterGroup(request)) {
        int i = mGroups->indexOf(group);
        if (i >= 0 && i < mGroups->size())
            delete mGroups->takeAt(i);
        ui->tabWidget->removeTab(index);
        delete tree;
    }
}

void FormTemplateMails::onChangeItem()
{
    const QTreeView *tree = currentTree();
    if (!tree)
        return;

    ui->toolButtonEdit->setEnabled(tree->currentIndex().isValid() && isEditPermission);
    ui->toolButtonDel->setEnabled(tree->currentIndex().isValid() && isDeletePermission);
}

void FormTemplateMails::onSaveItem(const bool isNew, DataItem *letter)
{
    NameTableModel *model = currentModel();
    const DataLetterGroup *group = currentGroup();
    const QTreeView *tree = currentTree();

    model->beginReset();
    if (isNew)
        group->templates()->append(new DataItem(letter));            
    else {        
        if (tree->currentIndex().isValid())
            group->templates()->at(tree->currentIndex().row())->copy(letter);
    }
    model->endReset();
    goToRowById(letter->getId());
}

void FormTemplateMails::showEvent(QShowEvent *)
{
    startTimer(1);
}

void FormTemplateMails::timerEvent(QTimerEvent *e)
{
    killTimer(e->timerId());
    if (!mIsShowed) {
        refreshData();
        if (ui->tabWidget->count())
            ui->tabWidget->setCurrentIndex(0);
    }
}

void FormTemplateMails::addGroup(const DataLetterGroup *group)
{
    QTreeView *tree = new QTreeView(this);
    NameTableModel *model = new NameTableModel(this, reinterpret_cast <ListDataItems *> (group->templates()));
    model->prependField(new Field("code", "Код"));
    tree->setModel(model);
    tree->setObjectName(group->getId());
    ui->tabWidget->addTab(tree, group->getName());
    ui->tabWidget->tabBar()->setTabsClosable(true);
    tree->resizeColumnToContents(0);
    tree->setSelectionMode(QAbstractItemView::ExtendedSelection);
    tree->setSelectionBehavior(QAbstractItemView::SelectRows);
    connect(tree->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            SLOT(onChangeItem()));
    connect(tree, SIGNAL(doubleClicked(QModelIndex)), ui->toolButtonEdit, SLOT(click()));
}

void FormTemplateMails::goToRowById(const QString &id)
{
    const NameTableModel *model = currentModel();
    const QTreeView *tree = currentTree();
    for (int i = 0; i < model->rowCount(); i ++)
        if (model->getItems()->at(i)->getId() == id) {
            tree->selectionModel()->setCurrentIndex(
                        model->index(i, 0),
                        QItemSelectionModel::SelectCurrent |
                        QItemSelectionModel::Rows);
            return;
        }
    if (model->rowCount() > 0)
        tree->selectionModel()->
                setCurrentIndex(model->index(0, 0),
                                QItemSelectionModel::SelectCurrent |
                                QItemSelectionModel::Rows);
    else onChangeItem();
}

void FormTemplateMails::refreshData()
{
    mPreloader->startAnimation();    

    for (int i = ui->tabWidget->count() - 1; i >= 0; i--) {
        delete ui->tabWidget->widget(i);
        ui->tabWidget->removeTab(i);
    }

    if (ApiAdapter::getApi()->getListTemplatesLetters(mGroups))
        mIsShowed = true;
    for (int i = 0; i < mGroups->size(); i++)
        addGroup(mGroups->at(i));
    mPreloader->stopAnimation();    
}

void FormTemplateMails::initVariables()
{
    mIsShowed = false;
    mGroups = new ListLettersGroups();    
    mPreloader = new Preloader(this);
}

void FormTemplateMails::initSignals()
{
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), SLOT(onChangeItem()));
    connect(ui->toolButtonRefresh, &QToolButton::clicked, this, &FormTemplateMails::refreshData);
    connect(ui->toolButtonAdd, SIGNAL(clicked()), SLOT(onAddEditItem()));
    connect(ui->toolButtonEdit, SIGNAL(clicked()), SLOT(onAddEditItem()));
    connect(ui->toolButtonDel, SIGNAL(clicked()), SLOT(onDeleteItem()));
    connect(ui->toolButtonAddTab, SIGNAL(clicked()), SLOT(onAddGroup()));
    connect(ui->tabWidget->tabBar(), SIGNAL(tabCloseRequested(int)), SLOT(onDeleteGroup(int)));
}

void FormTemplateMails::initPermissions()
{
    isAddPermission = UserPermission::isAdd(UserPermission::MAILS);
    isEditPermission = UserPermission::isEdit(UserPermission::MAILS);
    isDeletePermission = UserPermission::isDelete(UserPermission::MAILS);

    ui->toolButtonAdd->setEnabled(isAddPermission);
}

bool FormTemplateMails::existGroupByName(const QString &name)
{
    for (int i = 0; i < mGroups->size(); i++)
        if (mGroups->at(i)->getName().compare(name))
            return true;
    return false;
}

const DataLetterGroup *FormTemplateMails::currentGroup() const
{
    const QTreeView *tree = currentTree();
    for (int i = 0; i < mGroups->size(); i++)
        if (mGroups->at(i)->getId() == tree->objectName())
            return mGroups->at(i);
    return 0;
}

const DataItem *FormTemplateMails::currentLetter() const
{
    const DataLetterGroup *group = currentGroup();
    const QTreeView *tree = currentTree();
    if (group && tree)
        return group->templates()->at(tree->currentIndex().row());
    return 0;
}

NameTableModel *FormTemplateMails::currentModel() const
{
    const QTreeView *tree = currentTree();
    return qobject_cast <NameTableModel *> (tree->model());
}

const QTreeView *FormTemplateMails::currentTree() const
{
    return qobject_cast <QTreeView *> (ui->tabWidget->currentWidget());
}
