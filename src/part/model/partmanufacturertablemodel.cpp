#include "partmanufacturertablemodel.h"


PartManufacturerTableModel::PartManufacturerTableModel(const QStringList &fieldNames, const QStringList &columnNames, QObject *parent)
    : SimpleSqlTableModel("part_manufacturer", fieldNames, columnNames, "part", parent)
{
    registerRelation(ColumnManufacturer, new TableRelation("manufacturer", "id", "name"));
}

PartManufacturerTableModel * PartManufacturerTableModel::createNew(QObject * parent)
{
    QStringList fieldNames;
    QStringList columnNames;
    fieldNames<<"manufacturer"<<"partNumber";
    columnNames<<tr("Manufacturer")<<tr("Part Number");
    return new PartManufacturerTableModel(fieldNames, columnNames, parent);
}
