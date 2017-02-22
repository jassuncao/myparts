#include "customtablemodel.h"
#include <QDebug>
#include <QSqlError>
#include <QStringBuilder>
#include <QDateTime>
#include <QMimeData>
#include <QUrl>
#include "parametervalue.h"
#include "price.h"

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

TableRelation::TableRelation(const QString & tableName, const QString & indexField, const QString & displayField, const bool nullable) :
    _tableName(tableName), _displayField(displayField), _indexField(indexField), _initialized(false), _nullable(nullable)
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
    if(foreignId.isNull() && _nullable)
        return true;
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
            _lookupDictionary[indexValue] = displayValue;\
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
    if (role == Qt::DisplayRole || role == Qt::EditRole)
        return _items.at(index.row()).first;
    if (role == IModelWithForeignKey::ForeignKeyRole)
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
    QVector<ModelRelation*>::const_iterator begin = _relations.begin();
    QVector<ModelRelation*>::const_iterator end = _relations.end();
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
            ModelRelation * relation = _relations[column];
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
        ModelRelation * relation = _relations[index.column()];
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


//void CustomTableModel::createRelation(const int column, const QString & tableName, const QString & indexField, const QString & displayField, const bool nullable)
//{
//    if(_relations.size()<=column)
//        _relations.resize(column+1);
//    _relations[column] = new TableRelation(tableName, indexField, displayField, nullable);
//}

void CustomTableModel::registerRelation(const int column, ModelRelation * relation)
{
    if(_relations.size()<=column)
        _relations.resize(column+1);
    _relations[column] = relation;
}

QAbstractItemModel * CustomTableModel::relationModel(const int column) const
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
    CustomTableModel(fieldNames.size(), parent), _tableName(tableName), _fieldNames(fieldNames), _foreignKeyField(foreignKeyField), _sortColumn(-1)
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
        //selectStmt = QString("SELECT %2, id FROM %1 WHERE %3 = ?").arg(_tableName, fieldList, _foreignKeyField);
        insertStmt = QString("INSERT INTO %1 (%2, %3) VALUES (%4)").arg(_tableName, fieldList, _foreignKeyField, placeholders.join(", "));
    }
    else{
        //selectStmt = QString("SELECT %2, id FROM %1").arg(_tableName, fieldList);
        insertStmt = QString("INSERT INTO %1 (%2) VALUES (%4)").arg(_tableName, fieldList, placeholders.join(", "));

    }

    QString updateStmt("UPDATE %1 SET %2 WHERE id = ?");
    updateStmt = updateStmt.arg(_tableName, updatePlaceholders.join(", "));

    QString deleteStmt("DELETE FROM %1 WHERE id = ?");
    deleteStmt = deleteStmt.arg(_tableName);

    selectStmt = generateSelectStatement(_tableName, _fieldNames, _foreignKeyField, -1, Qt::AscendingOrder);

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

QString SimpleSqlTableModel::generateSelectStatement(const QString & tableName,  const QStringList & fieldNames, const QString & foreignKeyField, int sortColumn, Qt::SortOrder order)
{
    QString selectStmt;
    QString fieldList = fieldNames.join(", ");
    if(!foreignKeyField.isEmpty()){
        selectStmt = QString("SELECT %2, id FROM %1 WHERE %3 = ?").arg(tableName, fieldList, foreignKeyField);
    }
    else{
        selectStmt = QString("SELECT %2, id FROM %1").arg(tableName, fieldList);
    }
    if(sortColumn>=0 && sortColumn<fieldNames.size()){
        QLatin1String orderStatement = order==Qt::AscendingOrder ? QLatin1String(" ASC") : QLatin1String(" DESC");
        selectStmt.append(" ORDER BY ").append(fieldNames.at(sortColumn)).append(orderStatement);
    }
    return selectStmt;
}

void SimpleSqlTableModel::setCurrentForeignKey(const QVariant & foreignKey)
{
    _foreignKey = foreignKey;
}

