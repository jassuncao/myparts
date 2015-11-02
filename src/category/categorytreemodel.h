#ifndef CATEGORYTREEMODEL_H
#define CATEGORYTREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QIcon>
#include <QQueue>
#include "models/treeitemmodel.h"

class CategoryTreeItem;
class TreeItem;

class CategoryTreeModel : public TreeItemModel
{
    Q_OBJECT
public:
    explicit CategoryTreeModel(QObject *parent = 0);
    ~CategoryTreeModel();

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
    void partDropped();
};

#endif // CATEGORYTREEMODEL_H
