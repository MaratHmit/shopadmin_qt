#ifndef DATAITEM_H
#define DATAITEM_H

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QVector>
#include <QPixmap>
#include <QDebug>
#include <QMap>

class DataItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString id READ getId WRITE setId)
    Q_PROPERTY(QString name READ getName WRITE setName)
    Q_PROPERTY(QString nameGroup READ getNameGroup WRITE setNameGroup)
    Q_PROPERTY(QString pathGroup READ getPathGroup)
    Q_PROPERTY(QString display READ display)
    Q_PROPERTY(QString code READ getCode WRITE setCode)
    Q_PROPERTY(QString value READ getValue WRITE setValue)
    Q_PROPERTY(QString valueSecret READ getValueSecret)
    Q_PROPERTY(bool isMain READ getIsMain WRITE setIsMain)
    Q_PROPERTY(QString sortIndex READ sortIndexTitle)
    Q_PROPERTY(QString isActive READ isActiveTitle)
    Q_PROPERTY(QPixmap isActiveIco READ isActiveIco)
    Q_PROPERTY(QString getValueType READ valueTypeTitle)
    Q_PROPERTY(QString values READ valuesTitle)
    Q_PROPERTY(QString manager READ manager)

public:
    DataItem();
    DataItem(const DataItem *d);
    DataItem(const QJsonObject &jsonobject);
    virtual ~DataItem();

    virtual void copy(const DataItem *d);
    virtual void setFromJSONObject(const QJsonObject &jsonobject);
    virtual QJsonObject getAsJsonObject() const;
    virtual QString getAsJsonString() const;
    virtual void clearData();

    void addId(const QString &getId);
    void addIdGroup(const QString &getId);
    void delIdGroup(const QString &getId);
    void clearGroups();

    const QString getId() const;
    const QStringList getIds() const;
    const QString getIdMain() const;
    const QString getIdParent() const;
    const QString getIdGroup() const;
    const QString getIdValue() const;
    const QString idManager() const;
    const QString manager() const;
    int getSortIndex() const;
    int getCountChilds() const;
    const QString getCode() const;
    const QString getName() const;    
    const QString getNameGroup() const;
    const QString getPathGroup() const;
    const QString getImageFile() const;
    const QString getImageAlt() const;    
    const QString getUrl() const;
    virtual const QString getValue() const;
    QPixmap *getPixmap() const;
    const QString getValueSecret() const;
    int getValueSize() const;
    QString getValueType() const;
    const QStringList &getValues() const;
    const QMap<QString, DataItem*> &getMapDataItems() const;
    void addInListValues(const QString &item);
    void addInMapDataItems(const QString &getId, DataItem *item);
    bool getIsFilledChilds() const;
    const QStringList &getIdsChilds() const;
    void setIsValueEditableMode();

    bool setProperty(const char *getName, const QVariant &getValue);

    const QString getColorHexCode() const;
    int row();
    const QVector<QString> *getIdsGroups() const;
    virtual const QString display() const;

    virtual bool isModified() const;
    virtual const QVariant getPropertyDisplay(const QString &) const;
    virtual const QVariant getPropertyDecoration(const QString &) const;    
    virtual QVariant getPropertyEdit(const QString &) const;
    virtual void fillFromAllSimpleValues(const DataItem *item);

    bool save();

    bool getIsActive() const;
    bool getIsChecked() const;
    bool getIsSelected() const;
    bool getIsMain() const;

    const QString isActiveTitle() const;
    const QPixmap isActiveIco() const;
    const QString sortIndexTitle() const;
    const QString valueTypeTitle() const;
    const QString valuesTitle() const;

    virtual const QColor getColorByPropertyName(const QString &propertyName) const;    
    virtual bool getIsEditableByPropertyName(const QString &propertyName);
    virtual bool setEditDataByPropertyName(const QString &propertyName, const QVariant &data);    

    void reset();
    void setModified(const QString &fieldName);
    void setIsSetAllFields(const bool &isSetAllFields);
    virtual void setUnModified();

    void setIsFilledChilds(const bool &isFilledChilds);
    void moveChildrens(int from, int to);

