#include "storagetreemodel.h"
#include "core/treemodel/treeitem.h"
#include "core/treemodel/treemodelmimedata.h"
#include "core/treemodel/treeitemmodelpersistence.h"
#include "stock/partstockmimedata.h"
#include <QDebug>
#include <QSqlQuery>

const char * MIME_TYPE= "myparts/storage";

class StorageTreeItemModelPersistence : public TreeItemModelPersistence
{
public:
    explicit StorageTreeItemModelPersistence();
    virtual ~StorageTreeItemModelPersistence();
protected:
    void beforeRemoveNode(int newParentId, const Node & node) const;
};

StorageTreeItemModelPersistence::StorageTreeItemModelPersistence() :
    TreeItemModelPersistence("storage")
{
}

StorageTreeItemModelPersistence::~StorageTreeItemModelPersistence()
{
}

void StorageTreeItemModelPersistence::beforeRemoveNode(int newParentId, const Node & node) const
{
    QSqlQuery query(database());
    query.prepare("UPDATE part SET storage=? WHERE storage IN (SELECT id FROM storage WHERE lft BETWEEN ? AND ?)");
    query.bindValue(0,newParentId);
    query.bindValue(1,node.lft);
    query.bindValue(2,node.rgt);
    query.exec();
    qDebug("Updated %d parts",query.numRowsAffected());

}

StorageTreeModel::StorageTreeModel(QObject *parent) :
    TreeItemModel(new StorageTreeItemModelPersistence(), QLatin1String("myparts/storage"), parent)
{  
    QIcon folderIcon;
    folderIcon.addFile(":icons/box_closed", QSize(), QIcon::Normal, QIcon::Off);
    folderIcon.addFile(":icons/box_open", QSize(), QIcon::Normal, QIcon::On);
    setDefaultIcon(folderIcon);
}

StorageTreeModel::~StorageTreeModel()
{
}


QStringList StorageTreeModel::mimeTypes() const
{
    QStringList types = TreeItemModel::mimeTypes();
    types.append(PartStockMimeData::PART_STOCK_MIMETYPE);
    return types;
}

bool StorageTreeModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    if (action == Qt::IgnoreAction){
        return true;
    }

    if(data->hasFormat(PartStockMimeData::PART_STOCK_MIMETYPE)){
        const PartStockMimeData *stockData = qobject_cast<const PartStockMimeData *>(data);
        if(stockData){
            TreeItem* targetNode = getItem(parent);
            int storageId = targetNode->id();
            emit stockDropped(stockData->items(), storageId);
            return true;
        }
    }
    return TreeItemModel::dropMimeData(data, action, row, column, parent);
}

