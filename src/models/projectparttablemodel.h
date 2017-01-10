#ifndef PROJECTPARTTABLEMODEL_H
#define PROJECTPARTTABLEMODEL_H

#include "cachedsqltablemodel.h"

class ProjectPartTableModel : public CachedSqlTableModel
{
public:        
    enum Columns {
        RefDes,
        Quantity,
        PartName,
        AssignedPart,
        Remark
    };

    ProjectPartTableModel(QObject *parent);
    QVariant projectId() const { return _projectId;}
    void setProject(const QVariant & projectId);
    void sort(int column, Qt::SortOrder order);
    Qt::ItemFlags flags(const QModelIndex &index) const;
protected:
    virtual QSqlQuery prepareSelectQuery();
    virtual TableRow * createBlankItem() const;
    virtual bool updateRow(TableRow * row);
    virtual bool saveRow(TableRow * row);
    virtual bool deleteRow(const TableRow * row);
private:
    QVariant _projectId;
    int _sortColum;
    Qt::SortOrder _sortOrder;    
    QSqlQuery _deleteQuery;
    QSqlQuery _updateQuery;
    QSqlQuery _insertQuery;
};

#endif // PROJECTPARTTABLEMODEL_H
