#-------------------------------------------------
#
# Project created by QtCreator 2014-04-17T13:43:46
#
#-------------------------------------------------

QT       += core gui
QT       += network
QT       += webkit
QT       += webkitwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = managerSE
TEMPLATE = app

SOURCES += \
    contacts/formcontactcard.cpp \
    contacts/formcontactgroupcard.cpp \
    contacts/formcontacts.cpp \
    contacts/formcontactsgroups.cpp \
    contacts/formsetgroups.cpp \
    core/main.cpp \
    core/formmain.cpp \
    core/formlogin.cpp \
    core/api.cpp \
    core/preloader.cpp \
    core/seconfig.cpp \
    core/seconsts.cpp \    
    core/apiadapter.cpp \
    core/request.cpp \
    core/requestimages.cpp \
    core/requestfeature.cpp \
    core/xmlhighlighter.cpp \
    data-classes/datacontact.cpp \
    data-classes/dataorder.cpp \
    data-classes/dataproduct.cpp \
    data-classes/dataproductsgroup.cpp \
    data-classes/datafeature.cpp \
    data-classes/datamain.cpp \
    data-classes/datadelivery.cpp \
    data-classes/datalicense.cpp \
    data-classes/datacurrency.cpp \
    data-classes/datamodification.cpp \
    data-classes/datamodificationgroup.cpp \
    data-classes/datapayrequisite.cpp \
    data-classes/datapersonalaccount.cpp \
    data-classes/datadeliveryconditions.cpp \
    data-classes/dataitem.cpp \
    data-classes/datapaysystem.cpp \
    data-classes/dataletter.cpp \
    data-classes/datalettergroup.cpp \
    data-classes/field.cpp \
    data-classes/datanew.cpp \
    data-classes/datacoupon.cpp \
    frames/htmleditor/framehtmleditor.cpp \    
    frames/framepagenavigator.cpp \    
    frames/frameimage.cpp \
    frames/frameseo.cpp \
    frames/framesearchtable.cpp \
    frames/htmleditor/dialogseturl.cpp \
    frames/htmleditor/dialoginsertimage.cpp \
    frames/htmleditor/dialoghtmltable.cpp \
    frames/framelistimages.cpp \
    frames/frameimagedescription.cpp \
    frames/activelabel.cpp \
    frames/htmleditor/framewysiwyg.cpp \
    lib/seutils.cpp \
    lib/formsetvisibleheadercolumns.cpp \
    models/nametablemodel.cpp \
    models/licensetablemodel.cpp \
    models/modificationsmodel.cpp \
    models/listimagesdelegate.cpp \
    models/modificationsimagesmodel.cpp \
    orders/formorders.cpp \    
    products/brands/formbrands.cpp \
    products/brands/formbrandcard.cpp \
    products/features/formfeaturecard.cpp \
    products/features/formfeaturesgroups.cpp \
    products/features/formfeatureslist.cpp \
    products/features/formfeaturevaluecard.cpp \
    products/features/formfeaturegroupcard.cpp \
    products/features/formfeatures.cpp \
    products/groups/formproductgroupcard.cpp \
    products/groups/formproductsgroups.cpp \
    products/formproducts.cpp \ 
    products/formproductcard.cpp \
    products/cardframes/framecardmoreinfo.cpp \
    products/cardframes/framecardmaininfo.cpp \
    products/cardframes/framecardspecifications.cpp \
    products/cardframes/groupspecifications.cpp \
    products/cardframes/framecardmodifications.cpp \
    products/cardframes/widgetpagemodifications.cpp \
    products/features/formmodificationsgroups.cpp \
    products/features/formmodificationgroupcard.cpp \
    products/features/formmodificationsgroupsvalues.cpp \
    products/features/formselectmodifications.cpp \
    products/cardframes/framecardsimilar.cpp \
    products/cardframes/framecardcrossgroups.cpp \
    products/coupons/formcoupons.cpp \
    products/coupons/formcouponcard.cpp \
    products/discounts/formdiscounts.cpp \
    feedback/comments/formcommentcard.cpp \
    publications/formpublications.cpp \
    publications/formnewsgroups.cpp \
    settings/deliveries/dialogdelivcondition.cpp \
    settings/deliveries/formcarddelivery.cpp \
    settings/deliveries/formdeliveries.cpp \
    libimages/formlibimages.cpp \
    libimages/frameliblistimages.cpp \
    settings/license/formcardlicense.cpp \
    settings/license/formlicense.cpp \
    settings/main/dialogaddparampaysystem.cpp \
    settings/main/dialogsetpayparamvalue.cpp \
    settings/paysystems/formpaysystems.cpp \
    settings/formsettings.cpp \    
    settings/paysystems/formpaysystemcard.cpp \
    settings/paysystems/dialoghostname.cpp \
    settings/template_mails/formtemplatemails.cpp \
    settings/template_mails/dialoglettercard.cpp \
    settings/template_mails/dialogaddgroupletters.cpp \
    settings/currencies/formcurrencies.cpp \
    publications/formpublicationcard.cpp \
    publications/formnewsgroupcard.cpp \
    settings/currencies/formcurrencycard.cpp \
    settings/currencies/dialogsetrate.cpp \
    data-classes/datadiscount.cpp \
    products/discounts/formdiscountcard.cpp \
    products/groups/formparametersfilters.cpp \
    products/cardframes/framecarddiscounts.cpp \
    core/dialogregistration.cpp \
    orders/formordercard.cpp \
    orders/dialogproductitem.cpp \
    payments/formpayments.cpp \
    data-classes/datapayment.cpp \
    payments/formpaymentcard.cpp \
    frames/htmleditor/frametabshtmleditor.cpp \
    core/updater.cpp \
    products/dialogproductmodifications.cpp \
    data-classes/datatypedelivery.cpp \
    data-classes/dataorderitem.cpp \
    core/dialogabout.cpp \
    frames/dialogsetimageurl.cpp \
    data-classes/datacompanyrequisite.cpp \
    products/dialogaddprice.cpp \
    settings/deliveries/dialogdelivregion.cpp \
    lib/dialogexport.cpp \
    data-classes/dataintegration.cpp \
    models/integrationsmodel.cpp \
    models/integrationsdelegate.cpp \
    settings/managers/formmanagers.cpp \
    orders/dialogsetorderstatus.cpp \
    lib/dialogimport.cpp \
    core/dialogproxysettings.cpp \
    feedback/reviews/formreviews.cpp \
    data-classes/datareview.cpp \
    feedback/reviews/formreviewcard.cpp \
    products/dialogsetprices.cpp \
    contacts/dialogaddeditaccount.cpp \
    settings/interface/formsettingsinterface.cpp \
    settings/interface/dialogdynfieldsgroup.cpp \
    settings/interface/dialogdynfielditem.cpp \
    lib/csvreader.cpp \
    data-classes/dataimages.cpp \
    widgets/flowlayout.cpp \
    sesections/formsesections.cpp \
    sesections/dialogsesection.cpp \
    sesections/dialogsesectionitem.cpp \
    settings/integration_stores/formintegrationsstores.cpp \
    settings/integration_stores/checkoauthtoken.cpp \
    settings/seovariables/formseovariables.cpp \
    settings/seovariables/dialogseovariable.cpp \
    lib/dialogyacategories.cpp \
    core/formcatalog.cpp \
    core/dialogcard.cpp \
    products/goodstypes/dialogcardproducttype.cpp \
    settings/permission/formpermission.cpp \
    settings/permission/dialogsetroles.cpp \
    settings/permission/dialogcardrole.cpp \
    settings/permission/formpermissionobjects.cpp \
    models/permissionsmodel.cpp \
    models/permissionsdelegate.cpp \
    core/userpermission.cpp \
    settings/dialogmigrationimages.cpp \
    core/dialogauthcheck.cpp \
    lib/filewebloader.cpp \
    products/cardframes/framecardfiles.cpp \
    lib/filewebuploader.cpp \
    products/dialogaddfromymurl.cpp \
    settings/dialogcardgeotageting.cpp \
    contacts/formcompanycard.cpp \
    data-classes/datacompany.cpp \
    settings/interface/framecustomfields.cpp \
    frames/framecustomfieldscard.cpp \
    products/dialogsetrates.cpp \
    contacts/dialogpricetype.cpp \
    core/dialogcomplaint.cpp \
    products/groups/dialoggroupsmasterimport.cpp \
    frames/frameimportcolumn.cpp \
    products/dialogproductsmasterimport.cpp \
    dialogadddescription.cpp \
    settings/measures/formmeasures.cpp \
    settings/measures/dialogcardmeasure.cpp \
    products/groups/dialogincprices.cpp \
    products/options/formoptions.cpp \
    products/options/formoptionsgroups.cpp \
    products/options/formoptionsparams.cpp \
    products/options/formoptiongroupcard.cpp \
    products/options/formoptioncart.cpp \
    products/options/formoptionvaluecard.cpp \
    products/cardframes/framecardoptions.cpp \
    products/cardframes/widgetpageoptions.cpp \
    data-classes/dataoption.cpp \
    data-classes/dataoptionvalue.cpp \
    orders/dialogcardpreorder.cpp \
    orders/dialogcardcreditrequest.cpp \
    products/goodstypes/dialogcardproductlabel.cpp

