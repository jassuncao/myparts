#include "categoymimedata.h"

CategoyMimeData::CategoyMimeData(const QModelIndexList &indexes) :
    QMimeData(), _indexes(indexes)
{
}
