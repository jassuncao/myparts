#include "partstocklogtablemodel.h"
#include <QDateTime>

PartStockLogTableModel::PartStockLogTableModel(const QStringList &fieldNames, const QStringList &columnNames, QObject *parent)
    : SimpleSqlTableModel("part_stock_log", fieldNames, columnNames, "part", parent)
{
}

bool PartStockLogTableModel::appendRow(const int quantity, const QVariant & price, const QString & comment)
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
    if(index.column()==ColumnLastUpdate && value.isValid() && (role==Qt::EditRole || role==Qt::DisplayRole)){
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
