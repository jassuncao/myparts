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

QVariant PartStockTableModel2::data(const QModelIndex &index, int role) const
{
    const QVariant & value = SimpleSqlTableModel::data(index, role);
    const int col = index.column();
    if(col == ColumnLastUpdate && value.isValid() && (role==Qt::EditRole || role==Qt::DisplayRole)){
        return QDateTime::fromTime_t(value.toUInt());
    }   
    return value;
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
    item->setData(PartStockTableModel2::ColumnLastUpdate, now.toTime_t());
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
    QString mergeStockQuerySQL("UPDATE part_stock SET quantity = quantity + ? "
    "WHERE part = ? AND storage = ? AND condition = ?");

    QString deletePreviousStockQuerySQL("DELETE FROM part_stock "
    "WHERE part = ? AND storage = ? AND condition = ?");

    bool res;

    database().transaction();

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
            database().rollback();
            qWarning()<<"Failed to execute stock delete query. Reason:"<<deletePreviousStockEntryQuery.lastError();
        }
    }
    return res;
}

bool PartStockTableModel2::rawMovePartToStorage(int partId, const QVariant & newStorage)
{
    //merge the quantity from other storages that have records with conditions already present in the target storage
    QString mergeQuantityExistingConditionSQL(
                "UPDATE part_stock SET quantity = quantity + coalesce(0, "
                "(SELECT SUM(s1.quantity) FROM part_stock s1 "
                    "WHERE s1.condition = part_stock.condition AND s1.storage <> part_stock.storage AND s1.part = part_stock.part) ) "
                "WHERE part = ? AND storage = ?");


    QString insertRecsWithOtherConditionsSQL(
                "INSERT INTO part_stock (part, quantity, storage, condition, lastUpdate) "
                "SELECT s1.part, SUM(s1.quantity), :dst1 , s1.condition, strftime('%s','now') FROM part_stock s1 "
                "WHERE s1.part = :part1 AND s1.storage <> :dst2 "
                "AND s1.condition NOT IN "
                "(SELECT s2.condition FROM part_stock s2 WHERE s2.part = :part2 AND s2.storage = :dst3) "
                "GROUP BY s1.condition");


    QString deleteRecsFromOtherStorages("DELETE FROM part_stock WHERE part = :part AND storage <> :dst");

    bool res;
    QSqlQuery query;
    query.prepare(mergeQuantityExistingConditionSQL);
    query.bindValue(0, partId);
    query.bindValue(1, newStorage);   
    res = query.exec();

    if(!res){
        qWarning()<<"Failed to execute stock merge query. Reason:"<<query.lastError();
        qWarning()<<"Query:"<<query.lastQuery();
        QMap<QString,QVariant> values = query.boundValues();
        QMapIterator<QString, QVariant> it(values);
        while (it.hasNext()) {
            it.next();
            qWarning() << it.key() << " = " << it.value();
        }
        database().rollback();
        return false;
    }

    query.prepare(insertRecsWithOtherConditionsSQL);
    query.bindValue(":dst1", newStorage);
    query.bindValue(":part1", partId);
    query.bindValue(":dst2", newStorage);
    query.bindValue(":part2", partId);
    query.bindValue(":dst3", newStorage);

    res = query.exec();

    if(!res){
        qWarning()<<"Failed to execute stock insert query. Reason:"<<query.lastError();
        database().rollback();
        return false;
    }

    query.prepare(deleteRecsFromOtherStorages);
    query.bindValue(":part", partId);
    query.bindValue(":dst", newStorage);
    res = query.exec();

    if(!res){
        qWarning()<<"Failed to execute stock delete query. Reason:"<<query.lastError();
        database().rollback();
        return false;
    }
    database().commit();
    return true;

    /*
     *

    UPDATE part_stock SET quantity=quantity +
        (SELECT SUM(s1.quantity) FROM part_stock s1 WHERE s1.condition = part_stock.condition AND s1.storage <> part_stock.storage AND s1.part = part_stock.part)
    WHERE part = 3 AND storage=4;

    UPDATE part_stock SET quantity=quantity +
        (SELECT SUM(s1.quantity) FROM part_stock s1 WHERE s1.condition = part_stock.condition AND s1.storage <> part_stock.storage AND s1.part = 3)
    WHERE part = 3 AND storage=4;

    UPDATE part_stock SET quantity = quantity + coalesce(0,
    (SELECT SUM(s1.quantity) FROM part_stock s1 WHERE s1.condition = part_stock.condition AND s1.storage <> part_stock.storage AND s1.part = part_stock.part))
    WHERE part_stock.part = 3 AND part_stock.storage = 4;

    SELECT s1.part, s1.condition, SUM(s1.quantity) FROM part_stock s1 WHERE s1.condition = part_stock.condition AND s1.storage <> part_stock.storage AND s1.part = part_stock.part;

    SELECT part_stock.condition, part_stock.quantity + coalesce(0, (SELECT SUM(s1.quantity) FROM part_stock s1 WHERE s1.condition = part_stock.condition AND s1.storage <> part_stock.storage AND s1.part = part_stock.part)) FROM part_stock WHERE part = 3 AND storage=4;


    SELECT part_stock.quantity + (SELECT SUM(s1.quantity) FROM part_stock s1 WHERE s1.condition = part_stock.condition AND s1.storage <> part_stock.storage AND s1.part = part_stock.part) FROM part_stock WHERE part = 3 AND storage=7;
    SELECT SUM(s1.quantity), s1.condition FROM part_stock s1 WHERE s1.part = 3 AND s1.storage <> 7 GROUP BY s1.condition;

    INSERT INTO part_stock (part, quantity, storage, condition, lastUpdate)
    SELECT s1.part, SUM(s1.quantity), 7, s1.condition, strftime('%s','now') FROM part_stock s1
            WHERE s1.part = 3 AND s1.storage <> 7
            AND s1.condition NOT IN (SELECT s2.condition FROM part_stock s2 WHERE s2.storage = 7)
            GROUP BY s1.condition;

    DELETE FROM part_stock WHERE part = 3 AND storage <> 7;
    */
}