HEADERS  += \
    contacts/formcontactcard.h \
    contacts/formcontactgroupcard.h \
    contacts/formcontacts.h \
    contacts/formcontactsgroups.h \
    core/formmain.h \
    core/formlogin.h \
    core/api.h \
    core/preloader.h \
    core/seconfig.h \
    core/seconsts.h \
    core/apiadapter.h \    
    frames/htmleditor/framehtmleditor.h \    
    frames/framepagenavigator.h \        
    frames/frameimage.h \
    frames/frameseo.h \
    frames/framesearchtable.h \
    frames/htmleditor/dialogseturl.h \
    frames/htmleditor/dialoginsertimage.h \
    frames/htmleditor/dialoghtmltable.h \
    lib/seutils.h \
    orders/formorders.h \    
    publications/formpublications.h \
    data-classes/datacontact.h \
    data-classes/dataorder.h \
    data-classes/dataproduct.h \
    data-classes/dataproductsgroup.h \
    data-classes/datafeature.h \
    data-classes/datamain.h \
    models/nametablemodel.h \
    products/brands/formbrands.h \
    products/brands/formbrandcard.h \
    products/features/formfeaturecard.h \
    products/features/formfeaturesgroups.h \
    products/features/formfeatureslist.h \
    products/features/formfeaturevaluecard.h \
    products/features/formfeaturegroupcard.h \
    products/features/formfeatures.h \    
    products/groups/formproductgroupcard.h \
    products/groups/formproductsgroups.h \
    products/coupons/formcoupons.h \
    products/formproducts.h \
    data-classes/datadelivery.h \        
    data-classes/datalicense.h \
    models/licensetablemodel.h \    
    products/formproductcard.h \
    data-classes/datacurrency.h \
    products/cardframes/framecardmoreinfo.h \
    products/cardframes/framecardmaininfo.h \
    frames/framelistimages.h \    
    frames/frameimagedescription.h \
    frames/activelabel.h \
    products/cardframes/framecardspecifications.h \
    products/cardframes/groupspecifications.h \
    data-classes/datapayrequisite.h \
    products/cardframes/framecardmodifications.h \
    products/cardframes/widgetpagemodifications.h \
    products/features/formmodificationsgroups.h \
    products/features/formmodificationgroupcard.h \
    products/features/formmodificationsgroupsvalues.h \
    data-classes/datamodification.h \
    data-classes/datamodificationgroup.h \
    models/modificationsmodel.h \
    products/features/formselectmodifications.h \
    core/request.h \
    core/requestimages.h \
    core/requestfeature.h \
    products/cardframes/framecardsimilar.h \
    data-classes/dataitem.h \
    feedback/comments/formcommentcard.h \
    products/cardframes/framecardcrossgroups.h \
    models/listimagesdelegate.h \
    lib/formsetvisibleheadercolumns.h \
    contacts/formsetgroups.h \
    data-classes/datapersonalaccount.h \
    models/modificationsimagesmodel.h \
    frames/htmleditor/framewysiwyg.h \
    data-classes/datadeliveryconditions.h \
    core/xmlhighlighter.h \
    settings/deliveries/dialogdelivcondition.h \
    settings/deliveries/formcarddelivery.h \
    settings/deliveries/formdeliveries.h \
    libimages/formlibimages.h \
    libimages/frameliblistimages.h \
    settings/license/formcardlicense.h \
    settings/license/formlicense.h \
    settings/main/dialogaddparampaysystem.h \
    settings/main/dialogsetpayparamvalue.h \    
    settings/paysystems/formpaysystems.h \
    settings/formsettings.h \
    data-classes/datapaysystem.h \
    settings/paysystems/formpaysystemcard.h \
    settings/paysystems/dialoghostname.h \
    settings/template_mails/formtemplatemails.h \
    data-classes/dataletter.h \
    data-classes/datalettergroup.h \
    settings/template_mails/dialoglettercard.h \
    settings/template_mails/dialogaddgroupletters.h \
    publications/formnewsgroups.h \
    data-classes/field.h \
    data-classes/datanew.h \
    publications/formpublicationcard.h \
    publications/formnewsgroupcard.h \
    settings/currencies/formcurrencies.h \
    settings/currencies/formcurrencycard.h \
    settings/currencies/dialogsetrate.h \
    data-classes/datacoupon.h \
    products/coupons/formcouponcard.h \
    products/discounts/formdiscounts.h \
    data-classes/datadiscount.h \
    products/discounts/formdiscountcard.h \
    products/groups/formparametersfilters.h \
    products/cardframes/framecarddiscounts.h \
    core/dialogregistration.h \
    orders/formordercard.h \
    orders/dialogproductitem.h \
    payments/formpayments.h \
    data-classes/datapayment.h \
    payments/formpaymentcard.h \
    frames/htmleditor/frametabshtmleditor.h \
    core/updater.h \
    products/dialogproductmodifications.h \
    data-classes/datatypedelivery.h \
    data-classes/dataorderitem.h \
    core/dialogabout.h \
    frames/dialogsetimageurl.h \
    data-classes/datacompanyrequisite.h \
    products/dialogaddprice.h \
    settings/deliveries/dialogdelivregion.h \
    lib/dialogexport.h \
    data-classes/dataintegration.h \
    models/integrationsmodel.h \
    models/integrationsdelegate.h \
    settings/managers/formmanagers.h \
    orders/dialogsetorderstatus.h \
    lib/dialogimport.h \
    core/dialogproxysettings.h \
    feedback/reviews/formreviews.h \
    data-classes/datareview.h \
    feedback/reviews/formreviewcard.h \
    products/dialogsetprices.h \
    contacts/dialogaddeditaccount.h \
    settings/interface/formsettingsinterface.h \
    settings/interface/dialogdynfieldsgroup.h \
    settings/interface/dialogdynfielditem.h \
    lib/csvreader.h \
    data-classes/dataimages.h \
    widgets/flowlayout.h \
    sesections/formsesections.h \
    sesections/dialogsesection.h \
    sesections/dialogsesectionitem.h \
    settings/integration_stores/formintegrationsstores.h \
    settings/integration_stores/checkoauthtoken.h \
    settings/seovariables/formseovariables.h \
    settings/seovariables/dialogseovariable.h \
    lib/dialogyacategories.h \
    core/formcatalog.h \
    core/dialogcard.h \
    products/goodstypes/dialogcardproducttype.h \
    settings/permission/formpermission.h \
    settings/permission/dialogsetroles.h \
    settings/permission/dialogcardrole.h \
    settings/permission/formpermissionobjects.h \
    models/permissionsmodel.h \
    models/permissionsdelegate.h \
    core/userpermission.h \
    settings/dialogmigrationimages.h \
    core/dialogauthcheck.h \
    lib/filewebloader.h \
    products/cardframes/framecardfiles.h \
    lib/filewebuploader.h \
    products/dialogaddfromymurl.h \
    settings/dialogcardgeotageting.h \
    contacts/formcompanycard.h \
    data-classes/datacompany.h \
    settings/interface/framecustomfields.h \
    frames/framecustomfieldscard.h \
    products/dialogsetrates.h \
    contacts/dialogpricetype.h \
    core/dialogcomplaint.h \
    products/groups/dialoggroupsmasterimport.h \
    frames/frameimportcolumn.h \
    products/dialogproductsmasterimport.h \
    dialogadddescription.h \
    settings/measures/formmeasures.h \
    settings/measures/dialogcardmeasure.h \
    products/groups/dialogincprices.h \
    products/options/formoptions.h \
    products/options/formoptionsgroups.h \
    products/options/formoptionsparams.h \
    products/options/formoptiongroupcard.h \
    products/options/formoptioncart.h \
    products/options/formoptionvaluecard.h \
    products/cardframes/framecardoptions.h \
    products/cardframes/widgetpageoptions.h \
    data-classes/dataoption.h \
    data-classes/dataoptionvalue.h \
    orders/dialogcardpreorder.h \
    orders/dialogcardcreditrequest.h \
    products/goodstypes/dialogcardproductlabel.h

