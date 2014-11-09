#include "categorytreeitem.h"

CategoryTreeItem::CategoryTreeItem(const int id, const QString name, const QString description, CategoryTreeItem *parent)
{
    _parent = parent;
    _name = name;
    _description = description;
    _id = id;   
}

CategoryTreeItem::~CategoryTreeItem()
{
     qDeleteAll(_childs);
}

void CategoryTreeItem::appendChild(CategoryTreeItem *child)
{
    _childs.append(child);
    child->_parent = this;
}

void CategoryTreeItem::insertChild(int i, CategoryTreeItem *child)
{
    _childs.insert(i,child);
    if(child->_parent!=this){
        child->_parent = this;
    }
}

void CategoryTreeItem::removeChild(CategoryTreeItem *child)
{
    if(child->_parent==this){
        _childs.removeOne(child);
        child->_parent = 0;
    }
}

void CategoryTreeItem::removeChild(int at)
{
    _childs.removeAt(at);
}

void CategoryTreeItem::moveChild(int from, int to)
{
    _childs.move(from,to);
}

CategoryTreeItem * CategoryTreeItem::child(int row)
{
    return _childs.value(row);
}

int CategoryTreeItem::childCount() const
{
    return _childs.count();
}

int CategoryTreeItem::row() const
{
    if(_parent){
        return _parent->_childs.indexOf(const_cast<CategoryTreeItem*>(this));
    }
    return 0;
}

CategoryTreeItem * CategoryTreeItem::parent()
{
    return _parent;
}
