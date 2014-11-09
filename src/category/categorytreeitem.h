#ifndef CATEGORYTREEITEM_H
#define CATEGORYTREEITEM_H

#include <QList>
#include <QString>

class CategoryTreeItem
{
public:
    CategoryTreeItem(const int id, const QString name, const QString description = QString(), CategoryTreeItem *parent=0);
    ~CategoryTreeItem();
    void appendChild(CategoryTreeItem *child);
    void insertChild(int i, CategoryTreeItem *child);
    void removeChild(CategoryTreeItem *child);
    void removeChild(int at);
    void moveChild(int from, int to);
    CategoryTreeItem * child(int row);
    int childCount() const;
    int columnCount() const;
    QString data(int column) const;
    int row() const;
    CategoryTreeItem * parent();
    int id() const {return _id;}    
    QString name() {return _name;}
    void setName(const QString name) { _name = name;}
    QString description() {return _description;}
    void setDescription(QString description){_description = description;}
private:
    CategoryTreeItem * _parent;
    QList<CategoryTreeItem*> _childs;
    QString _name;
    QString _description;
    int _id;

};

#endif // CATEGORYTREEITEM_H
