#include "dataitem.h"
#include "seconfig.h"
#include "apiadapter.h"
#include "seutils.h"

#include <QDebug>
#include <QMetaProperty>

DataItem::DataItem()
{    
    initialization();
}

DataItem::DataItem(const DataItem *d)
{
    initialization();
    copy(d);
}

DataItem::DataItem(const QJsonObject &jsonobject)
{
    initialization();
    setFromJSONObject(jsonobject);
}

DataItem::~DataItem()
{
    delete pixmap;
}

void DataItem::initialization()
{
    setParent(0);
    isValueEditableMode = false;
    container = 0;    
    idsGroups = new QVector<QString>;
    sortIndex = 0;
    isActive = true;
    isMain = false;
    container = 0;
    countChilds = 0;
    isChecked = false;
    isSelected = false;
    isSetAllFields = false;
    pixmap = new QPixmap;
    isCloneMode = false;
    isFilledChilds = false;
    QPixmap ico(":/32/icons/32x32/view.png");
    pixmapIsActive = ico.scaled(QSize(16, 16));
}

void DataItem::moveChildrens(int from, int to)
{
    d_ptr->children.move(from, to);
}

void DataItem::copy(const DataItem *d)
{
    ids.clear();
    for (int i = 0; i < d->ids.size(); i++)
        ids.append(d->ids.at(i));
    idParent = d->idParent;
    idGroup = d->idGroup;
    idValue = d->idValue;
    sortIndex = d->sortIndex;
    countChilds = d->countChilds;
    code = d->code;
    name = d->name;
    nameGroup = d->nameGroup;
    imageFile = d->imageFile;
    imageAlt = d->imageAlt;    
    value = d->value;
    valueType = d->valueType;
    isChecked = d->isChecked;
    isActive = d->isActive;
    isMain = d->isMain;
    *pixmap = *d->pixmap;
    isFilledChilds = d->isFilledChilds;
    idsChilds = d->idsChilds;
    url = d->url;
    mIdManager = d->mIdManager;
    mManager = d->mManager;

    idsGroups->clear();
    for (int i = 0; i < d->idsGroups->size(); i++)
        idsGroups->push_back(d->idsGroups->at(i));

    modifiedFields.clear();
    for (int i = 0; i < d->modifiedFields.size(); i++)
        modifiedFields.append(d->modifiedFields.at(i));

    listValues.clear();
    for (int i = 0; i < d->listValues.size(); i++)
        listValues.append(d->getValues().at(i));

    for(int i = 0; i < d->dynamicPropertyNames().size(); ++i)
        QObject::setProperty(d->dynamicPropertyNames().at(i), d->property(d->dynamicPropertyNames().at(i)));
}

void DataItem::setId(const QString &id)
{
    ids.clear();
    if (!id.isNull() && !id.isEmpty())
        addId(id);
}

void DataItem::setIdMain(const QString &idMain)
{
    if (this->idMain != idMain) {
        this->idMain = idMain;
        setModified("idMain");
    }
}

void DataItem::setIdGroup(const QString &idGroup)
{
    if (this->idGroup != idGroup) {
        this->idGroup = idGroup;
        setModified("idGroup");
    }
}

void DataItem::setIdParent(const QString &idParent)
{
    if (this->idParent != idParent) {
        this->idParent = idParent;
        setModified("idParent");
    }
}

void DataItem::setSortIndex(const int &sortIndex)
{    
    if (this->sortIndex != sortIndex) {
        this->sortIndex = sortIndex;
        setModified("sortIndex");
    }
}

void DataItem::setCode(const QString &code)
{
    if (this->code != code) {
        this->code = code;
        setModified("code");
    }
}

void DataItem::setName(const QString &name)
{
    if (this->name != name) {
        this->name = name;
        setModified("name");
    }
}

void DataItem::setNameGroup(const QString &nameGroup)
{
    if (this->nameGroup != nameGroup) {
        this->nameGroup = nameGroup;
        setModified("nameGroup");
    }
}

void DataItem::setNote(const QString &note)
{
    if (note != property("note").toString()) {
        setProperty("note", note);
        setModified("note");
    }
}

