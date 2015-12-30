#ifndef TREEITEM_H
#define TREEITEM_H

#include <QList>
#include <QVariant>
#include <QVector>

class TreeItem
{
public:
    TreeItem(const int id, const QVariant & name, const QVariant & description, TreeItem *parent = 0);
    ~TreeItem();

    int id() const { return _id;}
    void setId(int id) { _id=id;}
    QVariant name() const {return _name;}
    void setName(const QVariant & name) { _name = name; }
    QVariant description() const { return _description; }
    void setDescription(const QVariant & description) { _description = description;}
    TreeItem *child(int number) const;
    int childCount() const;    
    bool insertChildren(int position, int count);
    TreeItem *parent();
    bool removeChildren(int position, int count);    
    int row() const;    
    void moveChild(int from, int to);
    void appendChild(TreeItem *child);
    void insertChild(int index, TreeItem *child);
    void removeAll();
    void collectIds(QList<int> &to) const;             
private:
    int _id;
    QVariant _name;
    QVariant _description;
    QList<TreeItem*> childItems;    
    TreeItem *parentItem;
};


#endif // TREEITEM_H
