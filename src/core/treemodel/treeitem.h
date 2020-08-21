#ifndef TREEITEM_H
#define TREEITEM_H

#include <QList>
#include <QVariant>
#include <QVector>
#include <QIcon>

class TreeItem
{
public:
    TreeItem(const int id, const QVariant & name, const QVariant & description, const QVariant &iconId, TreeItem *parent = 0);
    ~TreeItem();

    inline int id() const { return _id;}
    inline void setId(int id) { _id=id;}
    inline QVariant name() const {return _name;}
    inline void setName(const QVariant & name) { _name = name; }
    inline QVariant description() const { return _description; }
    inline void setDescription(const QVariant & description) { _description = description;}
    inline QVariant iconId() const { return _iconId;}
    void setIconId(const QVariant & iconId) { _iconId = iconId;}
    TreeItem *child(int number) const;
    int childCount() const;    
    bool insertChildren(int position, int count);
    TreeItem *parent();
    const TreeItem * parent() const;
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
    QVariant _iconId;
    QList<TreeItem*> childItems;    
    TreeItem *parentItem;
};


#endif // TREEITEM_H
