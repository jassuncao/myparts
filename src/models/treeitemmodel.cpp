#include "treeitemmodel.h"
#include "treeitem.h"
#include "treeitemmodelpersistence.h"
#include "treemodelmimedata.h"
#include "iconsrepository.h"
#include <QDebug>
#include <QMimeData>
#include <limits>
#include <algorithm>


const char * PART_MIME_TYPE= "myparts/part";

static const int COLUMN_COUNT = 1;

TreeItemModel::TreeItemModel(TreeItemModelPersistence *modelPersistence, const QString &mimeType, QObject *parent) :
    QAbstractItemModel(parent), _modelPersistence(modelPersistence), _mimeType(mimeType),
    _iconsRepository(0)
{
    _invisibleRootItem = new TreeItem(-1, tr("Name"), tr("Description"), QVariant());
    _defaultIcon.addFile(":icons/folder_closed", QSize(), QIcon::Normal, QIcon::Off);
    _defaultIcon.addFile(":icons/folder_open", QSize(), QIcon::Normal, QIcon::On);
}

TreeItemModel::~TreeItemModel()
{
    delete _invisibleRootItem;    
    delete _modelPersistence;
    _invisibleRootItem = NULL;
    _modelPersistence = NULL;
}

bool TreeItemModel::select()
{
    bool res;
    beginResetModel();
    _invisibleRootItem->removeAll();
    res = _modelPersistence->loadTree(_invisibleRootItem);
    endResetModel();
    return res;
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
    if(index.column()<0 || index.column()>=COLUMN_COUNT)
        return QVariant();
    TreeItem *item = getItem(index);
    switch(role){
    case Qt::DecorationRole:{
        QString iconId = item->iconId().toString();
        if(iconId.isNull() || _iconsRepository == 0){
            return _defaultIcon;
        }
        return _iconsRepository->getIcon(iconId);
    }
    case Qt::DisplayRole:
    case Qt::EditRole:
        return item->name();
    case Qt::ToolTipRole:
        return item->description();
    case TreeItemModel::FullPathRole:
        return computeFullPath(item);
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
    if(index.column()<0 || index.column()>=COLUMN_COUNT)
        return false;

    if(role!=Qt::EditRole && role!=Qt::ToolTipRole && role!=Qt::DecorationRole){
        return false;
    }
    TreeItem *item = getItem(index);
    if(role==Qt::EditRole){
        item->setName(value);
    }
    else if(role==Qt::ToolTipRole){
        item->setDescription(value);
    }
    else if(role == Qt::DecorationRole){       
        item->setIconId(value.toString());
    }
    if(!_uncommited.contains(item))
        _uncommited.append(item);
    return true;
}

int TreeItemModel::getItemId(const QModelIndex &index) const
{
    if(!index.isValid()){
        return -1;
    }
    TreeItem *item = getItem(index);
    return item->id();
}

QString TreeItemModel::getItemIconId(const QModelIndex &index) const
{
    if(!index.isValid()){
        return QString();
    }
    TreeItem *item = getItem(index);
    return item->iconId().toString();
}

bool TreeItemModel::setItemIconId(const QModelIndex &index, const QString & iconId)
{
    return setData(index, iconId, Qt::DecorationRole);
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
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole && section>=0 && section<COLUMN_COUNT){
             return _invisibleRootItem->name();
    }
    return QVariant();
}

QModelIndex TreeItemModel::index(int row, int column, const QModelIndex &parentIndex) const
{
    if(!hasIndex(row, column,parentIndex))
        return QModelIndex();
    /*
    if (parentIndex.isValid() && parentIndex.column() != 0){
      return QModelIndex();
    }
    */

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
    return createIndex(parentItem->row(), 0, parentItem);
}

int TreeItemModel::rowCount(const QModelIndex &parent) const
{
    if(parent.column()>0)
        return 0;
    TreeItem * parentItem;
    if(!parent.isValid())
        parentItem = _invisibleRootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());
    return parentItem->childCount();
    /*
    TreeItem *parentItem = getItem(parent);
    return parentItem->childCount();
    */
}

