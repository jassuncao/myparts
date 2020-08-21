#ifndef PARAMETERMODEL_H
#define PARAMETERMODEL_H

#include "core/sql/customtablemodel.h"

class TableItem;

class ParameterModel : public SimpleSqlTableModel
{
public:
    enum ColumnsIndex {
        ColumnName,
        ColumnKey,
        ColumnDatatype,
        ColumnUnit,
        ColumnDescription,
        ColumnDeletable,
        ColumnId,
    };

    static ParameterModel * createNew(QObject *parent);
private:
    QAbstractItemModel *relationModel(const int column) const;
protected:
    explicit ParameterModel(const QStringList &fieldNames, const QStringList &columnNames, QObject *parent = 0);
    TableItem * createBlankItem() const;
};

#endif // PARAMETERMODEL_H
