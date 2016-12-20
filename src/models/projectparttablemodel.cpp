#include "projectparttablemodel.h"
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>

enum Fields {
    Field_Id,
    Field_RefDes,
    Field_Quantity,
    Field_PartName,
    Field_AssignedPart,
    Field_Remark,
    Field_AssignedPart_Name,
    FIELD_COUNT
};

ProjectPartTableModel::ProjectPartTableModel(QObject *parent) :
    CachedSqlTableModel("project_part", parent),
    _sortColum(Field_Id),
    _sortOrder(Qt::AscendingOrder)
{
    mapColumn(QVariant::String, Field_RefDes, Field_RefDes);
    mapColumn(QVariant::Int, Field_Quantity, Field_Quantity);
    mapColumn(QVariant::String, Field_PartName, Field_PartName);
    mapColumn(QVariant::Int, Field_AssignedPart_Name, Field_AssignedPart);
    mapColumn(QVariant::String, Field_Remark, Field_Remark);
    setHeaderData(0, Qt::Horizontal, tr("Ref. Des."));
    setHeaderData(1, Qt::Horizontal, tr("Quantity"));
    //setHeaderData(1, Qt::Horizontal, Qt::AlignTrailing, Qt::TextAlignmentRole);
    setHeaderData(2, Qt::Horizontal, tr("Part Name"));
    setHeaderData(3, Qt::Horizontal, tr("Assigned Part"));
    setHeaderData(4, Qt::Horizontal, tr("Remark"));
    /*
    registerColumn(tr("Id"), "id", true);
    registerColumn(tr("Ref. Des."), "refdes");
    registerColumn(tr("Quantity"), "quantity");
    registerColumn(tr("Part"), "part_name");
    registerColumn(tr("Assigned Part"), "part", "part","id");
    registerColumn(tr("Remark"), "remark");
    registerJoin(TableJoin("part", "part", "id", TableJoin::LeftJoin));
    */

    _deleteQuery.prepare("DELETE FROM project_part WHERE id = ?");
    _updateQuery.prepare("UPDATE project_part SET refdes=?, quantity=?, part_name=?, assigned_part=?, remark=? WHERE id = ?");
    _insertQuery.prepare("INSERT INTO project_part (refdes, quantity, part_name, assigned_part, remark, project) VALUES (?, ?, ?, ?, ?, ?)");
}


void ProjectPartTableModel::setProject(const QVariant & projectId)
{
    _projectId = projectId;
}

void ProjectPartTableModel::sort(int column, Qt::SortOrder order)
{
    _sortColum = column;
    _sortOrder = order;
    select();
}

QSqlQuery ProjectPartTableModel::prepareSelectQuery()
{
    QString queryText("SELECT t0.id, t0.refdes, t0.quantity, t0.part_name, t0.assigned_part, t0.remark, t1.name AS assigned_part_name FROM "
                  "project_part t0 "
                  "LEFT JOIN part t1 ON t0.assigned_part = t1.id "
                  "WHERE t0.project = ? ");
    if(_sortColum >= 0){
        int sortField = _columnsMapping.at(_sortColum).displayField + 1;
        QString orderStmt = _sortOrder == Qt::DescendingOrder ? " ORDER BY %1 DESC" : " ORDER BY %1 ASC";
        queryText.append(orderStmt.arg(sortField));
    }
    qDebug()<<"Query is: "<<queryText;
    QSqlQuery query;
    query.prepare(queryText);
    query.bindValue(0, _projectId);
    return query;
}

TableRow * ProjectPartTableModel::createBlankItem() const
{
    TableRow * row = new TableRow(QVector<QVariant>(FIELD_COUNT));
    row->setData(Field_Quantity, QVariant(0));
    return row;
}

bool ProjectPartTableModel::updateRow(TableRow * row)
{    
    _updateQuery.bindValue(0, row->data(Field_RefDes));
    _updateQuery.bindValue(1, row->data(Field_Quantity));
    _updateQuery.bindValue(2, row->data(Field_PartName));
    _updateQuery.bindValue(3, row->data(Field_AssignedPart));
    _updateQuery.bindValue(4, row->data(Field_Remark));
    _updateQuery.bindValue(5, row->data(Field_Id));
    bool res = _updateQuery.exec();
    if(!res){
        qWarning()<<"Failed to execute update query. Reason: " << _updateQuery.lastError().driverText();
    }
    return res;
}

bool ProjectPartTableModel::saveRow(TableRow * row)
{
    _insertQuery.prepare("INSERT INTO project_part (refdes, quantity, part_name, assigned_part, remark, project) VALUES (?, ?, ?, ?, ?, ?)");
    _insertQuery.bindValue(0, row->data(Field_RefDes));
    _insertQuery.bindValue(1, row->data(Field_Quantity));
    _insertQuery.bindValue(2, row->data(Field_PartName));
    _insertQuery.bindValue(3, row->data(Field_AssignedPart));
    _insertQuery.bindValue(4, row->data(Field_Remark));
    _insertQuery.bindValue(5, projectId());
    bool res = _insertQuery.exec();
    if(res){
        row->setData(Field_Id, _insertQuery.lastInsertId());
    }
    else {
        qWarning()<<"Failed to execute insert query. Reason: "<<_insertQuery.lastError().driverText();
    }
    return res;
}

bool ProjectPartTableModel::deleteRow(const TableRow * row)
{
    _deleteQuery.bindValue(0, row->data(Field_Id));
    bool res = _deleteQuery.exec();
    if(!res){
        qWarning()<<"Failed to execute delete query. Reason: "<<_deleteQuery.lastError().driverText();
    }
    return res;
}


