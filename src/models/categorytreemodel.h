#ifndef CATEGORYTREEMODEL2_H
#define CATEGORYTREEMODEL2_H

#include "treeitemmodel.h"

class TreeItem;

class CategoryTreeModel : public TreeItemModel
{
    Q_OBJECT
public:
    explicit CategoryTreeModel(QObject *parent = 0);
    ~CategoryTreeModel();
};

#endif // CATEGORYTREEMODEL2_H