void DataItem::setImageFile(const QString &imageFile)
{
    if (this->imageFile != imageFile) {
        this->imageFile = imageFile;
        setModified("imageFile");
    }
}

void DataItem::setImageAlt(const QString &imageAlt)
{
    if (this->imageAlt != imageAlt) {
        this->imageAlt = imageAlt;
        setModified("imageAlt");
    }
}

void DataItem::setDescription(const QString &description)
{
    if (description != property("description").toString()) {
        setProperty("description", description);
        setModified("description");
    }
}

void DataItem::setFullDescription(const QString &fullDescription)
{
    if (fullDescription != property("fullDescription").toString()) {
        setProperty("fullDescription", fullDescription);
        setModified("fullDescription");
    }
}

void DataItem::setSEOHeader(const QString &seoHeader)
{
    if (seoHeader != property("seoHeader").toString()) {
        setProperty("seoHeader", seoHeader);
        setModified("seoHeader");
    }
}

void DataItem::setSEOKeywords(const QString &seoKeywords)
{
    if (seoKeywords != property("seoKeywords").toString()) {
        setProperty("seoKeywords", seoKeywords);
        setModified("seoKeywords");
    }
}

void DataItem::setSEODescription(const QString &seoDescription)
{
    if (seoDescription != property("seoDescription").toString()) {
        setProperty("seoDescription", seoDescription);
        setModified("seoDescription");
    }
}

void DataItem::setPageTitle(const QString &pageTitle)
{
    if (pageTitle != property("pageTitle").toString()) {
        setProperty("pageTitle", pageTitle);
        setModified("pageTitle");
    }
}

void DataItem::setIsActive(const bool &isActive)
{
    if (this->isActive != isActive) {
        this->isActive = isActive;
        setModified("isActive");
    }
}

void DataItem::setIsChecked(const bool &isChecked)
{
    if (this->isChecked != isChecked) {
        this->isChecked = isChecked;
        setModified("isChecked");
    }
}

void DataItem::setIsCheckedChilds(const bool &isChecked)
{
    for (int i = 0; i < children().size(); i++) {
        DataItem *item = static_cast <DataItem *> (children().at(i));
        item->setIsChecked(isChecked);
        item->setIsCheckedChilds(isChecked);
    }
}

void DataItem::setValue(const QString &value)
{
    if (this->value != value) {
        this->value = value;
        setModified("value");
    }
}

void DataItem::setIsSelected(const bool &isSelected)
{
    this->isSelected = isSelected;
}

void DataItem::setIsMain(const bool &isMain)
{    
    if (this->isMain != isMain) {
        this->isMain = isMain;
        setModified("isMain");
    }
}

void DataItem::setContainer(void *container)
{
    this->container = container;
}

void DataItem::setCloneMode(const bool &isCloneMode)
{
    this->isCloneMode = isCloneMode;
}

void DataItem::setValueType(const QString &valueType)
{
    if (this->valueType != valueType) {
        this->valueType = valueType;
        setModified("valueType");
    }
}

void DataItem::setIdValue(const QString &idValue)
{
    if (this->idValue != idValue) {
        this->idValue = idValue;
        setModified("idValue");
    }
}

void DataItem::setListValues(const QString &values)
{
    listValues = values.split(",");
    setModified("listValues");
}

void DataItem::setUrl(const QString &url)
{
    if (this->url != url) {
        this->url = url;
        setModified("url");
    }
}

void DataItem::setIdManager(const QString &idManager)
{
    if (mIdManager != idManager) {
        mIdManager = idManager;
        setModified("idManager");
    }
}

void DataItem::setManager(const QString &manager)
{
    if (mManager != manager) {
        mManager = manager;
        setModified("manager");
    }
}

void DataItem::addId(const QString &id)
{
    if (!ids.contains(id))
        ids.append(id);            
}

void DataItem::addIdGroup(const QString &id)
{
    if (!idsGroups->contains(id)) {
        idsGroups->append(id);
        setModified("idsGroups");
    }
}

void DataItem::delIdGroup(const QString &id)
{
    if (!idsGroups->contains("id")) {
        idsGroups->remove(idsGroups->indexOf(id));
        setModified("idsGroups");
    }
}

