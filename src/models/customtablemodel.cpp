#include "customtablemodel.h"
#include <QDebug>
#include <QSqlError>
#include <QStringBuilder>
#include "parametervalue.h"

/*
AbstractTableItem::AbstractTableItem(const QVariant & id, const bool dirty) : _id(id), _dirty(dirty)
{}
*/

 TableItem::TableItem(QVector<QVariant> data, const QVariant & id, bool dirty) :
     _data(data), _id(id), _dirty(dirty)
 {
 }

void TableItem::setData(int column, const QVariant & value)
{
    if(column<_data.size()){
        _data[column] = value;
        _dirty = true;
    }
}

void TableItem::setId(QVariant id)
{
   _id=id;
   _dirty = true;
}

QVariant TableItem::data(int column) const
{
    //qDebug()<<"Data for "<<column << "  of "<<_data.size();
    if(column<_data.size()){
        return _data.at(column);
    }
    return QVariant();
}

TableRelation::TableRelation(const QString & tableName, const QString & indexField, const QString & displayField) :
    _tableName(tableName), _displayField(displayField), _indexField(indexField), _initialized(false)
{   
    QString sqlText("SELECT %1, %2 FROM %3 ORDER BY %2");
    sqlText = sqlText.arg(_indexField, _displayField, _tableName);
    qDebug()<<"Table relation query "<<sqlText;
    _selectQuery.prepare(sqlText);
}

TableRelation::~TableRelation()
{
    qDebug()<<"TableRelation for "<<_displayField<<" deleted";
}

QVariant TableRelation::displayValue(const QVariant foreignId) const
{
    return _lookupDictionary.value(foreignId.toInt());
}

bool TableRelation::validId(const QVariant foreignId) const
{
    return _lookupDictionary.contains(foreignId.toInt());
}

QAbstractListModel * TableRelation::model()
{
    if(!_initialized)
        populateDictionary();
    return this;
}

void TableRelation::populateDictionary()
{
    _lookupDictionary.clear();
    _items.clear();
    if(_selectQuery.exec()){
        while(_selectQuery.next()){
            int indexValue = _selectQuery.value(0).toInt();
            QString displayValue = _selectQuery.value(1).toString();
            _lookupDictionary[indexValue] = displayValue;
            _items.append(qMakePair(displayValue, indexValue));
        }
        _initialized = true;
    }
    else{
        qWarning()<<"Failed to execute query. Reason "<<_selectQuery.lastError().driverText();
    }
}

int TableRelation::rowCount(const QModelIndex &parent ) const
{
    if (parent.isValid())
        return 0;
    return _items.size();
}

QVariant TableRelation::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= _items.size())
        return QVariant();
    if (role == Qt::DisplayRole)
        return _items.at(index.row()).first;
    if (role == Qt::EditRole)
        return _items.at(index.row()).second;
    return QVariant();
}


CustomTableModel::CustomTableModel(int maxColumns, QObject *parent)
    : QAbstractTableModel(parent), _maxColumns(maxColumns)
{
}

CustomTableModel::~CustomTableModel()
{
    qDeleteAll(_items);
    QVector<TableRelation*>::const_iterator begin = _relations.begin();
    QVector<TableRelation*>::const_iterator end = _relations.end();
    while (begin != end) {
        if(*begin)
            delete *begin;
        ++begin;
    }
}

QVariant CustomTableModel::headerData(int section, Qt::Orientation orientation, int role) const
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

bool CustomTableModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (orientation != Qt::Horizontal || section < 0 || columnCount() <= section)
        return false;

    if (_headers.size() <= section)
        _headers.resize(qMax(section + 1, 16));
    _headers[section][role] = value;
    emit headerDataChanged(orientation, section, section);
    return true;
}

int CustomTableModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : _items.count();
}


int CustomTableModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : _maxColumns;
}

Qt::ItemFlags CustomTableModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags theFlags = QAbstractTableModel::flags(index);
    if (index.isValid()){
        theFlags |= Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
    }
    return theFlags;
}

bool CustomTableModel::insertRows(int row, int count, const QModelIndex&)
{
    beginInsertRows(QModelIndex(), row, row + count - 1);
    for (int i = 0; i < count; ++i) {
        _items.insert(row, createBlankItem());
    }
    endInsertRows();
    return true;
}

