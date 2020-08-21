#include "treemodelmimedata.h"

TreeModelMimeData::TreeModelMimeData(const QModelIndexList &indexes) :
    QMimeData(), _indexes(indexes)
{
}