void DataItem::clearGroups()
{
    idsGroups->clear();
}

void DataItem::setFromJSONObject(const QJsonObject &jsonobject)
{
    if (!jsonobject["id"].isNull())
        setId(jsonobject["id"].toString());
    if (!jsonobject["sortIndex"].isNull())
        sortIndex = jsonobject["sortIndex"].toInt();
    if (!jsonobject["countChilds"].isNull())
        countChilds = jsonobject["countChilds"].toInt();
    if (!jsonobject["idGroup"].isNull())
        idGroup = jsonobject["idGroup"].toString();
    if (!jsonobject["idParent"].isNull()) {
        idParent = jsonobject["idParent"].toString();
        if (ids.size() && idParent == ids.at(0))
            idParent.clear();
    }
    if (!jsonobject["idValue"].isNull())
        idValue = jsonobject["idValue"].toString();
    if (!jsonobject["idMain"].isNull())
        idMain = jsonobject["idMain"].toString();
    if (!jsonobject["code"].isNull())
        code = jsonobject["code"].toString();
    if (!jsonobject["name"].isNull())
        name = jsonobject["name"].toString();
    if (!jsonobject["nameGroup"].isNull())
        nameGroup = jsonobject["nameGroup"].toString();
    if (!jsonobject["pathGroup"].isNull())
        pathGroup = jsonobject["pathGroup"].toString();
    if (!jsonobject["imageFile"].isNull())
        imageFile = jsonobject["imageFile"].toString();
    if (!jsonobject["imageAlt"].isNull())
        imageAlt = jsonobject["imageAlt"].toString();
    if (!jsonobject["isActive"].isNull())
        isActive = jsonobject["isActive"].toBool();
    if (!jsonobject["value"].isNull())
        value = jsonobject["value"].toString();
    if (!jsonobject["isMain"].isNull())
        isMain = jsonobject["isMain"].toBool();
    if (!jsonobject["color"].isNull())
        colorHexCode = jsonobject["color"].toString();
    if (!jsonobject["valueSize"].isNull())
        valueSize = jsonobject["valueSize"].toInt();
    if (!jsonobject["valueType"].isNull())
        valueType = jsonobject["valueType"].toString();
    if (!jsonobject["url"].isNull())
        url = jsonobject["url"].toString();
    if (!jsonobject["isChecked"].isNull())
        isChecked = jsonobject["isChecked"].toBool();
    if (!jsonobject["idManager"].isNull())
        mIdManager  = jsonobject["idManager"].toString();
    if (!jsonobject["manager"].isNull())
        mManager = jsonobject["manager"].toString();

    if (!jsonobject["idsGroups"].isNull()) {
        QJsonArray idgroups = jsonobject["idsGroups"].toArray();
           foreach (const QJsonValue& value, idgroups)
               idsGroups->append(value.toString());
    }
    if (!jsonobject["idsChilds"].isNull()) {
        QJsonArray idsChilds = jsonobject["idsChilds"].toArray();
           foreach (const QJsonValue& value, idsChilds)
               idsChilds.append(value.toString());
    }
    if (!jsonobject["listValues"].isNull()) {
        if (jsonobject["listValues"].isString()) {
            QString values = jsonobject["listValues"].toString();
            listValues = values.split(",");
        }
        if (jsonobject["listValues"].isArray()) {
            QJsonArray values = jsonobject["listValues"].toArray();
               foreach (const QJsonValue& value, values)
                   listValues.append(value.toString());
        }
    }

    int count = metaObject()->propertyCount();
    for (int i = 0; i < jsonobject.keys().size(); ++i)
        if (!jsonobject[jsonobject.keys().at(i)].isNull()) {
            bool isExist = false;
             for (int j = 0; j < count; ++j) {
                 isExist = QString(metaObject()->property(j).name()) == jsonobject.keys().at(i).toLatin1();
                 if (isExist)
                     break;
             }
             if (isExist)
                 continue;
            QObject::setProperty(jsonobject.keys().at(i).toLatin1(), jsonobject[jsonobject.keys().at(i)].toVariant());
        }
}