bool CustomTableModel::doInsertRow(int row, TableItem* item)
{
    beginInsertRows(QModelIndex(), row, row);
    _items.insert(row, item);
    endInsertRows();
    return true;
}


bool CustomTableModel::removeRows(int row, int count, const QModelIndex&)
{
    beginRemoveRows(QModelIndex(), row, row + count - 1);
    for (int i = 0; i < count; ++i) {
        const TableItem * item = _items.at(row);
        if(item->id().isValid()){
            _deletedItems.append(item->id());            
        }
        delete _items.takeAt(row);
    }
    endRemoveRows();
    return true;
}

void CustomTableModel::select()
{
    beginResetModel();
    qDeleteAll(_items);
    _items.clear();
    loadItems(_items);
    endResetModel();
}

bool CustomTableModel::submitAll()
{
    while (!_deletedItems.isEmpty()){
        QVariant id = _deletedItems.takeFirst();
        qDebug()<<"Deleting item "<<id;
        deleteItem(id);
    }
    for(int i=0; i<_items.count(); ++i){
        TableItem * item = _items[i];
        if(item->dirty()){
           if(saveItem(item)==false)
               return false;
        }
    }
    return true;
}

QVariant CustomTableModel::data(const QModelIndex &index, int role) const
{
    int column = index.column();
    if (!index.isValid() ||
        index.row() < 0 || index.row() >= _items.count() ||
        index.column() < 0 || column >= _maxColumns) {
        return QVariant();
    }
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        const TableItem * item = _items.at(index.row());

        if(role == Qt::DisplayRole && columnIsRelation(column)){
            TableRelation * relation = _relations[column];
            if(!relation->initialized())
                relation->populateDictionary();
            return relation->displayValue(item->data(column));
        }

        return item->data(column);
    }
    return QVariant();
}

bool CustomTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || role != Qt::EditRole ||
        index.row() < 0 || index.row() >= _items.count() ||
        index.column() < 0 || index.column() >= _maxColumns){
        return false;
    }    
    if(columnIsRelation(index.column())){
        TableRelation * relation = _relations[index.column()];
        if(!relation->initialized())
            relation->populateDictionary();

        if(!_relations[index.column()]->validId(value))
            return false;
    }
    TableItem * item = _items.at(index.row());
    item->setData(index.column(), value);
    emit dataChanged(index, index);
    return true;
}

TableItem * CustomTableModel::createBlankItem() const
{
    return new TableItem(QVector<QVariant>(_maxColumns));
}


void CustomTableModel::createRelation(const int column, const QString & tableName, const QString & indexField, const QString & displayField)
{
    if(_relations.size()<=column)
        _relations.resize(column+1);
    _relations[column] = new TableRelation(tableName, indexField, displayField);
}

QAbstractListModel * CustomTableModel::relationModel(const int column) const
{
    if(columnIsRelation(column))
        return _relations[column]->model();
    return 0;
}

bool CustomTableModel::columnIsRelation(const int column) const
{
    return column>=0 && column<_relations.size() && _relations[column]!=0;
}

void CustomTableModel::cloneData()
{
    QVariant invalidId;
    for(int i=0; i<_items.count(); ++i){
        TableItem * item = _items[i];
        item->setId(invalidId);
    }
}



