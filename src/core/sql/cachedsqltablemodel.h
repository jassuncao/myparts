#ifndef CACHEDSQLTABLEMODEL_H
#define CACHEDSQLTABLEMODEL_H

#include <QAbstractTableModel>
#include <QAbstractListModel>
#include <QList>
#include <QVector>
#include <QHash>
#include <QMap>
#include <QSqlQuery>
#include <QStyledItemDelegate>


class TableRow {
public:
    enum StateFlags {
        /// Has pending changes
        Dirty = 1,
        /// Was persisted to the database (has id)
        Attached = 2
    };

    explicit TableRow(QVector<QVariant> data, int flags=0);

    void setData(int fieldIndex, const QVariant & value);
    QVariant data(int fieldIndex) const;

    inline bool dirty() const { return (_flags & Dirty) != 0 ;}
    void resetDirty() { _flags = _flags & ~Dirty; }
    inline bool attached() const { return (_flags & Attached) != 0 ;}
    void setAttached() { _flags = _flags | Attached; }
private:
    QVector<QVariant> _data;
    int _flags;
    bool _dirty;
};

struct ColumnMapping {
    QVariant::Type atype;
    int displayField;
    int editField;

    ColumnMapping() :
        atype(QVariant::Invalid), displayField(-1), editField(-1)
    {}

    ColumnMapping(QVariant::Type aType, int aDisplayField, int aEditField) :
        atype(aType), displayField(aDisplayField), editField(aEditField)
    {}

    ColumnMapping(const ColumnMapping& other) :
        atype(other.atype), displayField(other.displayField), editField(other.editField)
    {}
};

class CachedSqlTableModel : public QAbstractTableModel
{
public:    
    explicit CachedSqlTableModel(QObject *parent = 0);
    ~CachedSqlTableModel();    
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
protected:
    void mapColumn(QVariant::Type type, int displayField, int editField);
    virtual QSqlQuery prepareSelectQuery() = 0;
    virtual bool loadItems(QList<TableRow*> & dest);
    virtual TableRow * createBlankItem() const = 0;
    virtual bool updateRow(TableRow * row) = 0;
    virtual bool saveRow(TableRow * row) = 0;
    virtual bool deleteRow(const TableRow * row) = 0;    

    QVector<QHash<int, QVariant> > _headers;
    QVector<ColumnMapping> _columnsMapping;    
    QList<TableRow*> _cachedRows;
    QList<TableRow*> _deletedRows;    

    QVariant _parentKey;
};

#endif // CACHEDSQLTABLEMODEL_H