void SimpleSqlTableModel::sort(int column, Qt::SortOrder order)
{
    const QString & selectStmt = generateSelectStatement(_tableName, _fieldNames, _foreignKeyField, column, order);
    qDebug()<<"Select query: "<<selectStmt;
    _selectQuery.prepare(selectStmt);
    select();
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
    Qt::ItemFlags theFlags = SimpleSqlTableModel::flags(index) | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
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

Qt::DropActions AttachmentTableModel3::supportedDropActions() const
{
    return Qt::MoveAction | Qt::CopyAction | Qt::LinkAction;
}

bool AttachmentTableModel3::dropMimeData(const QMimeData *data, Qt::DropAction, int row, int column, const QModelIndex &)
{
    bool success = false;
    qDebug()<<"Dropping "<<data<<" At "<<row<<","<<column;
    if(data->hasUrls()){
        int newRow = rowCount();
        success = insertRows(newRow, data->urls().size(), QModelIndex());
        if(success){
            foreach(QUrl url, data->urls()) {
                const QModelIndex idx = index(newRow, ColumnURL);
                setData(idx, url.toString());
                newRow++;
            }
        }
        else{
            qWarning()<<"Failed to append rows to attachments model";
        }
    }
    return success;
}

AttachmentTableModel3 * AttachmentTableModel3::createNewPartAttachmentModel(QObject * parent)
{
    QStringList fieldNames;
    QStringList columnNames;
    fieldNames<<"url"<<"description";
    columnNames<<tr("Location")<<tr("Description");
    return new AttachmentTableModel3("part_attachment", fieldNames, columnNames, "part", parent);
}

AttachmentTableModel3 * AttachmentTableModel3::createNewPackageAttachmentModel(QObject * parent)
{
    QStringList fieldNames;
    QStringList columnNames;
    fieldNames<<"url"<<"description";
    columnNames<<tr("Location")<<tr("Description");
    return new AttachmentTableModel3("package_attachment", fieldNames, columnNames, "package", parent);
}

AttachmentTableModel3 * AttachmentTableModel3::createNewProjectAttachmentModel(QObject * parent)
{
    QStringList fieldNames;
    QStringList columnNames;
    fieldNames<<"url"<<"description";
    columnNames<<tr("Location")<<tr("Description");
    return new AttachmentTableModel3("project_attachment", fieldNames, columnNames, "project", parent);
}


PartManufacturerTableModel2::PartManufacturerTableModel2(const QStringList &fieldNames, const QStringList &columnNames, QObject *parent)
    : SimpleSqlTableModel("part_manufacturer", fieldNames, columnNames, "part", parent)
{
    registerRelation(ColumnManufacturer, new TableRelation("manufacturer", "id", "name"));
    //createRelation(ColumnManufacturer, "manufacturer", "id", "name");
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
    registerRelation(ColumnDistributor, new TableRelation("distributor", "id", "name"));
    registerRelation(ColumnPackaging, new TableRelation("packaging", "id", "name"));
    //createRelation(ColumnDistributor, "distributor", "id", "name");
    //createRelation(ColumnPackaging, "packaging", "id", "name");
}

PartDistributorTableModel2 * PartDistributorTableModel2::createNew(QObject * parent)
{
    QStringList fieldNames;
    QStringList columnNames;
    fieldNames <<  "distributor"  <<  "partNumber" << "minimumOrder" << "unitPrice" << "currency" << "packaging" << "dateTime";
    columnNames<<tr("Distributor")<<tr("Part Number")<<tr("Minimum Order")<<tr("Unit Price")<<tr("Currency")<<tr("Packaging") << tr("Date");
    return new PartDistributorTableModel2(fieldNames, columnNames, parent);
}

TableItem * PartDistributorTableModel2::createBlankItem() const
{
    QDateTime now = QDateTime::currentDateTimeUtc();
    TableItem * item = SimpleSqlTableModel::createBlankItem();
    item->setData(ColumnMinimumOrder, QVariant(0.0));
    item->setData(ColumnUnitPrice, QVariant(0.0));
    item->setData(ColumnDateTime, now.toTime_t());
    return item;
}

Qt::ItemFlags PartDistributorTableModel2::flags(const QModelIndex& index) const
{
    Qt::ItemFlags res = SimpleSqlTableModel::flags(index);
    if(index.column() == PartDistributorTableModel2::ColumnDateTime){
        res &= ~Qt::ItemIsEditable;
    }
    return res;
}

QVariant PartDistributorTableModel2::data(const QModelIndex &index, int role) const
{
    if(!index.isValid()) return QVariant();
    int column = index.column();
    if(column == PartDistributorTableModel2::ColumnUnitPrice){
        if(role == Qt::DisplayRole || role == Qt::EditRole){
            QVariant value = SimpleSqlTableModel::data(index, role);
            if(value.isValid()){
                QVariant currency = SimpleSqlTableModel::index(index.row(), PartDistributorTableModel2::ColumnCurrency).data();
                return QVariant::fromValue(Price(value.toDouble(), currency.toString()));
            }
            else{
                return value;
            }
        }
    }
    return SimpleSqlTableModel::data(index, role);
}

