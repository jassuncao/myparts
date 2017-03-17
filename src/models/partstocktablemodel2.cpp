#include "partstocktablemodel2.h"
#include <QDateTime>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>

PartStockTableModel2::PartStockTableModel2(const QStringList &fieldNames, const QStringList &columnNames, QObject *parent)
    : SimpleSqlTableModel("part_stock", fieldNames, columnNames, "part", parent)
{
    registerRelation(ColumnStorage, new TableRelation("storage", "id", "name"));
    registerRelation(ColumnCondition, new TableRelation("condition", "id", "value"));
}

PartStockTableModel2 * PartStockTableModel2::createNew(QObject * parent)
{
    QStringList fieldNames;
    QStringList columnNames;
    fieldNames <<  "storage" << "condition" <<  "quantity" << "lastUpdate";
    columnNames<< tr("Storage") << tr("Condition") << tr("Quantity") << tr("Last Update");
    return new PartStockTableModel2(fieldNames, columnNames, parent);
}

TableItem * PartStockTableModel2::createBlankItem() const
{
    TableItem * item = SimpleSqlTableModel::createBlankItem();
    return item;
}

Qt::ItemFlags PartStockTableModel2::flags(const QModelIndex& index) const
{
    Qt::ItemFlags flags = SimpleSqlTableModel::flags(index);
    flags &= ~Qt::ItemIsEditable;
    flags = flags | Qt::ItemIsDragEnabled;
/*
    XXX: For now prevent any kind of edit using the views
    const int col = index.column();
    if(col == PartStockTableModel2::ColumnLastUpdate || col == PartStockTableModel2::ColumnQuantity){
        flags &= ~Qt::ItemIsEditable;
    }
    */    
    return flags;
}


Qt::DropActions PartStockTableModel2::supportedDropActions() const
{
    return Qt::CopyAction | Qt::LinkAction | Qt::MoveAction;
}

QStringList PartStockTableModel2::mimeTypes() const
{
    QStringList types;
    types << PartStockMimeData::PART_STOCK_MIMETYPE;
    return types;
}

QMimeData * PartStockTableModel2::mimeData(const QModelIndexList &indexes) const
{
    QVector<int> uniqueRows;
    foreach (const QModelIndex &index, indexes) {
        if (index.isValid()) {
            int row = index.row();
            if(!uniqueRows.contains(row)){
                uniqueRows.append(row);
            }
        }
    }
    QVariant partId = currentForeignKey();
    QList<PartStockItem> items;
    foreach (int row, uniqueRows) {
        TableItem * item = _items.at(row);
        QVariant storage = item->data(ColumnStorage);
        QVariant condition = item->data(ColumnCondition);
        QVariant quantity = item->data(ColumnQuantity);
        items.append(PartStockItem(partId, storage, condition, quantity));
    }
    PartStockMimeData * mimeData = new PartStockMimeData(items);
    return mimeData;
}

bool PartStockTableModel2::saveItem(TableItem* item)
{
    QDateTime now = QDateTime::currentDateTimeUtc();
    item->setData(PartStockTableModel2::ColumnLastUpdate, now);
    return SimpleSqlTableModel::saveItem(item);
}

bool PartStockTableModel2::insertOrUpdateRow(const QVariant & condition, const QVariant & storage, QVariant quantiy)
{
    bool res = false;
    int count = rowCount();
    int matchingRow = -1;
    for(int row = 0; row < count; ++row){
        QVariant otherCondition = index(row, ColumnCondition).data(Qt::EditRole);
        QVariant otherStorage = index(row, ColumnStorage).data(Qt::EditRole);
        if(otherCondition == condition && otherStorage == storage){
            matchingRow = row;
            break;
        }
    }
    if(matchingRow >=0 ){
        const QModelIndex idx = index(matchingRow, ColumnQuantity);
        double existing = data(idx, Qt::EditRole).toDouble();
        existing+=quantiy.toDouble();
        res = setData(idx, existing);
    }
    else{
        TableItem * item = createBlankItem();
        item->setData(ColumnCondition, condition);
        item->setData(ColumnStorage, storage);
        item->setData(ColumnQuantity, quantiy);
        res = doInsertRow(rowCount(), item);
    }
    return res;
}

QVariant PartStockTableModel2::computeCurrentStock() const
{
    int sum = 0;
    for(int i=0; i<_items.size(); ++i){
        const TableItem * item = _items.at(i);
        sum += item->data(ColumnQuantity).toDouble();
    }
    return QVariant(sum);
}


bool PartStockTableModel2::rawInsert(const QVariant & partId, const QVariant & condition, const QVariant & storage, QVariant quantiy)
{
    QDateTime lastUpdate = QDateTime::currentDateTimeUtc();
    _insertQuery.bindValue(0, storage);
    _insertQuery.bindValue(1, condition);
    _insertQuery.bindValue(2, quantiy);
    _insertQuery.bindValue(3, lastUpdate);
    _insertQuery.bindValue(4, partId);
    bool res = _insertQuery.exec();
    if(!res){
        qWarning()<<"Failed to insert part stock. Reason:"<<_insertQuery.lastError();
        return false;
    }
    return res;
}

bool PartStockTableModel2::rawMoveStockToStorage(const PartStockItem & stockItem, const QVariant & newStorage)
{
    database().transaction();

    QString mergeStockQuerySQL("UPDATE part_stock SET quantity = quantity + ? "
    "WHERE part = ? AND storage = ? AND condition = ?");

    QString deletePreviousStockQuerySQL("DELETE FROM part_stock "
    "WHERE part = ? AND storage = ? AND condition = ?");

    bool res;
    QSqlQuery mergeExistingQuery(mergeStockQuerySQL);
    mergeExistingQuery.bindValue(0, stockItem.quantity);
    mergeExistingQuery.bindValue(1, stockItem.partId);
    mergeExistingQuery.bindValue(2, newStorage);
    mergeExistingQuery.bindValue(3, stockItem.conditionId);
    res = mergeExistingQuery.exec();
    if(!res){
        qWarning()<<"Failed to execute stock merge query. Reason:"<<mergeExistingQuery.lastError();
        database().rollback();
        return false;
    }
    res = mergeExistingQuery.numRowsAffected() > 0;
    if(!res){
        res = rawInsert(stockItem.partId, stockItem.conditionId, newStorage, stockItem.quantity);
    }

    if(res){
        QSqlQuery deletePreviousStockEntryQuery(deletePreviousStockQuerySQL);
        deletePreviousStockEntryQuery.bindValue(0, stockItem.partId);
        deletePreviousStockEntryQuery.bindValue(1, stockItem.storageId);
        deletePreviousStockEntryQuery.bindValue(2, stockItem.conditionId);
        res = deletePreviousStockEntryQuery.exec();
        if(res){
            database().commit();
        }
        else{
            qWarning()<<"Failed to execute stock delete query. Reason:"<<deletePreviousStockEntryQuery.lastError();
        }
    }
    return res;
}

