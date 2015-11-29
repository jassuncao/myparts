#ifndef TREEITEM_H
#define TREEITEM_H

#include <QList>
#include <QVariant>
#include <QVector>

class TreeItem
{
public:
    TreeItem(const int id, const QVector<QVariant> &data, TreeItem *parent = 0);
    ~TreeItem();

    int id() const { return _id;}
    void setId(int id) { _id=id;}
    TreeItem *child(int number) const;
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    bool insertChildren(int position, int count, int columns);
    bool insertColumns(int position, int columns);
    TreeItem *parent();
    bool removeChildren(int position, int count);
    bool removeColumns(int position, int columns);
    int childNumber() const;
    bool setData(int column, const QVariant &value);
    void moveChild(int from, int to);
   // void reparent(TreeItem *newParent);
    void appendChild(TreeItem *child);
    void removeAll();
    void collectIds(QList<int> &to) const;
    /*
    void insertChild(int i, TreeItem *child);
    void removeChild(TreeItem *child);
    void removeChild(int at);
    */

private:
    int _id;
    QList<TreeItem*> childItems;
    QVector<QVariant> itemData;
    TreeItem *parentItem;
};


#endif // TREEITEM_H
