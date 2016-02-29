#ifndef PARTUNITSREADONLYMODEL_H
#define PARTUNITSREADONLYMODEL_H

#include <QSqlQueryModel>

class PartUnitsReadOnlyModel : public QSqlQueryModel
{
public:
    enum ColumnsIndex {
        ColumnId = 0,
        ColumnName,
        ColumnDefault
    };
    PartUnitsReadOnlyModel(QObject *parent = 0);
    int findDefaultUnitRow() const;
};

#endif // PARTUNITSREADONLYMODEL_H
