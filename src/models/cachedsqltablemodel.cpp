#include "cachedsqltablemodel.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>

typedef QPair<QVariant,QVariant> RelationPair;
Q_DECLARE_METATYPE(RelationPair)

/*
class QueryHelper {

public:
    void addField(const QString & fieldName){
        _fields.append(fieldName);
    }

    void addField(const QString & fieldName, const QString & tableName)
    {
        QString aliasedField = aliasField(fieldName, tableName);
        _fields.append(aliasedField);
    }

    QString aliasField(const QString &field, const QString & tableName)
    {
        TableJoin join;
        foreach (join, _tableJoins) {
            if(join.table == tableName){
                return join.prefix+"."+field;
            }
        }
        qWarning()<<"TableJoin for table "<<tableName<<" not found";
        return field;
    }

private:
    QVector<TableJoin> _tableJoins;
    QList<QString> _fields;
};
*/

class Column;
class RelationColumn;




class SelectQueryBuilder : public ColumnVisitor {

public:

    explicit SelectQueryBuilder(const QString & table, const QVector<TableJoin> & tableJoins) :
        _table(table), _tableJoins(tableJoins),
        _orderIndex(-1), _orderDir(Qt::AscendingOrder)
    {
    }

    virtual void visit(Column & column)
    {
        _fields.append(column.fieldName());
    }

    virtual void visit(RelationColumn & column)
    {
        _fields.append(column.fieldName());
        _fields.append(aliasField(column.relatedTable(), column.relatedFieldName()));
    }

    void setParentKey(const QString & fieldName/*, const QVariant & value*/)
    {
        _parentFieldName = fieldName;
        //_parentKeyValue = value;
    }

    void orderByColumn(Column * col, Qt::SortOrder order)
    {
        _orderIndex = col->displayIndex();
        _orderDir = order;
    }

    QString build()
    {
        QString fieldsExp = _fields.join(", ");
        QString joinExp;
        QString whereExp;
        QString orderDir = _orderDir == Qt::DescendingOrder ? "DESC" : "ASC";
        QString orderByExp = _orderIndex < 1 ? QString() : QString("ORDER BY %1 %2").arg(_orderIndex).arg(orderDir);
        QStringList joinsList;
        for(int i = 0; i < _tableJoins.count(); ++i){
            const TableJoin & join = _tableJoins.at(i);
            QString joinType = join.joinMode == TableJoin::LeftJoin ? "LEFT JOIN" :"INNER JOIN";
            QString tableAlias = aliasTable(join.table);
            QString refFieldAlias = aliasField(join.table, join.referencedField);
            QString exp = QString("%1 %2 AS %3 ON %4 = %5").arg(joinType, join.table, tableAlias, join.joinField, refFieldAlias);
            joinsList.append(exp);
        }
        joinExp = joinsList.join("\n");
        if(!_parentFieldName.isEmpty()){
            whereExp = QString("WHERE %1 = ?").arg(_parentFieldName);
        }

        QString selectTmpl("SELECT %1 FROM %2 %3 %4 %5");//fields, table, joins, where, order
        QString select = selectTmpl.arg(fieldsExp, _table, joinExp, whereExp, orderByExp);
        return select;
    }

protected:

    QString aliasTable(const QString & tableName) const
    {
        for(int i = 0; i < _tableJoins.count(); ++i){
            const TableJoin & join = _tableJoins.at(i);
            if(join.table == tableName){
                return QString("t%1").arg(i);
            }
        }
        qWarning()<<"TableJoin for table "<<tableName<<" not found";
        return tableName;
    }

    QString aliasField(const QString & tableName, const QString &field) const
    {
        const QString & alias = aliasTable(tableName);
        return QString("%1.%2").arg(alias).arg(field);
    }

private:
    const QString _table;
    const QVector<TableJoin> _tableJoins;
    QStringList _fields;
    int _orderIndex;
    Qt::SortOrder _orderDir;
    QString _parentFieldName;
   // QVariant _parentKeyValue;
};

