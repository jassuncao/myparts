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
#include <QComboBox>


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

class Column;
class RelationColumn;

class ColumnVisitor {
public:
    virtual void visit(Column & column) = 0;
    virtual void visit(RelationColumn & column) = 0;
};

class Column {
public:
    Column(const QString & fieldName, bool idColumn) :
        _fieldName(fieldName),
        _fieldIndex(0),
        _idColumn(idColumn)
    {}

    virtual void accept(ColumnVisitor& visitor){
        visitor.visit(*this);
    }

    virtual int displayIndex() const {
        return _fieldIndex;
    }

    virtual int editIndex() const {
        return _fieldIndex;
    }

    virtual QString fieldName() const {
        return _fieldName;
    }

    virtual bool idColumn() const {
        return _idColumn;
    }

private:
    const QString _fieldName;
    const int _fieldIndex;
    const bool _idColumn;
};

class RelationColumn : public Column {
public:
    RelationColumn(const QString & fieldName, int fieldIndex, const QString & relatedFieldName, int relatedFieldIndex, const QString & relatedTable) :
        Column(fieldName, false),
        _relatedFieldName(relatedFieldName),
        _relatedFieldIndex(relatedFieldIndex),
        _relatedTable(relatedTable)
    {}

    virtual void accept(ColumnVisitor& visitor){
        visitor.visit(*this);
    }

    virtual int displayIndex() const {
        return _relatedFieldIndex;
    }

    virtual QString relatedFieldName() const {
        return _relatedFieldName;
    }

    virtual QString relatedTable() const {
        return _relatedTable;
    }

private:
    const QString _relatedFieldName;
    const int _relatedFieldIndex;
    const QString _relatedTable;

};

struct ColumnMapping {
    QVariant::Type type;
    int displayField;
    int editField;

    ColumnMapping() :
        type(QVariant::Invalid), displayField(-1), editField(-1)
    {}

    ColumnMapping(QVariant::Type aType, int aDisplayField, int aEditField) :
        type(aType), displayField(aDisplayField), editField(aEditField)
    {}

    ColumnMapping(const ColumnMapping& other) :
        type(other.type), displayField(other.displayField), editField(other.editField)
    {}
};

struct TableJoin {
    enum JoinMode {
        InnerJoin,
        LeftJoin
    };

    TableJoin(){}

    TableJoin(const QString & aJoinField, const QString & aTable, const QString & aReferencedField, const JoinMode aJoinMode) :
        joinField(aJoinField), table(aTable), referencedField(aReferencedField), joinMode(aJoinMode)
    {}

    QString joinField;
    QString table;
    QString referencedField;
    QString prefix;
    JoinMode joinMode;
};

class CachedSqlTableModel : public QAbstractTableModel
{
public:    
    explicit CachedSqlTableModel(const QString & tableName, QObject *parent = 0);
    ~CachedSqlTableModel();
    //void addColumn(const QString & columnName, const QString & header);
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
    void registerColumn(const QString &header, const QString &fieldName, bool idColumn = false);
    void registerColumn(const QString &header, const QString &fieldName, const QString &relatedTable, const QString &relatedFieldName);
    void registerJoin(const TableJoin & join);
    //QString generateSelectStatement(int sortColumn, Qt::SortOrder order);
    virtual QSqlQuery prepareSelectQuery() = 0;
    virtual bool loadItems(QList<TableRow*> & dest);
    virtual TableRow * createBlankItem() const = 0;
    virtual bool updateRow(TableRow * row) = 0;
    virtual bool saveRow(TableRow * row) = 0;
    virtual bool deleteRow(const TableRow * row) = 0;

    QVector<QHash<int, QVariant> > _headers;
    //QList<Column*> _columns;
    QVector<ColumnMapping> _columnsMapping;
    QVector<TableJoin> _tableJoins;
    QList<TableRow*> _cachedRows;
    QList<TableRow*> _deletedRows;
    const QString _tableName;

    QVariant _parentKey;
//    QSqlQuery _selectQuery;
//    QSqlQuery _insertQuery;
//    QSqlQuery _updateQuery;
//    QSqlQuery _deleteQuery;
};

#endif // CACHEDSQLTABLEMODEL_H
