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

    virtual bool fillTree(TreeItem * rootItem);
    virtual bool doInsert(TreeItem * item);
    virtual bool doUpdate(TreeItem * item);
    virtual bool doDelete(TreeItem * item);
    virtual bool doRevert(TreeItem * item);

    Qt::DropActions supportedDropActions() const;
    virtual QStringList mimeTypes() const;
    virtual QMimeData *mimeData(const QModelIndexList &indexes) const;
    virtual bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);

signals:
    void partsDropped(QVector<int> parts, int categoryId);
private:
    TreeItemModelPersistence * _modelPersistence;    
};

#endif // CATEGORYTREEMODEL2_H
