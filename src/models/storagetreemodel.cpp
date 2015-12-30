#include "storagetreemodel.h"
#include "models/treeitem.h"
#include "models/treemodelmimedata.h"
#include "models/treeitemmodelpersistence.h"
#include <QDebug>
#include <QSqlQuery>

const char * MIME_TYPE= "myparts/storage";

class StorageTreeItemModelPersistence : public TreeItemModelPersistence
{
public:
    explicit StorageTreeItemModelPersistence();
    virtual ~StorageTreeItemModelPersistence();
protected:
    void beforeRemoveNode(int newParentId, const Node & node) const;
};

StorageTreeItemModelPersistence::StorageTreeItemModelPersistence() :
    TreeItemModelPersistence("storage")
{
}

StorageTreeItemModelPersistence::~StorageTreeItemModelPersistence()
{
}

void StorageTreeItemModelPersistence::beforeRemoveNode(int newParentId, const Node & node) const
{
    QSqlQuery query(database());
    query.prepare("UPDATE part SET storage=? WHERE storage IN (SELECT id FROM storage WHERE lft BETWEEN ? AND ?)");
    query.bindValue(0,newParentId);
    query.bindValue(1,node.lft);
    query.bindValue(2,node.rgt);
    query.exec();
    qDebug("Updated %d parts",query.numRowsAffected());

}

StorageTreeModel::StorageTreeModel(QObject *parent) :
    TreeItemModel(new StorageTreeItemModelPersistence(), QLatin1String("myparts/storage"), parent)
{  
    QIcon folderIcon;
    folderIcon.addFile(":icons/box_closed", QSize(), QIcon::Normal, QIcon::Off);
    folderIcon.addFile(":icons/box_open", QSize(), QIcon::Normal, QIcon::On);
    setFolderIcon(folderIcon);
}

StorageTreeModel::~StorageTreeModel()
{
}
