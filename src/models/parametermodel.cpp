#include "parametermodel.h"
#include "extrarowproxymodel.h"

ParameterModel::ParameterModel(const QStringList &fieldNames, const QStringList &columnNames, QObject *parent)
    : SimpleSqlTableModel("parameter", fieldNames, columnNames, QString(), parent)
{
    createRelation(ColumnUnit, "unit", "id", "name", true);
    createRelation(ColumnDatatype, "datatype", "id", "name");
}

ParameterModel * ParameterModel::createNew(QObject * parent)
{
    QStringList fieldNames;
    QStringList columnNames;
    fieldNames<<"name"<<"key"<<"datatype"<<"unit"<<"description"<<"deletable";
    columnNames<<tr("Name")<<tr("Key")<<tr("Datatype")<<tr("Unit")<<tr("Description");
    return new ParameterModel(fieldNames, columnNames, parent);
}

TableItem * ParameterModel::createBlankItem() const
{
    TableItem* item = new TableItem(QVector<QVariant>(_maxColumns));
    item->setData(ColumnDeletable, QVariant(true));
    return item;
}

QAbstractItemModel * ParameterModel::relationModel(const int column) const
{
    QAbstractItemModel * listModel = SimpleSqlTableModel::relationModel(column);
    if(column==ColumnUnit){
         ExtraRowProxyModel * proxyModel = new ExtraRowProxyModel(listModel);
         proxyModel->setSourceModel(listModel);
         return proxyModel;
    }
    else{
        return listModel;
    }
}
