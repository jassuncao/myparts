#include "models/treeitem.h"
#include "categorytreemodel.h"
#include "categorytreeitem.h"
#include "entities/categoriesdao.h"
#include <QDebug>
#include <QMimeData>
#include "categoymimedata.h"
#include <limits>

CategoryTreeModel::CategoryTreeModel(QObject *parent) :
    TreeItemModel(parent)
{
}

CategoryTreeModel::~CategoryTreeModel()
{
}

bool CategoryTreeModel::fillTree(TreeItem * rootItem)
{
    Entities::CategoriesDAO::createCategoriesTree(rootItem);
    return true;
}

bool CategoryTreeModel::doInsert(TreeItem * item)
{
    return Entities::CategoriesDAO::insertAtEnd(item);
}

bool CategoryTreeModel::doUpdate(TreeItem * item)
{
    return Entities::CategoriesDAO::update(item);
}

bool CategoryTreeModel::doDelete(TreeItem * item)
{
    return Entities::CategoriesDAO::remove(item->id(), item->parent()->id());
}

bool CategoryTreeModel::doRevert(TreeItem * item)
{
    return Entities::CategoriesDAO::reload(item);
}

Qt::DropActions CategoryTreeModel::supportedDropActions() const
{
    return Qt::MoveAction | Qt::LinkAction;
}

QStringList CategoryTreeModel::mimeTypes() const
{
    QStringList types;    
    types << "myparts/category" << "myparts/part";
    return types;
}

QMimeData *CategoryTreeModel::mimeData(const QModelIndexList &indexes) const
{
    CategoyMimeData * mimeData = new CategoyMimeData(indexes);

    QByteArray encodedData;
    QDataStream stream(&encodedData, QIODevice::WriteOnly);
    foreach (const QModelIndex &index, indexes) {
        if (index.isValid()) {            
            TreeItem* item = getItem(index);
            stream << (qint32)item->id();
        }
    }    
    mimeData->setData("myparts/category", encodedData);    
    return mimeData;
}


bool CategoryTreeModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
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

    const CategoyMimeData * catMimeData = qobject_cast<const CategoyMimeData*>(data);
    if(catMimeData){
        //qDebug("Droped CategoyMimeData");
        //qDebug("At row %d and beginRow %d and col %d", row, beginRow, column);
        foreach (const QModelIndex &index, catMimeData->indexes()) {
            if (index.isValid()) {
                TreeItem* item = getItem(index);


                if(dstRow>=0 && dstRow<targetNode->childCount()){
                    TreeItem* sibling = targetNode->child(dstRow);
                    qDebug()<<"Sibling is "<<sibling->id();
                    if(!Entities::CategoriesDAO::moveBeforeNode(item->id(), sibling->id())){
                        return false;
                    }
                }
                else{
                    if(!Entities::CategoriesDAO::moveNode(item->id(), targetNode->id())){
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
        Entities::CategoriesDAO::setPartCategory(parts, targetNode->id());
        qDebug()<<"Dropped parts: "<<parts.count();
        emit partDropped();
        return true;
    }

    return false;
}

