#ifndef CATEGORYTREEMODEL2_H
#define CATEGORYTREEMODEL2_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QIcon>
#include <QQueue>
#include <QVector>
#include "treeitemmodel.h"
#include "treeitemmodelpersistence.h"

class CategoryTreeItem;
class TreeItem;

class CategoryTreeModel2 : public TreeItemModel
{
    Q_OBJECT
public:
    explicit CategoryTreeModel2(QObject *parent = 0);
    ~CategoryTreeModel2();     
};

#endif // CATEGORYTREEMODEL2_H
