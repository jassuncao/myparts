#ifndef PACKAGETREEMODEL_H
#define PACKAGETREEMODEL_H
#include "treeitemmodel.h"

class PackageTreeModel : public TreeItemModel
{
public:
    PackageTreeModel(const QVector<QVariant> columns, QObject *parent = 0);
    ~PackageTreeModel();
};

#endif // PACKAGETREEMODEL_H
