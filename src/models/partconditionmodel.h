#ifndef PARTCONDITIONMODEL_H
#define PARTCONDITIONMODEL_H

#include "customtablemodel.h"

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

    static PartConditionModel * createNew(QObject *parent);
protected:
    explicit PartConditionModel(const QStringList &fieldNames, const QStringList &columnNames, QObject *parent = 0);
};

#endif // PARTCONDITIONMODEL_H
