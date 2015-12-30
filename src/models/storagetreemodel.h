#ifndef STORAGETREEMODEL_H
#define STORAGETREEMODEL_H

#include "treeitemmodel.h"

class StorageTreeModel : public TreeItemModel
{
    Q_OBJECT
public:
    explicit StorageTreeModel(QObject *parent = 0);
    ~StorageTreeModel();
};

#endif // STORAGETREEMODEL_H
