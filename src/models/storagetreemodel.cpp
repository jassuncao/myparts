#include "storagetreemodel.h"
#include "entities/storagedao.h"
#include "models/treeitem.h"
#include "models/treemodelmimedata.h"
#include <QDebug>
#include <QSqlQuery>

const char * MIME_TYPE= "myparts/storage";

class StorageTreeItemModelPersistence : public TreeItemModelPersistence
{
public:
    explicit StorageTreeItemModelPersistence();
    virtual ~StorageTreeItemModelPersistence();
protected:
    TreeItem * createNewItem(const QSqlRecord & record) const;
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
    TreeItemModel(parent), _modelPersistence(new StorageTreeItemModelPersistence())
{  
    QIcon folderIcon;
    folderIcon.addFile(":icons/box_closed", QSize(), QIcon::Normal, QIcon::Off);
    folderIcon.addFile(":icons/box_open", QSize(), QIcon::Normal, QIcon::On);
    setFolderIcon(folderIcon);
}

StorageTreeModel::~StorageTreeModel()
{
    delete _modelPersistence;
}

bool StorageTreeModel::fillTree(TreeItem * rootItem)
{
    return _modelPersistence->loadTree(rootItem);;
}

bool StorageTreeModel::doInsert(TreeItem * item)
{
    return _modelPersistence->insertAtEnd(item);
}

bool StorageTreeModel::doUpdate(TreeItem * item)
{
    return _modelPersistence->save(item);
}

bool StorageTreeModel::doDelete(TreeItem * item)
{
    return _modelPersistence->remove(item);
}

bool StorageTreeModel::doRevert(TreeItem * item)
{
    return _modelPersistence->revert(item);
}

Qt::DropActions StorageTreeModel::supportedDropActions() const
{
    return Qt::MoveAction | Qt::LinkAction;
}

QStringList StorageTreeModel::mimeTypes() const
{
    QStringList types;
    types << MIME_TYPE << "myparts/part";
    return types;
}

QMimeData *StorageTreeModel::mimeData(const QModelIndexList &indexes) const
{
    TreeModelMimeData * mimeData = new TreeModelMimeData(indexes);

    QByteArray encodedData;
    QDataStream stream(&encodedData, QIODevice::WriteOnly);
    foreach (const QModelIndex &index, indexes) {
        if (index.isValid()) {
            TreeItem* item = getItem(index);
            stream << (qint32)item->id();
        }
    }
    mimeData->setData(MIME_TYPE, encodedData);
    return mimeData;
}


bool StorageTreeModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    if (action == Qt::IgnoreAction){
        return true;
    }
    TreeItem* targetNode = getItem(parent);

    qDebug("Original row = %d and col %d", row, column);
    int dstRow;
    if (row != -1)
        dstRow = row;
    else if (parent.isValid())
        dstRow = 0;
    else
        dstRow = rowCount(QModelIndex());

    const TreeModelMimeData * mimeData = qobject_cast<const TreeModelMimeData*>(data);
    if(mimeData){
        foreach (const QModelIndex &index, mimeData->indexes()) {
            if (index.isValid()) {
                TreeItem* item = getItem(index);

                if(dstRow>=0 && dstRow<targetNode->childCount()){
                    TreeItem* sibling = targetNode->child(dstRow);
                    qDebug()<<"Sibling is "<<sibling->id();
                    if(!_modelPersistence->moveBeforeNode(item, sibling)){
                        return false;
                    }
                }
                else{
                    if(!_modelPersistence->reparentNode(item, targetNode)){
                        return false;
                    }
                }
                int srcRow = index.row();
                qDebug("beginMoveRows from %d to %d", srcRow, dstRow);
                qDebug()<<"Item is "<<index.row()<<" col "<<index.column()<<" with "<<srcRow;
                qDebug()<<"Parent is "<<parent.row()<<" col "<<parent.column()<<" with "<<dstRow;
                if(!beginMoveRows(index,srcRow, srcRow,parent,dstRow)){
                    qDebug("beginMoveRows returned false");
                    return false;
                }
                if(targetNode==item->parent()){                    
                    if(srcRow<=dstRow)
                        dstRow--;
                    qDebug("Moving from %d to %d", srcRow, dstRow);
                    item->parent()->moveChild(srcRow, dstRow);
                }
                else{
                    qDebug("Reparenting node to row %d", dstRow);
                    targetNode->insertChild(dstRow,item);
                }
                endMoveRows();
            }
        }
        return true;
    }
    else if(data->hasFormat("myparts/part")){
        QByteArray encodedData = data->data("myparts/part");
        QDataStream stream(&encodedData, QIODevice::ReadOnly);
        QVector<int> parts;
        stream >> parts;
        int storageId = targetNode->id();
        qDebug()<<"Dropped parts: "<<parts.count()<<" at storage "<<storageId;
        emit partsDropped(parts, storageId);
        return true;
    }

    return false;
}
