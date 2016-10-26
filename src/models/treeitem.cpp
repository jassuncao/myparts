#include "treeitem.h"

TreeItem::TreeItem(const int id, const QVariant &name, const QVariant &description, const QVariant &iconName, TreeItem *parent) :
    _id(id),
    _name(name),
    _description(description),
    _iconName(iconName)
 {
     parentItem = parent;
 }

 TreeItem::~TreeItem()
 {
     qDeleteAll(childItems);
 }

 TreeItem *TreeItem::child(int number) const
 {
     return childItems.value(number);
 }

 int TreeItem::childCount() const
 {
     return childItems.count();
 }

 int TreeItem::row() const
 {
     if (parentItem)
         return parentItem->childItems.indexOf(const_cast<TreeItem*>(this));

     return 0;
 }

 void TreeItem::appendChild(TreeItem *child)
 {
     if(child->parentItem && child->parentItem!=this){
            child->parentItem->childItems.removeOne(child);
     }     
     childItems.append(child);
     child->parentItem = this;
 }

 void TreeItem::insertChild(int index, TreeItem *child)
 {
     if(child->parentItem && child->parentItem!=this){
         child->parentItem->childItems.removeOne(child);
     }
     childItems.insert(index, child);
     child->parentItem = this;
 }

 bool TreeItem::insertChildren(int position, int count)
 {
     if (position < 0 || position > childItems.size())
         return false;

     for (int row = 0; row < count; ++row) {
         TreeItem *item = new TreeItem(-1, QString(), QString(), QVariant(), this);
         childItems.insert(position, item);
     }
     return true;
 }

 TreeItem *TreeItem::parent()
 {
     return parentItem;
 }

 bool TreeItem::removeChildren(int position, int count)
 {
     if (position < 0 || position + count > childItems.size())
         return false;

     for (int row = 0; row < count; ++row)
         delete childItems.takeAt(position);

     return true;
 }

 void TreeItem::moveChild(int from, int to)
 {
     childItems.move(from,to);
 }

 void TreeItem::removeAll()
 {
     qDeleteAll(childItems);
     childItems.clear();;
 }

 void TreeItem::collectIds(QList<int> & to) const
 {
     to.append(_id);
     TreeItem* it;
     foreach (it, childItems) {
         it->collectIds(to);
     }
 }
