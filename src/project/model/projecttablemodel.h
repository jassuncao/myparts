#ifndef PROJECTTABLEMODEL_H
#define PROJECTTABLEMODEL_H

#include "core/sql/basicentitytablemodel.h"
#include "core/sql/customtablemodel.h"

class ProjectTableModel : public BasicEntityTableModel {
public:
    enum ColumnsIndex {
        ColumnId = 0,
        ColumnName,
        ColumnDescription,
        ColumnRemark
    };
    explicit ProjectTableModel(QObject *parent = 0, QSqlDatabase db = QSqlDatabase());
    int getNameColumn() const;
    QModelIndex findIndexOfKeyValue(const QVariant& keyValue);
    QVariant keyValue(int row);
};

/*
class ProjectPartTableModel : public CustomTableModel
{
public:
    enum ColumnIndex {        
        ColumnRefdes = 0,
        ColumnQuantity,
        ColumnPartName,
        ColumnPart,
        ColumnRemark
    };

    explicit ProjectPartTableModel(QObject *parent = 0);
    QVariant data(const QModelIndex &index, int role) const;
    void setParentKey(const QVariant & parentKey);
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);
protected:

    QString generateSelectStatement(int sortColumn, Qt::SortOrder order);
    TableItem * createBlankItem() const;
    virtual bool deleteItem(QVariant id);
    virtual bool saveItem(TableItem* item) ;
    virtual bool loadItems(QList<TableItem *> &dest);
private:
    QVariant _parentKey;
    QSqlQuery _selectQuery;
    QSqlQuery _insertQuery;
    QSqlQuery _updateQuery;
    QSqlQuery _deleteQuery;
    int _sortColumn;
    Qt::SortOrder _order;
};
*/

#endif // PROJECTTABLEMODEL_H