public slots:
    void setId(const QString &id);
    void setIdMain(const QString &idMain);
    void setIdParent(const QString &idParent);
    void setIdGroup(const QString &idGroup);
    void setSortIndex(const int &sortIndex);
    void setCode(const QString &code);
    void setName(const QString &name);
    void setNameGroup(const QString &nameGroup);
    void setNote(const QString &note);
    void setImageFile(const QString &imageFile);
    void setImageAlt(const QString &imageAlt);    
    void setDescription(const QString &description);
    void setFullDescription(const QString &fullDescription);
    void setSEOHeader(const QString &seoHeader);
    void setSEOKeywords(const QString &seoKeywords);
    void setSEODescription(const QString &seoDescription);
    void setPageTitle(const QString &pageTitle);
    void setIsActive(const bool &isActive);
    void setIsChecked(const bool &isChecked);
    void setIsCheckedChilds(const bool &isChecked);
    void setValue(const QString &value);
    void setIsSelected(const bool &isSelected);
    void setIsMain(const bool &isMain);    
    void setContainer(void *container);    
    void setCloneMode(const bool &isCloneMode);    
    void setValueType(const QString &getValueType);
    void setIdValue(const QString &idValue);
    void setListValues(const QString &values);
    void setUrl(const QString &url);    
    void setIdManager(const QString &idManager);
    void setManager(const QString &manager);

signals:
    void modified();

protected:
    virtual void initialization();

    // список модифицированных полей объекта
    QStringList modifiedFields;
    bool isSetAllFields;

private:
    bool isValueEditableMode;

    QStringList ids;
    QString idMain;
    QString idParent;
    QString idGroup;
    QString idValue;
    int sortIndex;
    int countChilds;
    QString code;
    QString name;
    QString nameGroup;
    QString pathGroup;
    QString imageFile;
    QString imageAlt;    
    QString value;
    QString colorHexCode;
    QString url;
    QVector<QString> *idsGroups;
    QStringList idsChilds;
    QString mIdManager;
    QString mManager;
    bool isActive;
    bool isMain;
    bool isChecked;
    bool isSelected;
    bool isFilledChilds;
    int valueSize;
    QString valueType;
    QPixmap *pixmap;
    QPixmap pixmapIsActive;
    bool isCloneMode;
    QStringList listValues;
    QMap<QString, DataItem*> mapDataItems;

    // указатель на контейнер, если объект находиться в контейнере
    void *container;
};

Q_DECLARE_METATYPE (DataItem *)

template <class T>
class SEVector : private QVector<T>
{
public:
    SEVector() {
        mIsModified = false;
        mRoot = new DataItem();
        mRoot->setId("root");
        mAmount = 0;
        mIsSetAllFields = false;
    }

    ~SEVector()
    {
        delete mRoot;
    }

    void setCountInBase(const int &count)
     {
         mCountInBase = count;
     }

     int countInBase() const
     {
         return mCountInBase;
     }

     void setAmount(const qreal amount)
     {
         mAmount = amount;
     }

     qreal amount() const
     {
         return mAmount;
     }

     void deleteItems()
     {
         for (int i = 0; i < this->size(); i++)
             this->at(i)->setParent(0);

         while (!this->isEmpty())
             delete this->takeFirst();

         setUnModified();
     }

     void deleteItem(const T t) {
         if (!t)
             return;

         for (int i = t->children().size() - 1; i >= 0; --i)
                 deleteItem(reinterpret_cast <T> (t->children().at(i)));

         int i = this->indexOf(t);
         if (i >= 0) {
             DataItem *item = this->takeAt(i);
             delete item;
         }
         setModified();
     }

     bool contains(const T &t) const
     {
         return QVector<T>::contains(t);
     }

     bool existById(const T t) const
     {
         for (int i = 0; i < this->size(); i++)
             if (this->at(i)->getId() == t->getId())
                 return true;
         return false;
     }