int TreeItemModel::columnCount(const QModelIndex &parent) const
{        
    if(!parent.isValid())
        return 1;
    return 2;
}

bool TreeItemModel::insertRows(int row, int count, const QModelIndex &parent)
{
    TreeItem * parentNode = getItem(parent);
    int startRow = std::min(row, parentNode->childCount());
    int lastRow = startRow + count;
    beginInsertRows(parent, startRow, lastRow);
    bool success = parentNode->insertChildren(startRow, count);
    if(success){
        for(; startRow < lastRow; ++startRow){
            TreeItem * newItem = parentNode->child(startRow);
            _uncommited.append(newItem);
        }
    }
    endInsertRows();
    return success;
}

bool TreeItemModel::removeRows(int row, int count, const QModelIndex &parent)
{
    qDebug("Remove rows called");
    return true;
}

bool TreeItemModel::insertItem(const QModelIndex &parent)
{
    TreeItem * parentNode = getItem(parent);
    int row = parentNode->childCount();
    beginInsertRows(parent,row,row);
    bool success = parentNode->insertChildren(row,1);
    TreeItem * newItem = parentNode->child(row);
    _uncommited.append(newItem);
    //_uncommitedItem= newItem;
    //_uncommitedItemParent= &parent;
    endInsertRows();
    return success;
}

bool TreeItemModel::submit()
{
    bool res = true;  
    while (!_uncommited.isEmpty()){
        TreeItem * item = _uncommited.dequeue();
        if(item->id()<0){
            qDebug("Inserting new item (n)");
            res = res | _modelPersistence->insertAtEnd(item);
        }
        else{
            qDebug("Saving changes to item (n)");
            res = res | _modelPersistence->save(item);
        }
    }
    return res;
}

void TreeItemModel::revert()
{
    qDebug("revert changes");   
    while (!_uncommited.isEmpty()){
        TreeItem * item = _uncommited.dequeue();
        if(item->id()==-1){//not saved to the database
            qDebug("Reverting unsaved item (n)");
            QModelIndex parentIndex = findIndex(item->parent()->id());
            int idx = item->row();
            beginRemoveRows(parentIndex,idx,idx);
            item->parent()->removeChildren(idx,1);
            endRemoveRows();
        }
        else{
            qDebug("Reverting item (n)");          
        }
    }
}


bool TreeItemModel::removeItem(const QModelIndex &index)
{
    QModelIndex parent = index.parent();
    TreeItem *parentItem = getItem(parent);
    int position = index.row();
    bool success = false;
    TreeItem * child = parentItem->child(position);
    if(_modelPersistence->remove(child)){
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

Qt::DropActions TreeItemModel::supportedDropActions() const
{
    return Qt::MoveAction | Qt::LinkAction;
}

QStringList TreeItemModel::mimeTypes() const
{
    QStringList types;
    types << _mimeType << PART_MIME_TYPE;
    return types;
}


QMimeData * TreeItemModel::mimeData(const QModelIndexList &indexes) const
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
    mimeData->setData(_mimeType, encodedData);
    return mimeData;
}

bool TreeItemModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
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

    const TreeModelMimeData * mimeData = dynamic_cast<const TreeModelMimeData*>(data);
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
    else if(data->hasFormat(PART_MIME_TYPE)){
        QByteArray encodedData = data->data(PART_MIME_TYPE);
        QDataStream stream(&encodedData, QIODevice::ReadOnly);
        QVector<int> parts;
        stream >> parts;
        emit partsDropped(parts, targetNode);
        return true;
    }

    return false;
}

void TreeItemModel::setIconsRepository(IconsRepository * iconsRepository)
{
    _iconsRepository = iconsRepository;
}

QStringList TreeItemModel::computeFullPath(const TreeItem * child) const
{
    QStringList path;
    const TreeItem * current = child;
    while(current){
        path.insert(0, current->name().toString());
        current = child->parent();
    }
    return path;
}

