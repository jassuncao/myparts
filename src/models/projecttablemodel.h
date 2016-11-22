#ifndef PROJECTTABLEMODEL_H
#define PROJECTTABLEMODEL_H

#include "basicentitytablemodel.h"
#include "customtablemodel.h"

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

class ProjectPartTableModel : public SimpleSqlTableModel
{
public:
    enum ColumnIndex {
        ColumnId = 0,
        ColumnQuantity,
        ColumnRefdes,
        ColumnPartName,
        ColumnPart,
        ColumnRemark
    };

    static ProjectPartTableModel * createNew(QObject *parent);
protected:
    explicit ProjectPartTableModel(const QStringList &fieldNames, const QStringList &columnNames, QObject *parent = 0);
};

#endif // PROJECTTABLEMODEL_H
