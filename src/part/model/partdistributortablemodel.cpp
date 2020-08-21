#include "partdistributortablemodel.h"
#include "core/price.h"

#include <QDateTime>

PartDistributorTableModel::PartDistributorTableModel(const QStringList &fieldNames, const QStringList &columnNames, QObject *parent)
: SimpleSqlTableModel("part_distributor", fieldNames, columnNames, "part", parent)
{
    registerRelation(ColumnDistributor, new TableRelation("distributor", "id", "name"));
    registerRelation(ColumnPackaging, new TableRelation("packaging", "id", "name"));
}

PartDistributorTableModel * PartDistributorTableModel::createNew(QObject * parent)
{
    QStringList fieldNames;
    QStringList columnNames;
    fieldNames <<  "distributor"  <<  "partNumber" << "minimumOrder" << "unitPrice" << "currency" << "packaging" << "dateTime";
    columnNames<<tr("Distributor")<<tr("Part Number")<<tr("Minimum Order")<<tr("Unit Price")<<tr("Currency")<<tr("Packaging") << tr("Date");
    return new PartDistributorTableModel(fieldNames, columnNames, parent);
}

TableItem * PartDistributorTableModel::createBlankItem() const
{
    QDateTime now = QDateTime::currentDateTimeUtc();
    TableItem * item = SimpleSqlTableModel::createBlankItem();
    item->setData(ColumnMinimumOrder, QVariant(0.0));
    item->setData(ColumnUnitPrice, QVariant(0.0));
    item->setData(ColumnDateTime, now.toTime_t());
    return item;
}

Qt::ItemFlags PartDistributorTableModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags res = SimpleSqlTableModel::flags(index);
    if(index.column() == PartDistributorTableModel::ColumnDateTime){
        res &= ~Qt::ItemIsEditable;
    }
    return res;
}

QVariant PartDistributorTableModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid()) return QVariant();
    int column = index.column();
    if(column == PartDistributorTableModel::ColumnUnitPrice){
        if(role == Qt::DisplayRole){
            QVariant value = SimpleSqlTableModel::data(index, role);
            if(value.isValid()){
                QVariant currency = SimpleSqlTableModel::index(index.row(), PartDistributorTableModel::ColumnCurrency).data();
                return QVariant::fromValue(Price(value.toDouble(), currency.toString()));
            }
            else{
                return value;
            }
        }
    }
    return SimpleSqlTableModel::data(index, role);
}

bool PartDistributorTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    int column = index.column();
    bool res(false);
    if(column == PartDistributorTableModel::ColumnUnitPrice && value.canConvert<Price>()){
        Price price = qvariant_cast<Price>(value);
        const QModelIndex currencyIndex = SimpleSqlTableModel::index(index.row(), PartDistributorTableModel::ColumnCurrency);

        res = SimpleSqlTableModel::setData(currencyIndex, price.currency(), Qt::EditRole);
        res = res && SimpleSqlTableModel::setData(index, price.value(), Qt::EditRole);
    }
    else {
        res = SimpleSqlTableModel::setData(index, value, role);
    }

    return res;
}
