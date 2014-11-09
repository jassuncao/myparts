#ifndef PARTCONDITIONSQLTABLEMODEL_H
#define PARTCONDITIONSQLTABLEMODEL_H

#include <QSqlTableModel>

class PartConditionSqlTableModel : public QSqlTableModel
{    
    Q_OBJECT

public:
    enum ColumnsIndex {
        ColumnId = 0,
        ColumnName,
        ColumnDescription,
        ColumnDefault
    };

    explicit PartConditionSqlTableModel(QObject *parent = 0);
    Qt::ItemFlags flags(const QModelIndex &index) const;
};

#endif // PARTCONDITIONSQLTABLEMODEL_H
