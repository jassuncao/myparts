#include "treeitemmodel.h"
#include "treeitem.h"
#include <QDebug>
#include <QMimeData>
#include <limits>


TreeItemModel::TreeItemModel(int columns, QObject *parent) :
    QAbstractItemModel(parent), _toolTipColumn(-1)
{
    _invisibleRootItem = new TreeItem(-1, QVector<QVariant>(columns));
    _uncommitedItem=0;
    _uncommitedItemParent=0;
    _folderIcon.addFile(":icons/folder_closed", QSize(), QIcon::Normal, QIcon::Off);
    _folderIcon.addFile(":icons/folder_open", QSize(), QIcon::Normal, QIcon::On);
}

TreeItemModel::~TreeItemModel()
{
    delete _invisibleRootItem;
    _invisibleRootItem = NULL;
}

bool TreeItemModel::select()
{
    bool res;
    beginResetModel();
    _invisibleRootItem->removeAll();
    res = fillTree(_invisibleRootItem);
    endResetModel();
    return res;
}

void TreeItemModel::setToolTipColumn(int column)
{
    _toolTipColumn = column;
}

TreeItem * TreeItemModel::getItem(const QModelIndex &index) const
{
    if (index.isValid()) {
        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
        if (item) return item;
    }
    return _invisibleRootItem;
}

QVariant TreeItemModel::data(const QModelIndex &index, int role) const
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
        if(_toolTipColumn!=-1)
            return item->data(_toolTipColumn);       
        return QVariant();
    default:
        return QVariant();
        break;
    }
}

bool TreeItemModel::setData(const QModelIndex &index, const QVariant &value, int role)
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
}

int TreeItemModel::getItemId(const QModelIndex &index) const
{
    if(!index.isValid()){
        return -1;
    }
    TreeItem *item = getItem(index);
    return item->id();
}

QList<int> TreeItemModel::getSubTreeIds(const QModelIndex &index) const
{
    QList<int> list;
    if(index.isValid()){
        TreeItem *item = getItem(index);
        if(item)
            item->collectIds(list);
    }
    return list;
}

int TreeItemModel::rootItemId() const
{
    return _invisibleRootItem->child(0)->id();
}

Qt::ItemFlags TreeItemModel::flags(const QModelIndex &index) const
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

QVariant TreeItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole){
             return _invisibleRootItem->data(section);
    }
    return QVariant();
}

QModelIndex TreeItemModel::index(int row, int column, const QModelIndex &parentIndex) const
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
}

QModelIndex TreeItemModel::parent(const QModelIndex &index) const
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

int TreeItemModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem = getItem(parent);
    return parentItem->childCount();
}

int TreeItemModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

bool TreeItemModel::insertItem(const QModelIndex &parent)
{
    TreeItem * parentNode = getItem(parent);
    int row = parentNode->childCount();
    beginInsertRows(parent,row,row);
    bool success = parentNode->insertChildren(row,1, 3);
    TreeItem * newItem = parentNode->child(row);
    _uncommitedItem= newItem;
    _uncommitedItemParent= &parent;
    endInsertRows();
    return success;
}

bool TreeItemModel::doInsert(TreeItem * item)
{
    return true;
}

bool TreeItemModel::doUpdate(TreeItem * item)
{
    return true;
}

bool TreeItemModel::doDelete(TreeItem * item)
{
    return true;
}

bool TreeItemModel::doRevert(TreeItem * item)
{
    return true;
}

bool TreeItemModel::submit()
{
    bool res = true;
    if(_uncommitedItem){
        if(_uncommitedItem->id()<0){
            qDebug("Inserting new item (1)");
            res = doInsert(_uncommitedItem);
        }
        else{
            qDebug("Saving changes to item (1)");
            res = doUpdate(_uncommitedItem);
        }
        _uncommitedItem=0;
        _uncommitedItemParent=0;
    }
    while (!_uncommited.isEmpty()){
        TreeItem * item = _uncommited.dequeue();
        if(item->id()<0){
            qDebug("Inserting new item (n)");
            res = res | doInsert(item);
        }
        else{
            qDebug("Saving changes to item (n)");
            res = res | doUpdate(item);
        }
    }
    return res;
}

void TreeItemModel::revert()
{
    qDebug("revert changes");
    if(_uncommitedItem){        
        if(_uncommitedItem->id()==-1){//not saved to the database
            qDebug("Reverting unsaved item (1)");
            int idx = _uncommitedItem->childNumber();
            beginRemoveRows(*_uncommitedItemParent,idx,idx);
            _uncommitedItem->parent()->removeChildren(_uncommitedItem->childNumber(),1);
            endRemoveRows();
        }
        else{
            qDebug("Reverting item (1)");
            doRevert(_uncommitedItem);
        }
        _uncommitedItem=0;
        _uncommitedItemParent=0;
    }
    if(_uncommited.isEmpty())
        return;

    while (!_uncommited.isEmpty()){
        TreeItem * item = _uncommited.dequeue();
        if(item->id()==-1){//not saved to the database
            qDebug("Reverting unsaved item (n)");
            int idx = item->childNumber();
            item->parent()->removeChildren(idx,1);
        }
        else{
            qDebug("Reverting item (n)");
            doRevert(_uncommitedItem);
        }
    }
}


bool TreeItemModel::removeItem(const QModelIndex &index)
{
    QModelIndex parent = index.parent();
    TreeItem *parentItem = getItem(parent);
    int position = index.row();
    bool success = false;
    if(doDelete(parentItem->child(position))){
        beginRemoveRows(parent, position, position);
        success = parentItem->removeChildren(position, 1);
        endRemoveRows();
    }
    return success;
}

QModelIndex TreeItemModel::findIndex(int nodeId) const
{    
    return internalFindIndex(nodeId, _invisibleRootItem);
}

QModelIndex TreeItemModel::internalFindIndex(int nodeId, const TreeItem * parentNode) const
{   
    //Check if any of the childs match
    for(int row=0; row<parentNode->childCount(); ++row){
        TreeItem *child = parentNode->child(row);
        if(!child){
            return QModelIndex();
        }
        if(child->id()==nodeId){
         return createIndex(row, 0, child);
        }
        QModelIndex res = internalFindIndex(nodeId,child);
        if(res.isValid())
            return res;
    }
    return QModelIndex();
}

