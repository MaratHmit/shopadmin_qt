#ifndef FORMSETTINGS_H
#define FORMSETTINGS_H

#include "datamain.h"
#include "datapaysystem.h"
#include "dataintegration.h"
#include "nametablemodel.h"
#include "formlicense.h"
#include "formdeliveries.h"
#include "formpaysystems.h"
#include "formtemplatemails.h"
#include "formcurrencies.h"
#include "formsettingsinterface.h"
#include "formintegrationsstores.h"
#include "formseovariables.h"
#include "formpermission.h"
#include "formcatalog.h"
#include "formmeasures.h"
#include "request.h"
#include "field.h"
#include "integrationsmodel.h"
#include "integrationsdelegate.h"
#include "dialogcardgeotageting.h"

#include <QComboBox>
#include <QTableView>

namespace Ui {
class FormSettings;
}

class FormSettings : public QWidget
{
    Q_OBJECT

public:
    explicit FormSettings(QWidget *parent = 0);
    ~FormSettings();

    void refreshData();

signals:
    void changedShopName(const QString &);
    void reloadProject();

private slots:
    void saveMainSettings();
    void saveIntegrations();
    void saveDataShopParameters();
    void onTabMainSettingsChanged(int index);
    void onChangeTreePaySystems();
    void onChangeTreePayParams();
    void onChangeTreeIntegrations();
    void onChangeDataIntegrations();    
    void onChangeTreeShopParameters();
    void onChangeDataShopParameters();
    void onClickTreeShopParameters(const QModelIndex &index);

    void on_pushButtonMainSettings_clicked();
    void on_pushButtonDelivery_clicked();
    void on_pushButtonPaySystems_clicked();    
    void on_pushButtonLicenses_clicked();    
    void on_pushButtonMailTemplates_clicked();
    void on_pushButtonCurrencies_clicked();    
    void on_pushButtonInterface_clicked();
    void on_pushButtonStores_clicked();
    void on_pushButtonSEOVariables_clicked();
    void on_pushButtonPermission_clicked();
    void on_pushButtonGeotargeting_clicked();

    void onAddParamClicked();
    void onSetValueClicked();
    void onDeleteParamClicked();

    void onCreateDump();
    void onRestoreDump();
    void onImagesMigration();
    void onImagesDelFromBD();

    void clearCache();

    void on_pushButtonMeasures_clicked();

private:
    Ui::FormSettings *ui;
    NameTableModel *mPaysSystemsModel;
    NameTableModel *mPayReqsModel;
    ListDataItems *mPaySystems;
    ListDataItems *mPayReqs;
    ListIntegrations *mIntegrations;
    ListIntegrations mSaveIntegrations;    
    bool mIsShowed;
    bool mIsShowedMainPaySystems;    
    bool mIsShowedShopParameters;
    DataMain *mDataMain;
    Request mRequest;        
    Preloader *mPreloader;
    QString mFolderDumps;

    FormLicense *mFormLicenses;    
    FormDeliveries *mFormDeliveries;    
    FormPaySystems *mFormPaySystems;
    FormTemplateMails *mFormTemplateMails;
    FormCurrencies *mFormCurrencies;    
    FormSettingsInterface *mFormInterface;
    FormIntegrationsStores *mFormStores;
    FormSEOVariables *mFormSEOVariables;
    FormPermission *mFormPermission;
    FormCatalog *mFormGeotargeting;
    FormMeasures *mFormMeasures;

    Api *api;

    QVector<IntegrationsModel *> mListIntegrationsModels;
    QVector<QTableView *> mListIntegrationsTree;
    QVector<QLabel *> mListIntegrationsNoteLabels;

    QVector<IntegrationsModel *> mListShopParametersModels;
    QVector<QTableView *> mListShopParametersTree;
    QVector<QLabel *> mListShopParametersNoteLabels;

    void showEvent(QShowEvent *e);
    void timerEvent(QTimerEvent *e);
    bool eventFilter(QObject *object, QEvent *event);
    bool eventKeysTreePayParams(QKeyEvent *event);

    void initVariables();
    void initValidators();
    void initSignals();    
    void initPermissions();
    void setDataMain();
    void fillControlsForMain();        

    void refreshPayParams();
    void showMainPaySystems();

    void showIntegrations();
    void refreshIntegrations();

    void showShopParameters();
    void refreshShopParameters();
};

#endif // FORMSETTINGS_H
