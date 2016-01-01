#ifndef CATEGORYTREEMODEL2_H
#define CATEGORYTREEMODEL2_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QIcon>
#include <QQueue>
#include <QVector>
#include "treeitemmodel.h"
#include "treeitemmodelpersistence.h"

class TreeItem;

class CategoryTreeModel : public TreeItemModel
{
    Q_OBJECT
public:
    explicit CategoryTreeModel(QObject *parent = 0);
    ~CategoryTreeModel();
};

#endif // CATEGORYTREEMODEL2_H
