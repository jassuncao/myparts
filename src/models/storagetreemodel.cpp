#include "storagetreemodel.h"
#include "entities/storagedao.h"
#include "models/treeitem.h"
#include "models/treemodelmimedata.h"
#include <QDebug>

const char * MIME_TYPE= "myparts/storage";

StorageTreeModel::StorageTreeModel(TreeItem * invisibleRootItem, QObject *parent) :
    TreeItemModel(invisibleRootItem, parent)
{
    _dao = new StorageDAO();
}

StorageTreeModel::~StorageTreeModel()
{
    delete _dao;
}

bool StorageTreeModel::fillTree(TreeItem * rootItem)
{
    _dao->createTree(rootItem);
    return true;
}

bool StorageTreeModel::doInsert(TreeItem * item)
{
    return _dao->insertAtEnd(item);
}

bool StorageTreeModel::doUpdate(TreeItem * item)
{
    return _dao->update(item);
}

bool StorageTreeModel::doDelete(TreeItem * item)
{
    return _dao->remove(item->id(), item->parent()->id());
}

bool StorageTreeModel::doRevert(TreeItem * item)
{
    return _dao->reload(item);
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

    const TreeModelMimeData * catMimeData = qobject_cast<const TreeModelMimeData*>(data);
    if(catMimeData){
        //qDebug("Droped CategoyMimeData");
        //qDebug("At row %d and beginRow %d and col %d", row, beginRow, column);
        foreach (const QModelIndex &index, catMimeData->indexes()) {
            if (index.isValid()) {
                TreeItem* item = getItem(index);


                if(dstRow>=0 && dstRow<targetNode->childCount()){
                    TreeItem* sibling = targetNode->child(dstRow);
                    qDebug()<<"Sibling is "<<sibling->id();
                    if(!_dao->moveBeforeNode(item->id(), sibling->id())){
                        return false;
                    }
                }
                else{
                    if(!_dao->moveNode(item->id(), targetNode->id())){
                        return false;
                    }

                }
                int srcRow = index.row();
                if(!beginMoveRows(index,srcRow, srcRow,parent,dstRow)){
                    return false;
                }
                qDebug("beginMoveRows from %d to %d", srcRow, dstRow);
                if(targetNode==item->parent()){
                    if(srcRow<=dstRow)
                        dstRow--;
                    qDebug("Moving from %d to %d", srcRow, dstRow);
                    item->parent()->moveChild(srcRow, dstRow);
                }
                else{
                    qDebug("Reparenting node");
                    targetNode->appendChild(item);
                    /*
                    item->parent()->removeChild(item);
                    qDebug("Appending node");
                    targetNode->insertChild(dstRow,item);
                    */
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
        _dao->setPartStorage(parts, targetNode->id());
        qDebug()<<"Dropped parts: "<<parts.count();
        emit partDropped();
        return true;
    }

    return false;
}