     bool existById(const QString &id) const
     {
         for (int i = 0; i < this->size(); i++)
             if (this->at(i)->id() == id)
                 return true;
         return false;
     }

     bool existByName(const QString &name) const
     {
         for (int i = 0; i < this->size(); i++)
             if (this->at(i)->getName() == name)
                 return true;
         return false;
     }

     void setModified() {
         mIsModified = true;
     }

     void setUnModified() {
         mIsModified = false;
         for (int i = 0; i < this->size(); i++)
             this->at(i)->setUnModified();
     }

     bool isModified() const {
         bool result = mIsModified;
         for (int i = 0; i < this->size(); i++)
             result = result || this->at(i)->isModified();
         return result;
     }

     void append(const T &t, const bool &isSetSortIndex = 1) {
         QVector<T>::append(t);
         if (!t->parent())
            t->setParent(mRoot);
         t->setContainer(this);
         if (isSetSortIndex)
            t->setSortIndex(t->row());
         setModified();
     }

     void prepend(const T &t, const bool &isSetSortIndex = 1) {
         QVector<T>::prepend(t);
         if (!t->parent())
            t->setParent(mRoot);
         t->setContainer(this);
         if (isSetSortIndex)
            t->setSortIndex(0);

         DataItem *root = mRoot;
         root->moveChildrens(root->children().size() - 1, 0);

         setModified();
     }

     void clear() {
         QVector<T>::clear();
     }

     int size() const {
         return QVector<T>::size();
     }

     const T &at(int i) const {
        return QVector<T>::at(i);
     }

     const T &atRow(int i) const {
        for (int j = 0; j < this->size(); j++)
            if (this->at(j)->row() == i)
                return this->at(j);
        return QVector<T>::at(i);
     }

     T &operator[](int i) {
         return QVector<T>::operator [](i);
     }

     const T &operator[](int i) const {
         return QVector<T>::operator [](i);
     }

     bool empty() const {
         return QVector<T>::empty();
     }

     T takeAt(int i) {
         return QVector<T>::takeAt(i);
     }

     T takeLast() {
         return QVector<T>::takeLast();
     }

     T takeFirst() {
         return QVector<T>::takeFirst();
     }

     T value(int i) {
         return QVector<T>::value(i);
     }

     int indexOf(const T &t, int from = 0) const {
         return QVector<T>::indexOf(t, from);
     }

     void moveUp(DataItem *item) {
        int index = item->row();
        if (!index)
            return;

        DataItem *root = mRoot;
        if (item->parent() && item->parent() != root)
            root = dynamic_cast <DataItem *> (item->parent());

        item->setSortIndex(index - 1);
        dynamic_cast<DataItem *> (root->children().at(index - 1))->setSortIndex(index);
        root->moveChildrens(index, index - 1);
     }

     void moveDown(DataItem *item) {
        int index = item->row();
        if (index + 1 == mRoot->children().size())
            return;

        DataItem *root = mRoot;
        if (item->parent() && item->parent() != root)
            root = dynamic_cast <DataItem *> (item->parent());

        item->setSortIndex(index + 1);
        dynamic_cast<DataItem *> (root->children().at(index + 1))->setSortIndex(index);
        root->moveChildrens(index, index + 1);
     }

     QJsonArray getAsJsonArray() const {
         QJsonArray json;
         for (int i = 0; i < this->size(); i++) {
             this->at(i)->setIsSetAllFields(mIsSetAllFields);
             json.append(QJsonValue(this->at(i)->getAsJsonObject()));
         }

         return json;
     }

     const DataItem *root() const {
         return mRoot;
     }

     void setIsSetAllFields(const bool &isSetAllFields) {
         mIsSetAllFields = isSetAllFields;
     }

     void setRootParent(DataItem *item) {
         item->setParent(mRoot);
         item->setIdParent("root");
     }     

private:
    int mCountInBase;
    qreal mAmount;
    int mIsModified;
    DataItem *mRoot;
    bool mIsSetAllFields;
};

typedef SEVector<DataItem *> ListDataItems;


#endif // DATA_H