SimpleSqlTableModel::SimpleSqlTableModel(const QString &tableName, const QStringList & fieldNames, const QStringList &columnNames, const QString & foreignKeyField, QObject *parent) :
    CustomTableModel(fieldNames.size(), parent), _tableName(tableName), _fieldNames(fieldNames), _foreignKeyField(foreignKeyField)
{
    QString fieldList = fieldNames.join(", ");

    QStringList placeholders;
    QStringList updatePlaceholders;
    for(int i=0;i<fieldNames.size();++i){
        const QString & field = fieldNames.at(i) ;
        updatePlaceholders.append(field % "=?");
        placeholders.append("?");
    }
    QString selectStmt;
    QString insertStmt;
    if(!_foreignKeyField.isEmpty()){
        placeholders.append("?");//foreign id

        selectStmt = QString("SELECT %2, id FROM %1 WHERE %3 = ?").arg(_tableName, fieldList, _foreignKeyField);
        insertStmt = QString("INSERT INTO %1 (%2, %3) VALUES (%4)").arg(_tableName, fieldList, _foreignKeyField, placeholders.join(", "));
    }
    else{
        selectStmt = QString("SELECT %2, id FROM %1").arg(_tableName, fieldList);
        insertStmt = QString("INSERT INTO %1 (%2) VALUES (%4)").arg(_tableName, fieldList, placeholders.join(", "));

    }

    QString updateStmt("UPDATE %1 SET %2 WHERE id = ?");
    updateStmt = updateStmt.arg(_tableName, updatePlaceholders.join(", "));

    QString deleteStmt("DELETE FROM %1 WHERE id = ?");
    deleteStmt = deleteStmt.arg(_tableName);

    qDebug()<<"Queries for "<<tableName;
    qDebug()<<"Select query: "<<selectStmt;
    qDebug()<<"Insert query: "<<insertStmt;
    qDebug()<<"Update query: "<<updateStmt;
    qDebug()<<"Delete query: "<<deleteStmt;

    _selectQuery.prepare(selectStmt);
    _insertQuery.prepare(insertStmt);
    _updateQuery.prepare(updateStmt);
    _deleteQuery.prepare(deleteStmt);

    for(int i=0; i<columnNames.size(); ++i){
        setHeaderData(i, Qt::Horizontal, columnNames.at(i));
    }
}

void SimpleSqlTableModel::setCurrentForeignKey(const QVariant & foreignKey)
{
    _foreignKey = foreignKey;
}

bool SimpleSqlTableModel::deleteItem(QVariant id)
{
    _deleteQuery.bindValue(0, id);
    bool res = _deleteQuery.exec();
    if(!res){
        qWarning()<<"Failed to execute delete query. Reason: "<<_deleteQuery.lastError().driverText();
    }
    return res;
}

bool SimpleSqlTableModel::saveItem(TableItem* item)
{
    if(item->id().isValid()){
        //Edited item. Update
        qDebug()<<"Updating item "<<item->id();
        int i = 0;
        for(; i<_maxColumns;++i){
            _updateQuery.bindValue(i, item->data(i));
        }
        _updateQuery.bindValue(i, item->id());
        if(_updateQuery.exec()){
            item->resetDirty();
            return true;
        }
        qWarning()<<"Failed to execute update query. Reason: "<<_updateQuery.lastError().driverText();
        return false;
    }
    else{
        //Brand new item. Insert
        qDebug()<<"Inserting new item ";
        int i = 0;
        for(; i<_maxColumns;++i){
            _insertQuery.bindValue(i, item->data(i));
        }
        if(!_foreignKeyField.isEmpty()){
            _insertQuery.bindValue(i, _foreignKey);
        }
        if(_insertQuery.exec()){
            item->setId(_insertQuery.lastInsertId());
            item->resetDirty();
            return true;
        }
        qWarning()<<"Failed to execute insert query. Reason: "<<_insertQuery.lastError().driverText();
        return false;
    }
}

bool SimpleSqlTableModel::loadItems(QList<TableItem*> & dest)
{
    if(!_foreignKeyField.isEmpty()){
        if(!_foreignKey.isValid())
            return false;
        qDebug()<<"loading items for "<<_foreignKey;
        _selectQuery.bindValue(0, _foreignKey);
    }

    if(_selectQuery.exec()){
        while(_selectQuery.next())
        {
            QVector<QVariant> rowData(_maxColumns);
            for(int i=0; i<_maxColumns; ++i){
                rowData[i] = _selectQuery.value(i);
            }
            QVariant id = _selectQuery.value(_maxColumns);
            TableItem * item = new TableItem(rowData, id, false);
            dest.append(item);
        }
        return true;
    }
    qWarning()<<"Failed to execute query. Reason "<<_selectQuery.lastError().driverText();
    return false;
}


AttachmentTableModel3::AttachmentTableModel3(const QString &tableName, const QStringList & fieldNames,
                                             const QStringList & columnNames, const QString & foreignKeyField, QObject *parent)
    : SimpleSqlTableModel(tableName, fieldNames, columnNames, foreignKeyField, parent)
{
}

