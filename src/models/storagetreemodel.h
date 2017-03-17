#ifndef STORAGETREEMODEL_H
#define STORAGETREEMODEL_H

#include "treeitemmodel.h"
#include "partstockmimedata.h"
#include <QModelIndex>
#include <QList>

class StorageTreeModel : public TreeItemModel
{
    Q_OBJECT
public:
    explicit StorageTreeModel(QObject *parent = 0);
    ~StorageTreeModel();
    QStringList mimeTypes() const;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);
signals:
    void stockDropped(const QList<PartStockItem> & stockItems, const QVariant & storageId );
};

#endif // STORAGETREEMODEL_H
