#include "partunitsreadonlymodel.h"
#include "../utils.h"

PartUnitsReadOnlyModel::PartUnitsReadOnlyModel() :
    QSqlQueryModel(parent)
{
    setQuery("SELECT id, name, defaultUnit FROM part_unit ORDER BY name ASC");
}

int PartUnitsReadOnlyModel::findDefaultUnitRow() const
{
    return Utils::findDefaultValueRow(this, ColumnDefault);
}
