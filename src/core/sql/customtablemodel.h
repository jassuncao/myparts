#ifndef CUSTOMTABLEMODEL_H
#define CUSTOMTABLEMODEL_H

#include <QAbstractTableModel>
#include <QAbstractListModel>
#include <QList>
#include <QVector>
#include <QHash>
#include <QMap>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStyledItemDelegate>
#include <QComboBox>
#include "core/sql/modelwithforeignkey.h"

class TableItem {
public:
    explicit TableItem(QVector<QVariant> data, const QVariant & id = QVariant(), bool dirty=true);
    void setData(int column, const QVariant & value);
    QVariant data(int column) const;

    inline bool dirty() const { return _dirty; }
    void resetDirty() { _dirty = false; }

    inline QVariant id() const {return _id;}
    void setId(QVariant id);
private:
    QVector<QVariant> _data;
    QVariant _id;
    bool _dirty;
};

class ModelRelation {
public:
    virtual ~ModelRelation() {}
    virtual QVariant displayValue(const QVariant foreignId) const = 0;
    virtual bool validId(const QVariant foreignId) const = 0;
    virtual bool initialized() const = 0;
    virtual void populateDictionary() = 0;
    virtual QAbstractListModel * model() = 0;
};

class TableRelation : public QAbstractListModel, public ModelRelation {
public:
    explicit TableRelation(const QString & tableName, const QString & indexColumn, const QString & displayColumn, const bool nullable=false);
    ~TableRelation();
    QVariant displayValue(const QVariant foreignId) const;
    bool validId(const QVariant foreignId) const;
    bool initialized() const { return _initialized; }
    QAbstractListModel * model();    
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    void populateDictionary();
private:    
    bool _initialized;
    QHash<int, QVariant> _lookupDictionary;
    QList<QPair<QString,int> > _items;
    QSqlQuery _selectQuery;
    bool _nullable;
};


class CustomTableModel : public QAbstractTableModel, public IModelWithForeignKey
{
    Q_OBJECT
public:
    explicit CustomTableModel(int maxColumns, QObject *parent = 0);
    virtual ~CustomTableModel();
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    bool insertRows(int row, int count, const QModelIndex&);
    bool removeRows(int row, int count, const QModelIndex&);    
    void select();
    bool submitAll();    
    virtual QAbstractItemModel *relationModel(const int column) const;
    void cloneData();
protected:
    virtual TableItem * createBlankItem() const;
    virtual bool deleteItem(QVariant id) = 0;
    virtual bool saveItem(TableItem* item) = 0;
    virtual bool loadItems(QList<TableItem*> & dest) = 0;
    virtual bool doInsertRow(int row, TableItem* item);
    virtual void registerRelation(const int column, ModelRelation * relation);
    virtual QVariant getItemId(int row) const;
    bool columnIsRelation(const int column) const;

    const int _maxColumns;
    QList<TableItem*> _items;
    QList<QVariant> _deletedItems;
    QVector<QHash<int, QVariant> > _headers;
    QVector<ModelRelation*> _relations;
};

class SimpleSqlTableModel : public CustomTableModel
{
    Q_OBJECT
public:
    explicit SimpleSqlTableModel(const QString &tableName, const QStringList & fieldNames,
                                 const QStringList & columnNames, const QString & foreignKeyField = QString(), QObject *parent = 0, QSqlDatabase db = QSqlDatabase());
    void setCurrentForeignKey(const QVariant & foreignKey);
    inline QVariant currentForeignKey() const  {return _foreignKey;}
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);
protected:
    virtual bool deleteItem(QVariant id);
    virtual bool saveItem(TableItem* item) ;
    virtual bool loadItems(QList<TableItem *> &dest);
    QString generateSelectStatement(const QString & tableName,  const QStringList & fieldNames, const QString & foreignKeyField, int sortColumn, Qt::SortOrder order);
    QSqlDatabase database() const { return _database;}
    QSqlDatabase _database;
    const QString _tableName;
    const QStringList _fieldNames;    
    const QString _foreignKeyField;
    QVariant _foreignKey;
    QSqlQuery _selectQuery;
    QSqlQuery _insertQuery;
    QSqlQuery _updateQuery;
    QSqlQuery _deleteQuery;
    int _sortColumn;
    Qt::SortOrder _order;
};


#endif // CUSTOMTABLEMODEL_H
