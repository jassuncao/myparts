#include "footprinttreemodel.h"
#include "treeitem.h"

FootprintTreeModel::FootprintTreeModel(const QVector<QVariant> columns, QObject *parent)
    : TreeItemModel(new TreeItem(-1, columns), parent)
{

}

FootprintTreeModel::~FootprintTreeModel()
{

}