FORMS    += \
    core/formlogin.ui \
    orders/formorders.ui \
    frames/framepagenavigator.ui \
    publications/formpublications.ui \    
    frames/htmleditor/framehtmleditor.ui \        
    contacts/formcontactcard.ui \
    contacts/formcontactgroupcard.ui \
    frames/frameimage.ui \
    frames/frameseo.ui \
    frames/framesearchtable.ui \    
    contacts/formcontacts.ui \
    contacts/formcontactsgroups.ui \
    products/features/formfeatureslist.ui \
    products/features/formfeaturevaluecard.ui \
    products/features/formfeatures.ui \
    products/features/formfeaturegroupcard.ui \
    products/features/formfeaturecard.ui \
    products/features/formfeaturesgroups.ui \
    products/groups/formproductgroupcard.ui \
    products/brands/formbrandcard.ui \
    products/brands/formbrands.ui \
    products/groups/formproductsgroups.ui \
    products/formproducts.ui \
    frames/htmleditor/dialogseturl.ui \
    frames/htmleditor/dialoginsertimage.ui \
    frames/htmleditor/dialoghtmltable.ui \
    products/formproductcard.ui \
    products/cardframes/framecardmoreinfo.ui \
    products/cardframes/framecardmaininfo.ui \
    frames/framelistimages.ui \
    frames/frameimagedescription.ui \
    products/cardframes/framecardspecifications.ui \
    products/cardframes/framecardmodifications.ui \
    products/cardframes/widgetpagemodifications.ui \
    products/features/formmodificationsgroups.ui \
    products/features/formmodificationgroupcard.ui \
    products/features/formmodificationsgroupsvalues.ui \
    products/features/formselectmodifications.ui \
    products/cardframes/framecardsimilar.ui \
    feedback/comments/formcommentcard.ui \
    products/cardframes/framecardcrossgroups.ui \
    lib/formsetvisibleheadercolumns.ui \
    contacts/formsetgroups.ui \
    frames/htmleditor/framewysiwyg.ui \            
    settings/deliveries/dialogdelivcondition.ui \
    settings/deliveries/formcarddelivery.ui \
    settings/deliveries/formdeliveries.ui \
    libimages/formlibimages.ui \
    libimages/frameliblistimages.ui \
    settings/license/formcardlicense.ui \
    settings/license/formlicense.ui \
    settings/main/dialogaddparampaysystem.ui \
    settings/main/dialogsetpayparamvalue.ui \    
    settings/paysystems/formpaysystems.ui \
    settings/formsettings.ui \
    settings/paysystems/formpaysystemcard.ui \
    settings/paysystems/dialoghostname.ui \
    settings/template_mails/formtemplatemails.ui \
    settings/template_mails/dialoglettercard.ui \
    settings/template_mails/dialogaddgroupletters.ui \
    publications/formnewsgroups.ui \
    publications/formpublicationcard.ui \
    publications/formnewsgroupcard.ui \
    settings/currencies/formcurrencies.ui \
    settings/currencies/formcurrencycard.ui \
    settings/currencies/dialogsetrate.ui \
    products/coupons/formcoupons.ui \
    products/coupons/formcouponcard.ui \
    products/discounts/formdiscounts.ui \
    products/discounts/formdiscountcard.ui \
    products/groups/formparametersfilters.ui \
    products/cardframes/framecarddiscounts.ui \
    core/dialogregistration.ui \
    orders/formordercard.ui \
    orders/dialogproductitem.ui \
    payments/formpayments.ui \
    payments/formpaymentcard.ui \
    frames/htmleditor/frametabshtmleditor.ui \
    products/dialogproductmodifications.ui \
    core/dialogabout.ui \
    frames/dialogsetimageurl.ui \
    products/dialogaddprice.ui \
    settings/deliveries/dialogdelivregion.ui \
    lib/dialogexport.ui \
    settings/managers/formmanagers.ui \
    orders/dialogsetorderstatus.ui \
    lib/dialogimport.ui \
    core/dialogproxysettings.ui \
    feedback/reviews/formreviews.ui \
    feedback/reviews/formreviewcard.ui \
    products/dialogsetprices.ui \
    contacts/dialogaddeditaccount.ui \
    settings/interface/formsettingsinterface.ui \
    settings/interface/dialogdynfieldsgroup.ui \
    settings/interface/dialogdynfielditem.ui \
    sesections/formsesections.ui \
    sesections/dialogsesection.ui \
    sesections/dialogsesectionitem.ui \
    settings/integration_stores/formintegrationsstores.ui \
    settings/seovariables/formseovariables.ui \
    settings/seovariables/dialogseovariable.ui \
    lib/dialogyacategories.ui \
    products/goodstypes/dialogcardproducttype.ui \
    settings/permission/formpermission.ui \
    settings/permission/dialogsetroles.ui \
    settings/permission/dialogcardrole.ui \
    settings/permission/formpermissionobjects.ui \
    settings/dialogmigrationimages.ui \
    core/dialogauthcheck.ui \
    products/cardframes/framecardfiles.ui \
    products/dialogaddfromymurl.ui \
    settings/dialogcardgeotageting.ui \
    contacts/formcompanycard.ui \
    settings/interface/framecustomfields.ui \
    frames/framecustomfieldscard.ui \
    products/dialogsetrates.ui \
    contacts/dialogpricetype.ui \
    core/dialogcomplaint.ui \
    products/groups/dialoggroupsmasterimport.ui \
    frames/frameimportcolumn.ui \
    products/dialogproductsmasterimport.ui \
    dialogadddescription.ui \
    settings/measures/formmeasures.ui \
    settings/measures/dialogcardmeasure.ui \
    products/groups/dialogincprices.ui \
    products/options/formoptions.ui \
    products/options/formoptionsgroups.ui \
    products/options/formoptionsparams.ui \
    products/options/formoptiongroupcard.ui \
    products/options/formoptioncart.ui \
    products/options/formoptionvaluecard.ui \
    products/cardframes/framecardoptions.ui \
    products/cardframes/widgetpageoptions.ui \
    orders/dialogcardpreorder.ui \
    orders/dialogcardcreditrequest.ui \
    products/goodstypes/dialogcardproductlabel.ui

