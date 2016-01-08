#include "partstocktablemodel.h"
#include <QDateTime>

PartStockTableModel::PartStockTableModel(const QStringList &fieldNames, const QStringList &columnNames, QObject *parent)
    : SimpleSqlTableModel("stock_change", fieldNames, columnNames, "part", parent)
{
}

bool PartStockTableModel::appendRow(const int quantity, const QVariant & price, const QString & comment)
{
    QDateTime now = QDateTime::currentDateTimeUtc();
    TableItem * item = createBlankItem();
    item->setData(ColumnDateTime, now.toTime_t());
    item->setData(ColumnChange, quantity);
    item->setData(ColumnPrice, price);
    item->setData(ColumnComment, comment);
    doInsertRow(rowCount(), item);
    return true;
}

QVariant PartStockTableModel::data(const QModelIndex &index, int role) const
{
    const QVariant & value = SimpleSqlTableModel::data(index, role);
    if(index.column()==ColumnDateTime && value.isValid() && (role==Qt::EditRole || role==Qt::DisplayRole)){
        return QDateTime::fromTime_t(value.toUInt());
    }
    return value;
}

PartStockTableModel * PartStockTableModel::createNew(QObject * parent)
{
    QStringList fieldNames;
    QStringList columnNames;
    fieldNames<<"dateTime"<<"change"<<"price"<<"comment";
    columnNames<<tr("Date")<<tr("Amount")<<tr("Price")<<tr("Comment");
    return new PartStockTableModel(fieldNames, columnNames, parent);
}