QJsonObject DataItem::getAsJsonObject() const
{
    QJsonObject obj;   

    if (!ids.isEmpty()) {
        obj.insert("id", QJsonValue(ids.at(0)));
        QJsonArray items;
        for (int i = 0; i < ids.size(); i++)
            items.append(QJsonValue(ids.at(i)));
        obj.insert("ids", QJsonValue(items));
    }    

    if (isCloneMode)
        obj.insert("isCloneMode", QJsonValue(true));    
    if (!idValue.isEmpty())
        obj.insert("idValue", QJsonValue(idValue));
    if (isSetAllFields || modifiedFields.contains("idParent"))
        obj.insert("idParent", QJsonValue(idParent));
    if (isSetAllFields || modifiedFields.contains("idMain"))
        obj.insert("idMain", QJsonValue(idMain));
    if (isSetAllFields || modifiedFields.contains("isMain"))
        obj.insert("isMain", QJsonValue(isMain));
    if (isSetAllFields || modifiedFields.contains("sortIndex"))
        obj.insert("sortIndex", QJsonValue(sortIndex));
    if (isSetAllFields || modifiedFields.contains("code"))
        obj.insert("code", QJsonValue(code));
    if (isSetAllFields || modifiedFields.contains("name"))
        obj.insert("name", QJsonValue(name));
    if (isSetAllFields || modifiedFields.contains("imageFile"))
        obj.insert("imageFile", QJsonValue(imageFile));
    if (isSetAllFields || modifiedFields.contains("imageAlt"))
        obj.insert("imageAlt", QJsonValue(imageAlt));
    if (isSetAllFields || modifiedFields.contains("value"))
        obj.insert("value", QJsonValue(value));
    if (isSetAllFields || modifiedFields.contains("valueType"))
        obj.insert("valueType", QJsonValue(valueType));
    if (isSetAllFields || modifiedFields.contains("isActive"))
        obj.insert("isActive", QJsonValue(isActive));
    if (isSetAllFields || modifiedFields.contains("idGroup"))
        obj.insert("idGroup", QJsonValue(idGroup));
    if (isSetAllFields || modifiedFields.contains("url"))
        obj.insert("url", QJsonValue(url));    
    if (isSetAllFields || modifiedFields.contains("idManager"))
        obj.insert("idManager", QJsonValue(mIdManager));
    obj.insert("isChecked", QJsonValue(isChecked));

    if (isSetAllFields || modifiedFields.contains("idsGroups")) {
        QJsonArray items;
        for (int i = 0; i < idsGroups->size(); i++)
            items.append(QJsonValue(idsGroups->at(i)));
        obj.insert("idsGroups", QJsonValue(items));
    }    
    if (isSetAllFields || modifiedFields.contains("listValues")) {
        QJsonArray items;
        for (int i = 0; i < listValues.size(); i++)
            items.append(QJsonValue(listValues.at(i)));
        obj.insert("listValues", QJsonValue(items));
        obj.insert("values", listValues.join(","));
    }

    for(int i = 0; i < dynamicPropertyNames().size(); ++i)
    {
       if (isSetAllFields || modifiedFields.contains(dynamicPropertyNames().at(i))) {
           if (!obj.keys().contains(dynamicPropertyNames().at(i))) {
               if (property(dynamicPropertyNames().at(i)).type() == QVariant::String) {
                   obj.insert(dynamicPropertyNames().at(i),
                              QJsonValue(property(dynamicPropertyNames().at(i)).toString()));
                   continue;
               }
               if (property(dynamicPropertyNames().at(i)).type() == QVariant::Bool) {
                   obj.insert(dynamicPropertyNames().at(i),
                              QJsonValue(property(dynamicPropertyNames().at(i)).toBool()));
                   continue;
               }
               if (property(dynamicPropertyNames().at(i)).type() == QVariant::Double) {
                   obj.insert(dynamicPropertyNames().at(i),
                              QJsonValue(property(dynamicPropertyNames().at(i)).toDouble()));
                   continue;
               }
               if (property(dynamicPropertyNames().at(i)).type() == QVariant::Int) {
                   obj.insert(dynamicPropertyNames().at(i),
                              QJsonValue(property(dynamicPropertyNames().at(i)).toInt()));
                   continue;
               }
               if (property(dynamicPropertyNames().at(i)).type() == QVariant::Date) {
                   obj.insert(dynamicPropertyNames().at(i),
                              QJsonValue(property(dynamicPropertyNames().at(i)).toDate().toString("yyyy-MM-dd")));
                   continue;
               }
               obj.insert(dynamicPropertyNames().at(i),
                          QJsonValue::fromVariant(property(dynamicPropertyNames().at(i))));
           }
       }
    }

    return obj;
}

