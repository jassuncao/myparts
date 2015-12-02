#include "packagetreemodel.h"
#include "treeitem.h"

PackageTreeModel::PackageTreeModel(const QVector<QVariant> columns, QObject *parent)
    : TreeItemModel(3, parent)
{

}

PackageTreeModel::~PackageTreeModel()
{

}