RESOURCES += \
    res.qrc

RC_FILE = icon.rc

INCLUDEPATH += $$PWD \
    $$PWD/core \
    $$PWD/lib \
    $$PWD/frames \
    $$PWD/frames/htmleditor \
    $$PWD/contacts \
    $$PWD/orders \
    $$PWD/payments \
    $$PWD/products \
    $$PWD/products/brands \
    $$PWD/products/groups \
    $$PWD/products/features \
    $$PWD/products/coupons \
    $$PWD/products/cardframes \    
    $$PWD/products/discounts \
    $$PWD/products/goodstypes \
    $$PWD/products/options \
    $$PWD/feedback/comments \
    $$PWD/feedback/reviews \
    $$PWD/libimages \
    $$PWD/settings \
    $$PWD/settings/deliveries \
    $$PWD/settings/license \
    $$PWD/settings/main \    
    $$PWD/settings/regions \
    $$PWD/settings/paysystems \
    $$PWD/settings/template_mails \
    $$PWD/settings/currencies \
    $$PWD/settings/managers \
    $$PWD/settings/interface \
    $$PWD/settings/integration_stores \
    $$PWD/settings/seovariables \
    $$PWD/settings/permission \
    $$PWD/settings/measures \
    $$PWD/publications \
    $$PWD/data-classes \
    $$PWD/models \
    $$PWD/sesections \
    $$PWD/widgets

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

OTHER_FILES +=

CONFIG += c++11