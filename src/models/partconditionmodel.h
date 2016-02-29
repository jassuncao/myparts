#ifndef PARTCONDITIONMODEL_H
#define PARTCONDITIONMODEL_H

#include "customtablemodel.h"

class PartConditionModel : public SimpleSqlTableModel
{
public:
    enum ColumnsIndex {
        ColumnId = 0,
        ColumnValue,
        ColumnDefault
    };

    explicit PartConditionModel(const QStringList &fieldNames, const QStringList &columnNames, QObject *parent = 0);
    void makeSelectedItemDefault(const int selectedRow);
    int findDefaultValueRow() const;

};

#endif // PARTCONDITIONMODEL_H
