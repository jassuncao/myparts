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
    Qt::ItemFlags res = SimpleSqlTableModel::flags(index);
    res &= ~Qt::ItemIsEditable;
/*
    XXX: For now prevent any kind of edit using the views
    const int col = index.column();
    if(col == PartStockTableModel2::ColumnLastUpdate || col == PartStockTableModel2::ColumnQuantity){
        res &= ~Qt::ItemIsEditable;
    }
    */    
    return res;
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


