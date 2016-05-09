#ifndef PARAMETERMODEL_H
#define PARAMETERMODEL_H

#include "customtablemodel.h"

class QAbstractListModel;

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
protected:
    explicit ParameterModel(const QStringList &fieldNames, const QStringList &columnNames, QObject *parent = 0);

};

#endif // PARAMETERMODEL_H
