#ifndef FORMREVIEWS_H
#define FORMREVIEWS_H

#include "field.h"
#include "datareview.h"
#include "request.h"
#include "nametablemodel.h"
#include "preloader.h"
#include "formreviewcard.h"

#include <QWidget>

namespace Ui {
class FormReviews;
}

class FormReviews : public QWidget
{
    Q_OBJECT

public:
    explicit FormReviews(QWidget *parent = 0);
    ~FormReviews();

signals:
    void afterShow();

private slots:
    void showContextMenu(QPoint point);
    void onChangeCurrentItem();
    void onRunSearch();
    void refreshData();

    void goNextPage();
    void goPreviosPage();
    void goLastPage();
    void goFirstPage();

    void addEditReview();
    void deleteReviews();

private:
    Ui::FormReviews *ui;
    Preloader *mPreloader;
    ListReviews *mListReviews;
    NameTableModel *mModel;
    Request mRequest;
    int mCountReviews;
    int mNumPage;
    bool mIsLoadedLayouts;
    bool mIsShowed;

    bool isAddPermission;
    bool isEditPermission;
    bool isDeletePermission;

    void showEvent(QShowEvent *);

    void initActions();
    void initFields();
    void initSignals();
    void initVariables();
    void initPermissions();

    void goToRowById(const QString &id);
    void resetRequest();
};

#endif // FORMREVIEWS_H