QString DataItem::getAsJsonString() const
{
    QJsonDocument doc;
    doc.setObject(getAsJsonObject());
    return QString(doc.toJson(QJsonDocument::Compact));
}

const QString DataItem::getId() const
{
    if (ids.size())
        return ids.at(0);
    return QString();
}

const QStringList DataItem::getIds() const
{
    return ids;
}

const QString DataItem::getIdMain() const
{
    return idMain;
}

const QString DataItem::getIdParent() const
{
    return idParent;
}

const QString DataItem::getIdGroup() const
{
    return idGroup;
}

const QString DataItem::getIdValue() const
{
    return idValue;
}

const QString DataItem::idManager() const
{
    return mIdManager;
}

const QString DataItem::manager() const
{
    return mManager;
}

int DataItem::getSortIndex() const
{    
    return sortIndex;
}

int DataItem::getCountChilds() const
{
    return countChilds;
}

const QString DataItem::getCode() const
{
    return code;
}

const QString DataItem::getName() const
{
    return name;
}

const QString DataItem::getNameGroup() const
{
    return nameGroup;
}

const QString DataItem::getPathGroup() const
{
    return pathGroup;
}

const QString DataItem::getImageFile() const
{
    return imageFile;
}

const QString DataItem::getImageAlt() const
{
    return imageAlt;
}

const QString DataItem::getUrl() const
{
    return url;
}

const QString DataItem::getValue() const
{
    return value;
}

QPixmap *DataItem::getPixmap() const
{
    return pixmap;
}

const QString DataItem::getValueSecret() const
{
    if (getCode().toUpper().indexOf("PASS") >= 0 ||
            getCode().toUpper().indexOf("KEY") >= 0 ||
            getCode().toUpper().indexOf("SECRET") >= 0 ||
            getCode().toUpper().indexOf("SIGN") >= 0)
        return "************";

    return getValue();
}

int DataItem::getValueSize() const
{
    return valueSize;
}

QString DataItem::getValueType() const
{
    return valueType;
}

const QStringList &DataItem::getValues() const
{
    return listValues;
}

const QMap<QString, DataItem*> &DataItem::getMapDataItems() const
{
    return mapDataItems;
}

void DataItem::addInListValues(const QString &item)
{
    listValues.append(item);
    setModified("listValues");
}

void DataItem::addInMapDataItems(const QString &id, DataItem *item)
{
    mapDataItems.insert(id, item);
}

bool DataItem::getIsFilledChilds() const
{
    return isFilledChilds;
}

const QStringList &DataItem::getIdsChilds() const
{
    return idsChilds;
}

void DataItem::setIsValueEditableMode()
{
    isValueEditableMode = true;
}

bool DataItem::setProperty(const char *name, const QVariant &value)
{
    setModified(QString(name));
    return QObject::setProperty(name, value);

}

const QString DataItem::getColorHexCode() const
{
    return colorHexCode;
}

int DataItem::row()
{
    if (container && parent()) {
        DataItem *parentItem = qobject_cast <DataItem *> (parent());
        if (!parentItem)
            return 0;
        else return parentItem->children().indexOf(this);
    }
    return 0;
}

const QVector<QString> *DataItem::getIdsGroups() const
{
    return idsGroups;
}

const QString DataItem::display() const
{
    return name;
}

bool DataItem::isModified() const
{
    return modifiedFields.size();
}

const QVariant DataItem::getPropertyDisplay(const QString &name) const
{
    return property(name.toUtf8().data());
}

const QVariant DataItem::getPropertyDecoration(const QString &) const
{
    return QPixmap();
}

bool DataItem::save()
{
    return false;
}

bool DataItem::getIsActive() const
{
    return isActive;
}

bool DataItem::getIsChecked() const
{
    return isChecked;
}

