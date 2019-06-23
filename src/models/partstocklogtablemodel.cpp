#include "partstocklogtablemodel.h"
#include <QDateTime>
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>

PartStockLogTableModel::PartStockLogTableModel(const QStringList &fieldNames, const QStringList &columnNames, QObject *parent)
    : SimpleSqlTableModel("part_stock_log", fieldNames, columnNames, "part", parent)
{
}

bool PartStockLogTableModel::appendRow(const QVariant & quantity, const QVariant & price, const QString & comment)
{
    QDateTime now = QDateTime::currentDateTimeUtc();
    TableItem * item = createBlankItem();
    item->setData(ColumnLastUpdate, now.toTime_t());
    item->setData(ColumnChange, quantity);
    item->setData(ColumnPrice, price);
    item->setData(ColumnComment, comment);
    doInsertRow(rowCount(), item);
    return true;
}

QVariant PartStockLogTableModel::data(const QModelIndex &index, int role) const
{
    const QVariant & value = SimpleSqlTableModel::data(index, role);
    const int col = index.column();
    if(col == ColumnLastUpdate && value.isValid() && (role==Qt::EditRole || role==Qt::DisplayRole)){
        return QDateTime::fromTime_t(value.toUInt());
    }   
    return value;
}

QVariant PartStockLogTableModel::computeAveragePrice() const
{
    double priceSum = 0;
    int count = 0;
    for(int i=0; i<_items.size(); ++i){
        const TableItem * item = _items.at(i);
        bool priceOk;
        double price = item->data(ColumnPrice).toDouble(&priceOk);
        if(priceOk && price > 0){
            count++;
            priceSum+=price;
        }
    }
    if(count > 0){
        return QVariant(priceSum / count);
    }
    return QVariant();
}

PartStockLogTableModel * PartStockLogTableModel::createNew(QObject * parent)
{
    QStringList fieldNames;
    QStringList columnNames;
    fieldNames<<"lastUpdate"<<"change"<<"price"<<"comment";
    columnNames<<tr("Date")<<tr("Amount")<<tr("Price")<<tr("Comment");
    return new PartStockLogTableModel(fieldNames, columnNames, parent);
}

bool PartStockLogTableModel::rawInsert(const QVariant & partId, const QVariant & quantity, const QVariant & price, const QString & comment)
{
    QDateTime lastUpdate = QDateTime::currentDateTimeUtc();
    _insertQuery.bindValue(0, lastUpdate);
    _insertQuery.bindValue(1, quantity);
    _insertQuery.bindValue(2, price);
    _insertQuery.bindValue(3, comment);
    _insertQuery.bindValue(4, partId);
    bool res = _insertQuery.exec();
    if(!res){
        qWarning()<<"Failed to insert stock log. Reason:"<<_insertQuery.lastError();
        return false;
    }
    return res;
}