/*
class RelationColumn : public Column {
public:
    explicit RelationColumn(const QString & columnName, const QString & headerName, const QString & relatedTable, const QString displayColumn) :
        Column(columnName, headerName),
        _relatedTable(relatedTable),
        _displayColumn(displayColumn)
    {}

    void collectFields(QueryHelper * helper){
        Column::collectFields(helper);
        helper->addField(_displayColumn, _relatedTable);
    }

    QString orderByField(QueryHelper * helper){
        return helper->aliasField(_displayColumn, _relatedTable);
    }

private:
    const QString _relatedTable;
    const QString _displayColumn;
};
*/

TableRow::TableRow(QVector<QVariant> data, int flags) :
    _data(data),
    _flags(flags)
{
}

void TableRow::setData(int fieldIndex, const QVariant & value){
    if(fieldIndex<_data.size()){
        _data[fieldIndex] = value;
        _flags = _flags | Dirty;
    }
}

QVariant TableRow::data(int fieldIndex) const {
    if(fieldIndex<_data.size()){
        return _data.at(fieldIndex);
    }
    return QVariant();
}

CachedSqlTableModel::CachedSqlTableModel(const QString &tableName, QObject *parent)
    :QAbstractTableModel(parent), _tableName(tableName)
{
}

CachedSqlTableModel::~CachedSqlTableModel()
{
    qDeleteAll(_cachedRows);
    //qDeleteAll(_columns);
}

/*
void CachedSqlTableModel::addColumn(const QString & columnName, const QString & header)
{
    int section = _columns.count();
    _columns.append(new Column(columnName, header));
    setHeaderData(section, Qt::Horizontal, header);
}
*/

void CachedSqlTableModel::mapColumn(QVariant::Type type, int displayField, int editField)
{
    _columnsMapping.append(ColumnMapping(type, displayField, editField));
}

void CachedSqlTableModel::registerColumn(const QString & header, const QString & fieldName, bool idColumn)
{
//    int section = _columns.count();
//    _columns.append(new Column(fieldName, idColumn));
//    setHeaderData(section, Qt::Horizontal, header);
}

void CachedSqlTableModel::registerColumn(const QString & header, const QString & fieldName, const QString & relatedTable, const QString & relatedFieldName)
{
//    int section = _columns.count();
//    _columns.append(new RelationColumn(fieldName, section, relatedFieldName, section+1, relatedTable));
//    setHeaderData(section, Qt::Horizontal, header);
}

void CachedSqlTableModel::registerJoin(const TableJoin & join)
{
    _tableJoins.append(join);
}

QVariant CachedSqlTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal) {
        QVariant val = _headers.value(section).value(role);
        if (role == Qt::DisplayRole && !val.isValid())
            val = _headers.value(section).value(Qt::EditRole);
        if (val.isValid())
            return val;
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}

bool CachedSqlTableModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (orientation != Qt::Horizontal || section < 0 || columnCount() <= section)
        return false;

    if (_headers.size() <= section)
        _headers.resize(qMax(section + 1, 16));
    _headers[section][role] = value;
    emit headerDataChanged(orientation, section, section);
    return true;
}

int CachedSqlTableModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : _cachedRows.count();
}

int CachedSqlTableModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : _columnsMapping.count();
}

Qt::ItemFlags CachedSqlTableModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags theFlags = QAbstractTableModel::flags(index);
    if (index.isValid()){
        theFlags |= Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
    }
    return theFlags;
}

//QString CachedSqlTableModel::generateSelectStatement(int sortColumn, Qt::SortOrder order)
//{
//    SelectQueryBuilder builder(_tableName, _tableJoins);
//    if(sortColumn >= 0 && sortColumn < _columns.size()) {
//        builder.orderByColumn(_columns.at(sortColumn), order);
//    }
//    return builder.build();
//}

bool CachedSqlTableModel::insertRows(int row, int count, const QModelIndex&)
{
    beginInsertRows(QModelIndex(), row, row + count - 1);
    for (int i = 0; i < count; ++i) {
        _cachedRows.insert(row, createBlankItem());
    }
    endInsertRows();
    return true;
}