bool DataItem::getIsSelected() const
{
    return isSelected;
}

bool DataItem::getIsMain() const
{
    return isMain;
}

const QString DataItem::isActiveTitle() const
{
    if (isActive)
        return tr("Да");
    return tr("Нет");
}

const QPixmap DataItem::isActiveIco() const
{    
    if (isActive)
        return pixmapIsActive;
    return QPixmap();
}

const QString DataItem::sortIndexTitle() const
{
    if (getId().isEmpty())
        return QString();
    return QString::number(sortIndex);
}

const QString DataItem::valueTypeTitle() const
{
    if (valueType == "S" || valueType == "string")
        return tr("Строка");
    if (valueType == "P" || valueType == "password")
        return tr("Пароль");
    if (valueType == "T" || valueType == "text")
        return tr("Текст");
    if (valueType == "D" || valueType == "number")
        return tr("Число");
    if (valueType == "B" || valueType == "boolean")
        return tr("Логический");
    if (valueType == "CB" || valueType == "checkbox")
        return tr("Флажок");
    if (valueType == "R" || valueType == "radio")
        return tr("Переключатель");
    if (valueType == "L" || valueType == "select" || valueType == "list")
        return tr("Список");
    if (valueType == "DT" || valueType == "date")
        return tr("Дата");
    return QString();
}

const QString DataItem::valuesTitle() const
{
    QString result = QString();
    if (listValues.size())
        result = listValues.join(',');
    return result;
}

const QColor DataItem::getColorByPropertyName(const QString &) const
{
    return QColor();
}

QVariant DataItem::getPropertyEdit(const QString &propertyName) const
{
    if (propertyName == "name")
        return name;
    if (propertyName == "code")
        return code;

    return property(propertyName.toLatin1());
}

void DataItem::fillFromAllSimpleValues(const DataItem *item)
{
    copy(item);
}

bool DataItem::getIsEditableByPropertyName(const QString &propertyName)
{
    if (!isValueEditableMode) {
        if (propertyName == "name")
            return true;
        if (propertyName == "code")
            return true;
        if (propertyName == "note")
            return true;
    } else {
        if (propertyName == "value")
            return true;
    }

    return false;
}

bool DataItem::setEditDataByPropertyName(const QString &propertyName, const QVariant &data)
{
    if (!isValueEditableMode) {
        if (propertyName == "name") {
            setName(data.toString());
            return true;
        }
        if (propertyName == "code") {
            setCode(data.toString());
            return true;
        }
        if (propertyName == "note") {
            setProperty("note", data);
            return true;
        }
    } else {
        if (propertyName == "value") {
            setValue(data.toString());
            return true;
        }
    }
    return false;
}

void DataItem::clearData()
{
    ids.clear();
    idMain = QString();
    idParent = QString();
    idGroup = QString();
    sortIndex = 0;
    isChecked = false;
    isActive = false;
    code = QString();
    name = QString();
    nameGroup = QString();
    imageFile = QString();
    imageAlt = QString();    
    value = QString();
    url = QString();
    idsGroups->clear();
    idsChilds.clear();
    isSelected = false;
    isMain = false;
    isFilledChilds = false;
    *pixmap = QPixmap();
    listValues.clear();
    mapDataItems.clear();

    for(int i = 0; i < dynamicPropertyNames().size(); ++i)
        QObject::setProperty(dynamicPropertyNames().at(i), QVariant());
}

void DataItem::reset()
{
    QStringList ids;
    for (int i = 0; i < ids.size(); i++)
        ids.append(ids.at(i));
    clearData();
    for (int i = 0; i < ids.size(); i++)
        ids.append(ids.at(i));
    setUnModified();
}

void DataItem::setModified(const QString &fieldName)
{
    if (!modifiedFields.contains(fieldName))
        modifiedFields.append(fieldName);
    emit modified();
}

void DataItem::setIsSetAllFields(const bool &isSetAllFields)
{
    this->isSetAllFields = isSetAllFields;
}

void DataItem::setUnModified()
{
    modifiedFields.clear();
}

void DataItem::setIsFilledChilds(const bool &isFilledChilds)
{
    this->isFilledChilds = isFilledChilds;
}





