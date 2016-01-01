#include "categorytreemodel.h"
#include "treeitem.h"
#include "models/treemodelmimedata.h"
#include <QVector>
#include <QMimeData>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QDebug>


class CategoryTreeItemModelPersistence : public TreeItemModelPersistence
{
public:
    explicit CategoryTreeItemModelPersistence();
    virtual ~CategoryTreeItemModelPersistence();
protected:
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

CategoryTreeModel::CategoryTreeModel(QObject *parent) :
    TreeItemModel(new CategoryTreeItemModelPersistence(), QLatin1String("myparts/category"), parent)
{   
}

CategoryTreeModel::~CategoryTreeModel()
{
}





