#include "categorytreemodel2.h"
#include <QVector>
#include <QMimeData>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QDebug>
#include "treeitem.h"

class CategoryMimeData : public QMimeData
{
public:
    CategoryMimeData() : QMimeData(){}
    void setIndexes(const QList<QPersistentModelIndex> &indexes) {_indexes = indexes;}
    const QList<QPersistentModelIndex> & indexes() const {return _indexes;}
private:
    QList<QPersistentModelIndex> _indexes;
};


class CategoryTreeItemModelPersistence : public TreeItemModelPersistence
{
public:
    explicit CategoryTreeItemModelPersistence();
    virtual ~CategoryTreeItemModelPersistence();
protected:
    TreeItem * createNewItem(const QSqlRecord & record) const;
    void beforeRemoveNode(int newParentId, const Node & node) const;
};

CategoryTreeItemModelPersistence::CategoryTreeItemModelPersistence() :
    TreeItemModelPersistence("category")
{
}

CategoryTreeItemModelPersistence::~CategoryTreeItemModelPersistence()
{
}

void CategoryTreeItemModelPersistence::beforeRemoveNode(int newParentId, const Node & node) const
{
    QSqlQuery query(database());
    query.prepare("UPDATE part SET category=? WHERE category IN (SELECT id FROM category WHERE lft BETWEEN ? AND ?)");
    query.bindValue(0,newParentId);
    query.bindValue(1,node.lft);
    query.bindValue(2,node.rgt);
    query.exec();
    qDebug("Updated %d parts",query.numRowsAffected());

}

CategoryTreeModel2::CategoryTreeModel2(QObject *parent) :
    TreeItemModel(parent), _modelPersistence(new CategoryTreeItemModelPersistence())
{   
}

CategoryTreeModel2::~CategoryTreeModel2()
{
    delete _modelPersistence;
}

bool CategoryTreeModel2::fillTree(TreeItem * rootItem)
{
    return _modelPersistence->loadTree(rootItem);
}

bool CategoryTreeModel2::doInsert(TreeItem * item)
{
    return _modelPersistence->insertAtEnd(item);
}

bool CategoryTreeModel2::doUpdate(TreeItem * item)
{
    return _modelPersistence->save(item);
}

bool CategoryTreeModel2::doDelete(TreeItem * item)
{
    return _modelPersistence->remove(item);
}

bool CategoryTreeModel2::doRevert(TreeItem * item)
{
    return _modelPersistence->revert(item);
}

Qt::DropActions CategoryTreeModel2::supportedDropActions() const
{
    return Qt::MoveAction | Qt::LinkAction;
}

QStringList CategoryTreeModel2::mimeTypes() const
{
    QStringList types;
    types << "myparts/category" << "myparts/part";
    return types;
}

QMimeData *CategoryTreeModel2::mimeData(const QModelIndexList &indexes) const
{
    QList<QPersistentModelIndex> persistentIndexes;
    foreach (const QModelIndex &index, indexes) {
        persistentIndexes.append(QPersistentModelIndex(index));
    }
    CategoryMimeData * mimeData = new CategoryMimeData();
    mimeData->setIndexes(persistentIndexes);

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

bool CategoryTreeModel2::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    if (action == Qt::IgnoreAction){
        return true;
    }
    TreeItem* targetNode = getItem(parent);
    qDebug("Action = %d", action);
    qDebug("Original row = %d and col %d", row, column);
    int dstRow;
    if (row != -1)
        dstRow = row;
    else if (parent.isValid())
        dstRow = 0;
    else
        dstRow = rowCount(QModelIndex());

    const CategoryMimeData * catMimeData = dynamic_cast<const CategoryMimeData*>(data);
    if(catMimeData){
        //qDebug("Droped CategoyMimeData");
        //qDebug("At row %d and beginRow %d and col %d", row, beginRow, column);
        foreach (const QPersistentModelIndex &index, catMimeData->indexes()) {
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
                if(!beginMoveRows(index,srcRow,srcRow, parent,dstRow)){
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
        //Entities::CategoriesDAO::setPartCategory(parts, targetNode->id());
        int categoryId = targetNode->id();
        qDebug()<<"Dropped parts: "<<parts.count()<<" at category "<<categoryId;
        emit partsDropped(parts, categoryId);
        return true;
    }

    return false;
}




