#include "treeitem.h"
#include "categorytreemodel.h"
#include "categorytreeitem.h"
#include "entities/categoriesdao.h"
#include <QDebug>
#include <QMimeData>
#include "categoymimedata.h"
#include <limits>

CategoryTreeModel::CategoryTreeModel(QObject *parent) :
    QAbstractItemModel(parent)
{
    _uncommitedItem=0;
    _uncommitedItemParent=0;
    QVector<QVariant> headerData(3);
    headerData.append("Name");
    headerData.append("Description");
    headerData.append("ID");
    _invisibleRootItem = new TreeItem(-1,headerData);
    Entities::CategoriesDAO::createCategoriesTree(_invisibleRootItem);
    _folderIcon.addFile(":icons/box_closed", QSize(), QIcon::Normal, QIcon::Off);
    _folderIcon.addFile(":icons/box_open", QSize(), QIcon::Normal, QIcon::On);
}

CategoryTreeModel::~CategoryTreeModel()
{
    delete _invisibleRootItem;
}

TreeItem * CategoryTreeModel::getItem(const QModelIndex &index) const
{
    if (index.isValid()) {
        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
        if (item) return item;
    }
    return _invisibleRootItem;
}

QVariant CategoryTreeModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid()){
        return QVariant();
    }
    TreeItem *item = getItem(index);
    switch(role){
    case Qt::DecorationRole:
            return _folderIcon;
        break;            
    case Qt::DisplayRole:
    case Qt::EditRole:
            return item->data(index.column());
        break;
    case Qt::ToolTipRole:
            return item->data(Entities::CategoriesDAO::DESCRIPTION_COL);
        break;
    default:
        return QVariant();
        break;
    }
}

bool CategoryTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!index.isValid()){
        return false;
    }
    if(role!=Qt::EditRole){
        return false;
    }
    TreeItem *item = getItem(index);   
    bool res = item->setData(index.column(), value);
    if(res){
        if(!_uncommited.contains(item)){
            _uncommited.enqueue(item);
        }
        emit dataChanged(index,index);
    }
    return res;
    /*
    QString catName = value.toString();
    if(!Entities::CategoriesDAO::setCategoryName(item->id(), catName)){
        return false;
    }    
    item->setName(catName);
    return true;
    */
}

int CategoryTreeModel::getItemId(const QModelIndex &index) const
{
    if(!index.isValid()){
        return -1;
    }
    TreeItem *item = getItem(index);
    return item->id();
}

int CategoryTreeModel::rootCategory() const
{
    return _invisibleRootItem->child(0)->id();
}

Qt::ItemFlags CategoryTreeModel::flags(const QModelIndex &index) const
{
    if(!index.isValid()){
        return Qt::NoItemFlags;
    }
    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDropEnabled | Qt::ItemIsEditable;
    if(item->parent()!=_invisibleRootItem){
        flags = flags |  Qt::ItemIsDragEnabled;
    }
    return flags;
}

QVariant CategoryTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole){
             return _invisibleRootItem->data(section);
    }
    return QVariant();
}

QModelIndex CategoryTreeModel::index(int row, int column, const QModelIndex &parentIndex) const
{
    if (parentIndex.isValid() && parentIndex.column() != 0){
      return QModelIndex();
    }

     TreeItem *parentItem = getItem(parentIndex);
     TreeItem *childItem = parentItem->child(row);
     if (childItem){
         return createIndex(row, column, childItem);
     }
     else{
         return QModelIndex();
     }

     /*
     if (!hasIndex(row, column, parentIndex)){
         return QModelIndex();
     }
     if (!parent.isValid()){
         parentItem = _invisibleRootItem;
     }
     else{
         parentItem = static_cast<CategoryTreeItem*>(parent.internalPointer());
     }

     CategoryTreeItem *childItem = parentItem->child(row);
     if (childItem){
         return createIndex(row, column, childItem);
     }
     else{
         return QModelIndex();
     }
     */
}

QModelIndex CategoryTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid()){
         return QModelIndex();
    }

    TreeItem *childItem = getItem(index);
    TreeItem *parentItem = childItem->parent();

    if (parentItem == _invisibleRootItem){
        return QModelIndex();
    }
    return createIndex(parentItem->childNumber(), 0, parentItem);
}

int CategoryTreeModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem = getItem(parent);
    return parentItem->childCount();
}

