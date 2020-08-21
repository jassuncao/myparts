#include "cachedsqltablemodel.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>

typedef QPair<QVariant,QVariant> RelationPair;
Q_DECLARE_METATYPE(RelationPair)



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

CachedSqlTableModel::CachedSqlTableModel(QObject *parent)
    :QAbstractTableModel(parent)
{
}

CachedSqlTableModel::~CachedSqlTableModel()
{    
    qDeleteAll(_cachedRows);
}

void CachedSqlTableModel::mapColumn(QVariant::Type type, int displayField, int editField)
{
    _columnsMapping.append(ColumnMapping(type, displayField, editField));
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
    while (!_deletedRows.isEmpty()){
        const TableRow * row = _deletedRows.at(0);
        bool res = deleteRow(row);
        if(res){
            delete _deletedRows.takeFirst();
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

    if (role & ~(Qt::DisplayRole | Qt::EditRole)){
        return QVariant();
    }

    const TableRow * row = _cachedRows.at(index.row());
    const ColumnMapping & colMapping = _columnsMapping.at(col);
    const int fieldIndex = Qt::DisplayRole == role ? colMapping.displayField : colMapping.editField;
    return row->data(fieldIndex);
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


bool CachedSqlTableModel::loadItems(QList<TableRow*> & dest)
{
    QSqlQuery selectQuery = prepareSelectQuery();   

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