Qt::ItemFlags AttachmentTableModel3::flags(const QModelIndex &index) const
{
    Qt::ItemFlags theFlags = SimpleSqlTableModel::flags(index);
    if(index.column()==ColumnURL){
        theFlags = theFlags & (~Qt::ItemIsEditable);
    }
    return theFlags;
}

bool AttachmentTableModel3::appendRow(const QString & url, const QString & description)
{
    TableItem * item = createBlankItem();
    item->setData(ColumnURL, url);
    item->setData(ColumnDescription, description);
    doInsertRow(rowCount(), item);
    return true;
}

AttachmentTableModel3 * AttachmentTableModel3::createNewPartAttachmentModel(QObject * parent)
{
    QStringList fieldNames;
    QStringList columnNames;
    fieldNames<<"url"<<"description";
    columnNames<<tr("Location")<<tr("Description");
    return new AttachmentTableModel3("part_attachment", fieldNames, columnNames, "part", parent);
}

AttachmentTableModel3 * AttachmentTableModel3::createNewFootprintAttachmentModel(QObject * parent)
{
    QStringList fieldNames;
    QStringList columnNames;
    fieldNames<<"url"<<"description";
    columnNames<<tr("Location")<<tr("Description");
    return new AttachmentTableModel3("footprint_attachment", fieldNames, columnNames, "footprint", parent);
}


PartManufacturerTableModel2::PartManufacturerTableModel2(const QStringList &fieldNames, const QStringList &columnNames, QObject *parent)
    : SimpleSqlTableModel("part_manufacturer", fieldNames, columnNames, "part", parent)
{
    createRelation(ColumnManufacturer, "manufacturer", "id", "name");
}

PartManufacturerTableModel2 * PartManufacturerTableModel2::createNew(QObject * parent)
{
    QStringList fieldNames;
    QStringList columnNames;
    fieldNames<<"manufacturer"<<"partNumber";
    columnNames<<tr("Manufacturer")<<tr("Part Number");
    return new PartManufacturerTableModel2(fieldNames, columnNames, parent);
}


PartDistributorTableModel2::PartDistributorTableModel2(const QStringList &fieldNames, const QStringList &columnNames, QObject *parent)
    : SimpleSqlTableModel("part_distributor", fieldNames, columnNames, "part", parent)
{
    createRelation(ColumnDistributor, "distributor", "id", "name");
    createRelation(ColumnPackaging, "packaging", "id", "name");
}

PartDistributorTableModel2 * PartDistributorTableModel2::createNew(QObject * parent)
{
    QStringList fieldNames;
    QStringList columnNames;
    fieldNames<<"distributor"<<"partNumber"<<"minimumOrder"<<"unitPrice"<<"packaging";
    columnNames<<tr("Distributor")<<tr("Part Number")<<tr("Minimum Order")<<tr("Unit Price")<<tr("Packaging");
    return new PartDistributorTableModel2(fieldNames, columnNames, parent);
}

PartParametersTableModel3::PartParametersTableModel3(const QStringList &fieldNames, const QStringList &columnNames, QObject *parent)
    : SimpleSqlTableModel("part_parameter", fieldNames, columnNames, "part", parent)
{
    createRelation(ColumnUnit, "unit", "id", "name");
    createRelation(HiddenColumnUnitSymbol, "unit", "id", "symbol");
}

QVariant PartParametersTableModel3::data(const QModelIndex &index, int role) const
{
    QVariant var = SimpleSqlTableModel::data(index, role);
    if(role == Qt::DisplayRole && index.column()==ColumnValue && var.isValid() ){
        const TableItem * item = _items.at(index.row());
        TableRelation * relation = _relations[HiddenColumnUnitSymbol];
        if(!relation->initialized())
            relation->populateDictionary();

        const QString & symbol = relation->displayValue(item->data(ColumnUnit)).toString();
        ParameterValue paramValue(var.toDouble(), symbol);
        var.setValue(paramValue);
    }
    return var;
}

PartParametersTableModel3 * PartParametersTableModel3::createNew(QObject * parent)
{
    QStringList fieldNames;
    QStringList columnNames;
    fieldNames<<"name"<<"value"<<"unit"<<"description";
    columnNames<<tr("Name")<<tr("Value")<<tr("Unit")<<tr("Description");
    return new PartParametersTableModel3(fieldNames, columnNames, parent);
}