int CategoryTreeModel::columnCount(const QModelIndex &parent) const
{  
    return 1;
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

/*
QModelIndex CategoryTreeModel::insertCategory(const QString &name, const QString &description, const QModelIndex &parent)
{
    TreeItem * parentNode = getItem(parent);
    int row = parentNode->childCount();
    beginInsertRows(parent,row,row);
    CategoryTreeItem * child = Entities::CategoriesDAO::insertCategoryAtEnd(name, description, parentNode->id());
    parentNode->appendChild(child);
    endInsertRows();
    return createIndex(row,0, child);
}
*/

bool CategoryTreeModel::insertCategory(const QModelIndex &parent)
{
    TreeItem * parentNode = getItem(parent);
    int row = parentNode->childCount();
    beginInsertRows(parent,row,row);
    bool success = parentNode->insertChildren(row,1, 3);
    TreeItem * newItem = parentNode->child(row);
    _uncommitedItem= newItem;
    _uncommitedItemParent= &parent;
    //_uncommited.enqueue(parentNode->child(row));
    endInsertRows();    
    return success;
}

bool CategoryTreeModel::submit()
{    
    if(_uncommitedItem){
        if(_uncommitedItem->id()<0){
            qDebug("Inserting new item");
             Entities::CategoriesDAO::insertAtEnd(_uncommitedItem);
        }
        else{
            qDebug("Saving changes to item");
            Entities::CategoriesDAO::update(_uncommitedItem);
        }
        _uncommitedItem=0;
        _uncommitedItemParent=0;
    }
    while (!_uncommited.isEmpty()){
        TreeItem * item = _uncommited.dequeue();
        if(item->id()<0){
            qDebug("Inserting new item");
             Entities::CategoriesDAO::insertAtEnd(item);
        }
        else{
            qDebug("Saving changes to item");
            Entities::CategoriesDAO::update(item);
        }
    }
    return true;
}

void CategoryTreeModel::revert()
{
    qDebug("revert changes");
    if(_uncommitedItem){
        if(_uncommitedItem->id()==-1){//not saved to the database
            int idx = _uncommitedItem->childNumber();
            beginRemoveRows(*_uncommitedItemParent,idx,idx);
            _uncommitedItem->parent()->removeChildren(_uncommitedItem->childNumber(),1);
            endRemoveRows();
        }
        else{
            Entities::CategoriesDAO::reload(_uncommitedItem);
        }
        _uncommitedItem=0;
        _uncommitedItemParent=0;
    }
    if(_uncommited.isEmpty())
        return;

    while (!_uncommited.isEmpty()){
        TreeItem * item = _uncommited.dequeue();
        if(item->id()==-1){//not saved to the database
            int idx = item->childNumber();
            item->parent()->removeChildren(item->childNumber(),1);            
        }
        else{
            Entities::CategoriesDAO::reload(item);                                    
        }
    }    
}


bool CategoryTreeModel::removeCategory(const QModelIndex &index)
{
    QModelIndex parent = index.parent();
    TreeItem *parentItem = getItem(parent);
    int position = index.row();
    bool success = false;
    if(Entities::CategoriesDAO::remove(parentItem->child(position)->id(), parentItem->id())){
        beginRemoveRows(parent, position, position);
        success = parentItem->removeChildren(position, 1);
        endRemoveRows();
    }
    return success;
}


/*
bool CategoryTreeModel::insertRows(int row, int count, const QModelIndex &parent)
{
    qDebug()<<"Insert rows "<<row << " "<<count;
    if(parent.isValid()){
         qDebug()<<static_cast<CategoryTreeItem*>(parent.internalPointer())->name();
    }
    return QAbstractItemModel::insertRows(row, count, parent);
}

bool CategoryTreeModel::removeRows(int row, int count, const QModelIndex &parent)
{
    qDebug()<<"Remove rows "<<row << " "<<count;

    if(parent.isValid()){
         qDebug()<<static_cast<CategoryTreeItem*>(parent.internalPointer())->data(0);
         CategoryTreeItem * parentItem = static_cast<CategoryTreeItem*>(parent.internalPointer());
         qDebug()<<"Data:"<<parentItem->child(row)->data(0);
    }    
    return QAbstractItemModel::removeRows(row, count, parent);
}
*/

/*
bool CategoryTreeModel::beginMoveRows(const QModelIndex &sourceParent, int sourceFirst, int sourceLast, const QModelIndex &destinationParent, int destinationRow)
{
    qDebug()<<"beginMoveRows rows "<<sourceFirst << " "<<sourceLast;
 return QAbstractItemModel::beginMoveRows(sourceParent, sourceFirst, sourceLast, destinationParent, destinationRow);
}
*/
