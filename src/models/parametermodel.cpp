#include "parametermodel.h"

ParameterModel::ParameterModel(const QStringList &fieldNames, const QStringList &columnNames, QObject *parent)
    : SimpleSqlTableModel("parameter", fieldNames, columnNames, QString(), parent)
{
    createRelation(ColumnUnit, "unit", "id", "name");
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