bool CachedSqlTableModel::removeRows(int row, int count, const QModelIndex&)
{
    beginRemoveRows(QModelIndex(), row, row + count - 1);
    for (int i = 0; i < count; ++i) {

        TableRow * tableRow = _cachedRows.takeAt(row);
        _deletedRows.append(tableRow);
    }
    endRemoveRows();
    return true;
}

void CachedSqlTableModel::select()
{
    beginResetModel();
    qDeleteAll(_cachedRows);
    _cachedRows.clear();
    loadItems(_cachedRows);
    endResetModel();
}

bool CachedSqlTableModel::submitAll()
{
    if(!_deletedRows.isEmpty()){
        while (!_deletedRows.isEmpty()){            
            const TableRow * row = _deletedRows.at(0);
            bool res = deleteRow(row);
            if(res){
                delete _deletedRows.takeFirst();
            }
        }
    }

    for(int i=0; i<_cachedRows.count(); ++i){
        TableRow * row = _cachedRows[i];
        if(row->dirty()){
            bool res;
            if(row->attached()){
                res = updateRow(row);                
            }
            else{
                res = saveRow(row);
                row->setAttached();
            }
            if(res){
                row->resetDirty();
            }
            else{
                return res;
            }
        }
    }
    return true;
}

QVariant CachedSqlTableModel::data(const QModelIndex &index, int role) const
{
    const int col = index.column();
    if (!index.isValid() ||
        index.row() < 0 || index.row() >= _cachedRows.count() ||
        col < 0 || col >= _columnsMapping.count()) {
        return QVariant();
    }

    QVariant v;
    if (role & ~(Qt::DisplayRole | Qt::EditRole)){
        return v;
    }

    if (role == Qt::DisplayRole){
        const TableRow * row = _cachedRows.at(index.row());
        int fieldIndex = _columnsMapping.at(col).displayField;
        return row->data(fieldIndex);
    }
    else {
        const TableRow * row = _cachedRows.at(index.row());
        int fieldIndex = _columnsMapping.at(col).editField;
        return row->data(fieldIndex);
    }    
}

bool CachedSqlTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    const int col = index.column();
    const int row = index.row();

    if (!index.isValid() ||
        row < 0 || row >= _cachedRows.count() ||
        col < 0 || col >= _columnsMapping.count()){
        return false;
    }

    int fieldIndex = -1;

    if(role == Qt::DisplayRole){
        fieldIndex = _columnsMapping.at(col).displayField;
    }
    else if(role == Qt::EditRole){
        fieldIndex = _columnsMapping.at(col).editField;
    }
    if(fieldIndex >= 0){        
        qDebug()<<"Set data for field "<<fieldIndex;
        TableRow * tableRow = _cachedRows.at(row);
        tableRow->setData(fieldIndex, value);
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

/*
TableRow * CachedSqlTableModel::createBlankItem() const
{
    return new TableRow(QVector<QVariant>(_maxColumns));
}
*/


bool CachedSqlTableModel::loadItems(QList<TableRow*> & dest)
{
    QSqlQuery selectQuery = prepareSelectQuery();
    /*
    if(!_foreignKeyField.isEmpty()){
        if(!_foreignKey.isValid())
            return false;
        qDebug()<<"loading items for "<<_foreignKey;
        _selectQuery.bindValue(0, _foreignKey);
    }
    */

    if(selectQuery.exec()){
        int fieldsCount = selectQuery.record().count();
        while(selectQuery.next())
        {
            QVector<QVariant> rowData(fieldsCount);
            for(int i=0; i<fieldsCount; ++i){
                rowData[i] = selectQuery.value(i);
            }
            TableRow * row = new TableRow(rowData, TableRow::Attached);
            dest.append(row);
        }
        return true;
    }
    qWarning()<<"Failed to execute query. Reason "<<selectQuery.lastError().driverText();
    return false;
}
