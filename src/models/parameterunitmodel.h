#ifndef PARAMETERUNITMODEL_H
#define PARAMETERUNITMODEL_H

#include "customtablemodel.h"

class TableItem;

class ParameterUnitModel : public SimpleSqlTableModel
{
public:
    enum ColumnsIndex {
        ColumnName,
        ColumnSymbol,
        ColumnDeletable
    };

    static ParameterUnitModel * createNew(QObject *parent);
protected:
    explicit ParameterUnitModel(const QStringList &fieldNames, const QStringList &columnNames, QObject *parent = 0);
    TableItem * createBlankItem() const;

};

#endif // PARAMETERUNITMODEL_H
