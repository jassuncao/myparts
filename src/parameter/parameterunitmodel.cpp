#include "parameterunitmodel.h"

ParameterUnitModel::ParameterUnitModel(const QStringList &fieldNames, const QStringList &columnNames, QObject *parent)
    : SimpleSqlTableModel("unit", fieldNames, columnNames, QString(), parent)
{
}

ParameterUnitModel * ParameterUnitModel::createNew(QObject * parent)
{
    QStringList fieldNames;
    QStringList columnNames;
    fieldNames<<"name"<<"symbol"<<"deletable";
    columnNames<<tr("Name")<<tr("Symbol");
    return new ParameterUnitModel(fieldNames, columnNames, parent);
}

TableItem * ParameterUnitModel::createBlankItem() const
{
    TableItem* item = new TableItem(QVector<QVariant>(_maxColumns));
    item->setData(ColumnDeletable, QVariant(true));
    return item;
}
