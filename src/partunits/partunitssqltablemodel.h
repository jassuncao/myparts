#ifndef PARTUNITSSQLTABLEMODEL_H
#define PARTUNITSSQLTABLEMODEL_H

#include <QSqlTableModel>

class PartUnitsSqlTableModel : public QSqlTableModel
{
public:
    enum ColumnsIndex {
        ColumnId = 0,
        ColumnName,
        ColumnAbbreviation,
        ColumnDefault
    };

    explicit PartUnitsSqlTableModel(QObject *parent = 0);    
    Qt::ItemFlags flags(const QModelIndex &index) const;
private:    
};

#endif // PARTUNITSSQLTABLEMODEL_H
