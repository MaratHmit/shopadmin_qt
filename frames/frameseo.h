#ifndef FRAMESEO_H
#define FRAMESEO_H

#include <QWidget>

#include "dataitem.h"
#include "request.h"
#include "apiadapter.h"
#include "flowlayout.h"

namespace Ui {
class FrameSEO;
}

class FrameSEO : public QWidget
{
    Q_OBJECT

public:
    explicit FrameSEO(QWidget *parent = 0);
    FrameSEO(QWidget *parent, DataItem *dataItem);
    ~FrameSEO();

    void setSEOHeader(const QString &seoHeader);
    void setSEOKeywords(const QString &seoKeywords);
    void setSEODescription(const QString &seoDescription);
    void setPageTitle(const QString &pageTitle);

    void hidePageTitle();

    enum SEOType { SEOGoods, SEOGoodsGroups };

    void setSEOType(const SEOType seoType);

    QString seoHeader();
    QString seoKeywords();
    QString seoDescription();
    QString pageTitle();

    void fillControls();    
    void clearInfo();

signals:
    void modified();

private slots:
    void onChangeData();
    void onButtonClickInsertSEOTagVar();

private:
    Ui::FrameSEO *ui;
    DataItem *mDataItem;
    bool mIsFilled;
    ListDataItems *mTagsVars;
    SEOType mTypeSEO;
    FlowLayout *mLayoutSEOButtons;

    void connectSignals();
    void disconnectSignals();

    void initSEOTagsVars();
    void createTagsButtons();
};

#endif // FRAMESEO_H
