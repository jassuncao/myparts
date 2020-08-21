#ifndef PARTCONDITIONMODEL_H
#define PARTCONDITIONMODEL_H

#include "core/sql/customtablemodel.h"

class PartConditionModel : public SimpleSqlTableModel
{
public:
    enum ColumnsIndex {        
        ColumnDefault,
        ColumnValue,
        ColumnDescription,
        ColumnId,
    };

    void makeSelectedItemDefault(const int selectedRow);
    int findDefaultValueRow() const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    static PartConditionModel * createNew(QObject *parent);
protected:
    explicit PartConditionModel(const QStringList &fieldNames, const QStringList &columnNames, QObject *parent = 0);
};

#endif // PARTCONDITIONMODEL_H
