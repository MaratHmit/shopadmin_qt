#include "formselectmodifications.h"
#include "ui_formselectmodifications.h"
#include "userpermission.h"

#include <QDebug>

#include "seconsts.h"

FormSelectModifications::FormSelectModifications(
        QWidget *parent, DataModificationGroup *group, const bool isStealthMode) :
    QDialog(parent),
    ui(new Ui::FormSelectModifications)
{
    ui->setupUi(this);
    mGroup = group;
    mFormsColumns = new QList<FormFeaturesList *> ();
    mSelectedList = new ListModifications();
    mIsStealthMode = isStealthMode;
    initSignals();    
    initColumns();
}

FormSelectModifications::~FormSelectModifications()
{    
    while (!mSelectedList->empty())
        delete mSelectedList->takeLast();
    delete ui;
    delete mFormsColumns;
    delete mSelectedList;
}

const ListModifications *FormSelectModifications::selectedList() const
{
    return mSelectedList;
}

void FormSelectModifications::onApply()
{
    if (setSelectedList())
        accept();
}

void FormSelectModifications::initSignals()
{
    connect(ui->buttonCancel, SIGNAL(clicked()), SLOT(reject()));
    connect(ui->buttonSave, SIGNAL(clicked()), SLOT(onApply()));
}

void FormSelectModifications::initColumns()
{
    for (int i = 0; i < mGroup->columns()->size(); i++) {
        FormFeaturesList *frameColumn = new FormFeaturesList(this);
        QStringList list;
        for (int j = 0; j < mGroup->modifications()->size(); j++)
            for (int k = 0; k < mGroup->modifications()->at(j)->values()->size(); k++)
                list.append(mGroup->modifications()->at(j)->values()->at(k)->getId());
        frameColumn->setFeature(mGroup->columns()->at(i));
        frameColumn->setSelectModificationMode();
        frameColumn->setSelectedValues(list);
        if (mIsStealthMode) {
            frameColumn->refreshData();
            frameColumn->selectAll();
        }
        ui->horizontalLayoutFrames->addWidget(frameColumn);
        mFormsColumns->push_back(frameColumn);
    }
}

bool FormSelectModifications::setSelectedList()
{
    setCursor(Qt::WaitCursor);

    QList<int> indexes, counts;
    int n = 0;
    while (!mSelectedList->empty())
        delete mSelectedList->takeLast();

    for (int i = 0; i < mFormsColumns->size(); i++) {
        indexes.append(0);
        counts.append(mFormsColumns->at(i)->selectedValues()->size());
        if (!counts.at(i)) {
            setCursor(Qt::ArrowCursor);
            return false;
        }
        if (!n)
            n = mFormsColumns->at(i)->selectedValues()->size();
        else n *= mFormsColumns->at(i)->selectedValues()->size();
    }

    if (n > SEConsts::MAX_COUNT_MODIFICATIONS)
        n = SEConsts::MAX_COUNT_MODIFICATIONS;

    for (int p = 0; p < n; p++) {

        DataModification *modification = new DataModification();
        for (int i = 0; i < indexes.size(); i++) {
            modification->values()->append(
                        new DataFeature(mFormsColumns->at(i)->
                                        selectedValues()->at(indexes.at(i))), false);
        }
        mSelectedList->append(modification, false);

        for (int i = indexes.size() - 1; i >= 0; i--) {
            if (indexes.at(i) < counts.at(i) - 1) {
                indexes[i]++;
                for (int j = i + 1; j < indexes.size(); ++j)
                    indexes[j] = 0;
                break;
            }
        }
    }

    setCursor(Qt::ArrowCursor);
    return true;
}
