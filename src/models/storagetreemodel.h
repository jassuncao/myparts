#ifndef STORAGETREEMODEL_H
#define STORAGETREEMODEL_H

#include "treeitemmodel.h"
#include "treeitemmodelpersistence.h"

class StorageDAO;

class StorageTreeModel : public TreeItemModel
{
    Q_OBJECT
public:
    explicit StorageTreeModel(QObject *parent = 0);
    ~StorageTreeModel();
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
    void partsDropped(QVector<int> parts, int storageId);

private:
    TreeItemModelPersistence * _modelPersistence;
};

#endif // STORAGETREEMODEL_H
