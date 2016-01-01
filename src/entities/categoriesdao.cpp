#include "categoriesdao.h"
#include <dquest.h>
#include "entities.h"
#include "models//treeitem.h"
#include <QStack>
#include <QDebug>
#include <QSqlError>
#include <limits>

namespace Entities {

CategoriesDAO::CategoriesDAO()
{
}


typedef struct TreeItemWrapperT  {
    TreeItemWrapperT(){}
    TreeItemWrapperT(TreeItem * item, int rgt): item(item), rgt(rgt){}
    TreeItem * item;
    int rgt;
} TreeItemWrapper;

void CategoriesDAO::createCategoriesTree(TreeItem * rootItem)
{    
    QStack<TreeItemWrapper> stack;
    TreeItemWrapper rootAux(rootItem, std::numeric_limits<qint32>::max());
    stack.push(rootAux);

    DQQuery<Entities::CategoryEntity> query;
    query = query.orderBy("lft ASC");
    if(query.exec()){
        while(query.next())
        {
            Entities::CategoryEntity category = query.record();
            //Check if the current top node is parent of the new category
            if(stack.count()>0){
                while(category.rgt>stack.top().rgt){
                    stack.pop();
                }
            }

            QString name = category.name.get().toString();
            QString description = category.description.get().toString();
            int id = category.id.get().toInt();
            TreeItem * item = new TreeItem(id, name, description, stack.top().item);
            //CategoryTreeItem * item = new CategoryTreeItem(category.id, category.name, category.description, stack.top().item);
            stack.top().item->appendChild(item);
            //the category has children if the rgt-lft > 1
            if(category.rgt-category.lft>1){
                TreeItemWrapper aux(item, category.rgt);
                stack.push(aux);
            }
        }
    }
}

bool CategoriesDAO::moveNode(int nodeId, int parentId)
{
    qDebug() << "Moving part category with id "<< nodeId << " to " << parentId;
    Entities::CategoryEntity cat;
    if(!cat.load(DQWhere("id")==nodeId)){
        qWarning("PartCategory with id %d not found",nodeId);
        return false;
    }
    int newPos;
    if(parentId>=0){
        Entities::CategoryEntity parentCat;
        if(!parentCat.load(DQWhere("id")==parentId)){
            qWarning("PartCategory with id %d not found",parentId);
            return false;
        }
        newPos = parentCat.rgt;
    }
    else{
        QSqlQuery auxQuery = DQConnection::defaultConnection().query();
        auxQuery.exec("SELECT MAX(rgt) FROM category");
        if(auxQuery.next()){
            newPos = auxQuery.value(0).toInt()+1;
        }
        else{
            qWarning("Unable to find tree position for root category");
            return false;
        }
    }
    int width = (cat.rgt-cat.lft)+1;
    int distance = newPos - cat.lft;
    int tmppos = cat.lft;
    int oldPos = cat.rgt;
    if (distance < 0)
    {
        distance -= width;
        tmppos += width;
    }
    qDebug("newPos=%d, width=%d, distance=%d, tmppos=%d",newPos, width, distance, tmppos);

    DQConnection conn;
    QSqlQuery query = conn.query();
    query.prepare("UPDATE category SET lft = lft + :width WHERE lft >= :newpos");
    query.bindValue(":width",width);
    query.bindValue(":newpos",newPos);
    query.exec();
    qDebug("Changed %d rows",query.numRowsAffected());

    query.prepare("UPDATE category SET rgt = rgt + :width WHERE rgt >= :newpos");
    query.bindValue(":width",width);
    query.bindValue(":newpos",newPos);
    query.exec();
    qDebug("Changed %d rows",query.numRowsAffected());

    query.prepare("UPDATE category SET lft = lft + ?, rgt = rgt + ? WHERE lft >= ? AND rgt < ?");
    query.bindValue(0,distance);
    query.bindValue(1,distance);
    query.bindValue(2,tmppos);
    query.bindValue(3,tmppos+width);
    query.exec();
    qDebug("Changed %d rows",query.numRowsAffected());

    query.prepare("UPDATE category SET lft = lft - :width WHERE lft > :oldpos");
    query.bindValue(":width",width);
    query.bindValue(":oldpos",oldPos);
    query.exec();
    qDebug("Changed %d rows",query.numRowsAffected());

    query.prepare("UPDATE category SET rgt = rgt - :width WHERE rgt > :oldpos");
    query.bindValue(":width",width);
    query.bindValue(":oldpos",oldPos);
    query.exec();
    qDebug("Changed %d rows",query.numRowsAffected());
    return true;
}

bool CategoriesDAO::moveBeforeNode(int nodeId, int siblingId)
{
    qDebug() << "Moving part category with id "<< nodeId << " before " << siblingId;
    Entities::CategoryEntity cat;
    if(!cat.load(DQWhere("id")==nodeId)){
        qWarning("PartCategory with id %d not found",nodeId);
        return false;
    }
    int newPos;
    if(siblingId>=0){
        Entities::CategoryEntity siblingCat;
        if(!siblingCat.load(DQWhere("id")==siblingId)){
            qWarning("PartCategory with id %d not found",siblingId);
            return false;
        }
        newPos = siblingCat.lft;
    }
    else{
        QSqlQuery auxQuery = DQConnection::defaultConnection().query();
        auxQuery.exec("SELECT MAX(rgt) FROM category");
        if(auxQuery.next()){
            newPos = auxQuery.value(0).toInt()+1;
        }
        else{
            qWarning("Unable to find tree position for root category");
            return false;
        }
    }
    int width = (cat.rgt-cat.lft)+1;
    int distance = newPos - cat.lft;
    int tmppos = cat.lft;
    int oldPos = cat.rgt;
    if (distance < 0)
    {
        distance -= width;
        tmppos += width;
    }
    qDebug("newPos=%d, width=%d, distance=%d, tmppos=%d",newPos, width, distance, tmppos);

    DQConnection conn;
    QSqlQuery query = conn.query();
    query.prepare("UPDATE category SET lft = lft + :width WHERE lft >= :newpos");
    query.bindValue(":width",width);
    query.bindValue(":newpos",newPos);
    query.exec();
    qDebug("Changed %d rows",query.numRowsAffected());

    query.prepare("UPDATE category SET rgt = rgt + :width WHERE rgt >= :newpos");
    query.bindValue(":width",width);
    query.bindValue(":newpos",newPos);
    query.exec();
    qDebug("Changed %d rows",query.numRowsAffected());

    query.prepare("UPDATE category SET lft = lft + ?, rgt = rgt + ? WHERE lft >= ? AND rgt < ?");
    query.bindValue(0,distance);
    query.bindValue(1,distance);
    query.bindValue(2,tmppos);
    query.bindValue(3,tmppos+width);
    query.exec();
    qDebug("Changed %d rows",query.numRowsAffected());

    query.prepare("UPDATE category SET lft = lft - :width WHERE lft > :oldpos");
    query.bindValue(":width",width);
    query.bindValue(":oldpos",oldPos);
    query.exec();
    qDebug("Changed %d rows",query.numRowsAffected());

    query.prepare("UPDATE category SET rgt = rgt - :width WHERE rgt > :oldpos");
    query.bindValue(":width",width);
    query.bindValue(":oldpos",oldPos);
    query.exec();
    qDebug("Changed %d rows",query.numRowsAffected());
    return true;
}

bool CategoriesDAO::setCategoryName(int nodeId, const QString &name)
{
    qDebug() << "Changing name of part category with id "<< nodeId << " to " << name;
    Entities::CategoryEntity cat;
    if(cat.load(DQWhere("id")==nodeId)){
        cat.name.set(name);
        return cat.save();
    }
    qWarning("PartCategory with id %d not found",nodeId);
    return false;
}

QStringList CategoriesDAO::getPath(int nodeId)
{
    QStringList list;
    DQConnection conn;
    QSqlQuery query = conn.query();
    query.prepare("SELECT parent.name FROM category AS node, category AS parent "\
                  " WHERE node.lft BETWEEN parent.lft AND parent.rgt AND node.id = ? ORDER BY parent.lft ASC");
    query.bindValue(0,nodeId);
    if(query.exec()){
        while(query.next()){
            list.append(query.value(0).toString());
        }
    }
    else{
        qWarning()<<"Unable to find node path. Query error:" << query.lastError();
    }
   return list;
}



int CategoriesDAO::insertCategoryAtEnd(const QVector<QVariant> & data, int parentId)
{
    int newPos;
    if(parentId>=0){
        Entities::CategoryEntity parentCat;
        if(!parentCat.load(DQWhere("id")==parentId)){
            qWarning("PartCategory with id %d not found",parentId);
            return 0;
        }
        newPos = parentCat.rgt;
    }
    else{
        QSqlQuery auxQuery = DQConnection::defaultConnection().query();
        auxQuery.exec("SELECT MAX(rgt) FROM category");
        if(auxQuery.next()){
            newPos = auxQuery.value(0).toInt()+1;
        }
        else{
            qWarning("Unable to find tree position for root category");
            return 0;
        }
    }
    DQConnection conn;
    QSqlQuery query = conn.query();
    query.prepare("UPDATE category SET lft = lft+2 WHERE lft >= ?");
    query.bindValue(0,newPos);
    query.exec();
    qDebug("Changed %d rows",query.numRowsAffected());
    query.prepare("UPDATE category SET rgt = rgt+2 WHERE rgt >= ?");
    query.bindValue(0,newPos);
    query.exec();
    qDebug("Changed %d rows",query.numRowsAffected());
    Entities::CategoryEntity category;
    category.name.set(data[NAME_COL]);
    category.description.set(data[DESCRIPTION_COL]);
    category.lft = newPos;
    category.rgt = newPos+1;
    if(category.save())
        return category.id;
    return -1;
}

bool CategoriesDAO::insertAtEnd(TreeItem * item)
{
    int newPos;
    int parentId = item->parent()->id();
    if(parentId>=0){
        Entities::CategoryEntity parentCat;
        if(!parentCat.load(DQWhere("id")==parentId)){
            qWarning("PartCategory with id %d not found",parentId);
            return 0;
        }
        newPos = parentCat.rgt;        
    }
    else{
        QSqlQuery auxQuery = DQConnection::defaultConnection().query();
        auxQuery.exec("SELECT MAX(rgt) FROM category");
        if(auxQuery.next()){
            newPos = auxQuery.value(0).toInt()+1;
        }
        else{
            qWarning("Unable to find tree position for root category");
            return 0;
        }
    }
    DQConnection conn;
    QSqlQuery query = conn.query();
    query.prepare("UPDATE category SET lft = lft+2 WHERE lft >= ?");
    query.bindValue(0,newPos);
    query.exec();
    qDebug("Changed %d rows",query.numRowsAffected());
    query.prepare("UPDATE category SET rgt = rgt+2 WHERE rgt >= ?");
    query.bindValue(0,newPos);
    query.exec();
    qDebug("Changed %d rows",query.numRowsAffected());

    Entities::CategoryEntity category;
    category.name.set(item->name());
    category.description.set(item->description());
    category.lft = newPos;
    category.rgt = newPos+1;
    if(category.save()){
        item->setId(category.id);
        return true;
    }
    qWarning() << "Failed to save PartCategory "<<category.name.get();
    qWarning() << "Reason:"<<query.lastError();
    return false;
}

bool CategoriesDAO::reload(TreeItem * item)
{
    Entities::CategoryEntity cat;
    if(cat.load(DQWhere("id")==item->id())){
        item->setName(cat.name.get().toString());
        item->setDescription(cat.description.get().toString());
        return true;
    }
    qWarning("PartCategory with id %d not found",item->id());
    return false;
}

bool CategoriesDAO::update(TreeItem * item)
{
    int id = item->id();
    Entities::CategoryEntity cat;
    if(!cat.load(DQWhere("id")==id)){
        qWarning("PartCategory with id %d not found", id);
        return false;
    }
    cat.name.set(item->name());
    cat.description.set(item->description());
    return cat.save();
}

bool CategoriesDAO::remove(int nodeId, int newParent)
{
    Entities::CategoryEntity cat;
    if(!cat.load(DQWhere("id")==nodeId)){
        qWarning("PartCategory with id %d not found", nodeId);
        return false;
    }
    int width = (cat.rgt-cat.lft)+1;
    /*
    if(width>2){
        qWarning("Removing a whole subtree is not supported");
        return false;
    }
    */
    //First we replace the category of the parts belonging to this category with the parent.
    DQConnection conn;
    QSqlQuery query = conn.query();
    query.prepare("UPDATE part SET category=? WHERE category IN (SELECT id FROM category WHERE lft BETWEEN ? AND ?)");
    //query.prepare("UPDATE part SET category=? WHERE category=?");
    query.bindValue(0,newParent);
    query.bindValue(1,cat.lft.get());
    query.bindValue(2,cat.rgt.get());
    query.exec();
    qDebug("Updated %d parts",query.numRowsAffected());

    //Then we remove the category

    query.prepare("DELETE FROM category WHERE lft BETWEEN ? AND ?");
    query.bindValue(0,cat.lft.get());
    query.bindValue(1,cat.rgt.get());
    query.exec();
    qDebug("Deleted %d rows",query.numRowsAffected());

    //Finally we update the remaining tree
    query.prepare("UPDATE category SET rgt = rgt - ? WHERE rgt > ?");
    query.bindValue(0,width);
    query.bindValue(1,cat.rgt.get());
    query.exec();
    qDebug("Changed %d rows",query.numRowsAffected());

    query.prepare("UPDATE category SET lft = lft - ? WHERE lft > ?");
    query.bindValue(0,width);
    query.bindValue(1,cat.lft.get());
    query.exec();
    qDebug("Changed %d rows",query.numRowsAffected());

    return true;
}

bool CategoriesDAO::setPartCategory(const QVector<int> & parts, int parentCategoryId)
{
    DQConnection conn;
    QSqlQuery query = conn.query();
    query.prepare("UPDATE part SET category=? WHERE id=?");
    query.bindValue(0,parentCategoryId);
    int partId;
    foreach (partId, parts) {
        query.bindValue(1,partId);
        query.exec();
    }
    return true;
}

QVector<QVariant> CategoriesDAO::listChildCategories(int nodeId)
{
    QVector<QVariant> list;
    Entities::CategoryEntity cat;
    if(!cat.load(DQWhere("id")==nodeId)){
        qWarning("PartCategory with id %d not found", nodeId);
        return list;
    }
    DQConnection conn;
    QSqlQuery query = conn.query();
    query.prepare("SELECT id FROM category WHERE lft BETWEEN ? AND ? ORDER BY lft");
    query.bindValue(0,cat.lft.get());    
    query.bindValue(1,cat.rgt.get());
    query.exec();
    while(query.next()){
        list.append(query.value(0));
    }
    return list;
}

}//namespace
