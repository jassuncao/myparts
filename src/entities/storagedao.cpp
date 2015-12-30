#include "storagedao.h"
#include <dquest.h>
#include "entities.h"
#include "models//treeitem.h"
#include <QStack>
#include <QDebug>
#include <QSqlError>
#include <limits>
#include <QSqlDatabase>

StorageDAO::StorageDAO() :
    _tableName("storage")
{
}

typedef struct TreeItemWrapperT  {
    TreeItemWrapperT(){}
    TreeItemWrapperT(TreeItem * item, int rgt): item(item), rgt(rgt){}
    TreeItem * item;
    int rgt;
} TreeItemWrapper;

typedef struct NodeT  {
    NodeT(){}
    NodeT(int id, int lft, int rgt, const QString name)
        : id(id),
          lft(lft),
          rgt(rgt),
          name(name)
    {}

    int id;
    int lft;
    int rgt;
    QString name;
} Node;

bool StorageDAO::loadNode(int nodeId, NodeT *dest)
{
    QString queryText = QString("SELECT lft, rgt, name FROM %1 WHERE id = ?").arg(_tableName);
    QSqlQuery query = DQConnection::defaultConnection().query();
    query.prepare(queryText);
    query.bindValue(0, nodeId);
    if(!query.exec()){
        qWarning() << "Failed to execute query:" << query.lastError().driverText();
        return false;
    }
    if(!query.next()){
        qWarning("Entity with id %d not found",nodeId);
        return false;
    }
    dest->id = nodeId;
    dest->lft = query.value(0).toInt();
    dest->rgt = query.value(1).toInt();
    dest->name = query.value(2).toString();
    return true;

}

void StorageDAO::createTree(TreeItem * rootItem)
{
    QSqlQuery query = DQConnection::defaultConnection().query();
    QStack<TreeItemWrapper> stack;
    TreeItemWrapper rootAux(rootItem, std::numeric_limits<qint32>::max());
    stack.push(rootAux);
    QString queryText = QString("SELECT id, name, description, lft, rgt FROM %1 ORDER BY lft ASC").arg(_tableName);
    if(query.exec(queryText)){
        while(query.next())
        {
            QVariant id = query.value(0);
            QVariant name = query.value(1);
            QVariant description = query.value(2);
            int lft = query.value(3).toInt();
            int rgt = query.value(4).toInt();
            //Check if the current top node is parent of the new entity
            if(stack.count()>0){
                while(rgt>stack.top().rgt){
                    stack.pop();
                }
            }          
            qDebug()<<"Loaded item "<<name;
            TreeItem * item = new TreeItem(id.toInt(), name, description, stack.top().item);
            stack.top().item->appendChild(item);
            //the node has children if the rgt-lft > 1
            if(rgt-lft>1){
                TreeItemWrapper aux(item, rgt);
                stack.push(aux);
            }
        }
    }
}


bool StorageDAO::moveNode(int nodeId, int parentId)
{
    qDebug() << "Moving entity with id "<< nodeId << " to " << parentId;
    /*
    QString queryText = QString("SELECT id, lft, rgt FROM %1 WHERE id = ?").arg(_tableName);
    QSqlQuery query = DQConnection::defaultConnection().query();
    query.prepare(queryText);
    query.bindValue(0, nodeId);
    if(!query.exec()){
        qWarning() << "Failed to execute query:" << query.lastError().driverText();
        return false;
    }
    if(!query.next()){
        qWarning("Entity with id %d not found",nodeId);
        return false;
    }
    int nodeLft = query.value(1).toInt();
    int nodeRgt = query.value(2).toInt();
    */
    Node node;
    if(!loadNode(nodeId, &node)){
        return false;
    }
    int newPos;
    if(parentId>=0){
        Node parentNode;
        if(!loadNode(parentId, &parentNode)){
            return false;
        }
        newPos = parentNode.rgt;
    }
    else{
        QSqlQuery auxQuery = DQConnection::defaultConnection().query();
        auxQuery.exec(QString("SELECT MAX(rgt) FROM %1 ").arg(_tableName));
        if(auxQuery.next()){
            newPos = auxQuery.value(0).toInt()+1;
        }
        else{
            qWarning("Unable to find tree position for root entity");
            return false;
        }
    }
    int width = (node.rgt-node.lft)+1;
    int distance = newPos - node.lft;
    int tmppos = node.lft;
    int oldPos = node.rgt;
    if (distance < 0)
    {
        distance -= width;
        tmppos += width;
    }
    qDebug("newPos=%d, width=%d, distance=%d, tmppos=%d",newPos, width, distance, tmppos);

    QSqlQuery query = DQConnection::defaultConnection().query();
    query.prepare(QString("UPDATE %1 SET lft = lft + :width WHERE lft >= :newpos").arg(_tableName));
    query.bindValue(":width",width);
    query.bindValue(":newpos",newPos);
    query.exec();
    qDebug("Changed %d rows",query.numRowsAffected());

    query.prepare(QString("UPDATE %1 SET rgt = rgt + :width WHERE rgt >= :newpos").arg(_tableName));
    query.bindValue(":width",width);
    query.bindValue(":newpos",newPos);
    query.exec();
    qDebug("Changed %d rows",query.numRowsAffected());

    query.prepare(QString("UPDATE %1 SET lft = lft + ?, rgt = rgt + ? WHERE lft >= ? AND rgt < ?").arg(_tableName));
    query.bindValue(0,distance);
    query.bindValue(1,distance);
    query.bindValue(2,tmppos);
    query.bindValue(3,tmppos+width);
    query.exec();
    qDebug("Changed %d rows",query.numRowsAffected());

    query.prepare(QString("UPDATE %1 SET lft = lft - :width WHERE lft > :oldpos").arg(_tableName));
    query.bindValue(":width",width);
    query.bindValue(":oldpos",oldPos);
    query.exec();
    qDebug("Changed %d rows",query.numRowsAffected());

    query.prepare(QString("UPDATE %1 SET rgt = rgt - :width WHERE rgt > :oldpos").arg(_tableName));
    query.bindValue(":width",width);
    query.bindValue(":oldpos",oldPos);
    query.exec();
    qDebug("Changed %d rows",query.numRowsAffected());
    return true;
}

bool StorageDAO::moveBeforeNode(int nodeId, int siblingId)
{
    qDebug() << "Moving part category with id "<< nodeId << " before " << siblingId;
    Node node;
    if(!loadNode(nodeId, &node)){
        qWarning("Node with id %d not found",nodeId);
        return false;
    }

    int newPos;
    if(siblingId>=0){
        Node sibling;
        if(!loadNode(siblingId, &sibling)){
            qWarning("Sibling node with id %d not found",nodeId);
            return false;
        }
        newPos = sibling.lft;
    }
    else{
        QSqlQuery auxQuery = DQConnection::defaultConnection().query();
        auxQuery.exec(QString("SELECT MAX(rgt) FROM %1 ").arg(_tableName));
        if(auxQuery.next()){
            newPos = auxQuery.value(0).toInt()+1;
        }
        else{
            qWarning("Unable to find tree position for root category");
            return false;
        }
    }
    int width = (node.rgt-node.lft)+1;
    int distance = newPos - node.lft;
    int tmppos = node.lft;
    int oldPos = node.rgt;
    if (distance < 0)
    {
        distance -= width;
        tmppos += width;
    }
    qDebug("newPos=%d, width=%d, distance=%d, tmppos=%d",newPos, width, distance, tmppos);

    DQConnection conn;
    QSqlQuery query = conn.query();
    query.prepare(QString("UPDATE %1 SET lft = lft + :width WHERE lft >= :newpos").arg(_tableName));
    query.bindValue(":width",width);
    query.bindValue(":newpos",newPos);
    query.exec();
    qDebug("Changed %d rows",query.numRowsAffected());

    query.prepare(QString("UPDATE %1 SET rgt = rgt + :width WHERE rgt >= :newpos").arg(_tableName));
    query.bindValue(":width",width);
    query.bindValue(":newpos",newPos);
    query.exec();
    qDebug("Changed %d rows",query.numRowsAffected());

    query.prepare(QString("UPDATE %1 SET lft = lft + ?, rgt = rgt + ? WHERE lft >= ? AND rgt < ?").arg(_tableName));
    query.bindValue(0,distance);
    query.bindValue(1,distance);
    query.bindValue(2,tmppos);
    query.bindValue(3,tmppos+width);
    query.exec();
    qDebug("Changed %d rows",query.numRowsAffected());

    query.prepare(QString("UPDATE %1 SET lft = lft - :width WHERE lft > :oldpos").arg(_tableName));
    query.bindValue(":width",width);
    query.bindValue(":oldpos",oldPos);
    query.exec();
    qDebug("Changed %d rows",query.numRowsAffected());

    query.prepare(QString("UPDATE %1 SET rgt = rgt - :width WHERE rgt > :oldpos").arg(_tableName));
    query.bindValue(":width",width);
    query.bindValue(":oldpos",oldPos);
    query.exec();
    qDebug("Changed %d rows",query.numRowsAffected());
    return true;
}

QStringList StorageDAO::getPath(int nodeId)
{
    QStringList list;
    DQConnection conn;
    QSqlQuery query = conn.query();
    query.prepare(QString("SELECT parent.name FROM %1 AS node, %1 AS parent "\
                          " WHERE node.lft BETWEEN parent.lft AND parent.rgt AND node.id = ? ORDER BY parent.lft ASC").arg(_tableName));
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

int StorageDAO::insertAtEnd(const QVector<QVariant> & data, int parentId)
{
    int newPos;

    if(parentId>=0){
        Node parentNode;
        if(!loadNode(parentId, &parentNode)){
            return 0;
        }
        newPos = parentNode.rgt;
    }
    else{
        QSqlQuery auxQuery = DQConnection::defaultConnection().query();
        auxQuery.exec(QString("SELECT MAX(rgt) FROM %1 ").arg(_tableName));
        if(auxQuery.next()){
            newPos = auxQuery.value(0).toInt()+1;
        }
        else{
            qWarning("Unable to find tree position for root entity");
            return 0;
        }
    }

    DQConnection conn;
    QSqlQuery query = conn.query();
    query.prepare(QString("UPDATE %1 SET lft = lft+2 WHERE lft >= ?").arg(_tableName));
    query.bindValue(0,newPos);
    query.exec();
    qDebug("Changed %d rows",query.numRowsAffected());
    query.prepare(QString("UPDATE %1 SET rgt = rgt+2 WHERE rgt >= ?").arg(_tableName));
    query.bindValue(0,newPos);
    query.exec();
    qDebug("Changed %d rows",query.numRowsAffected());

    query.prepare(QString("INSERT INTO %1 (name, lft, rgt) VALUES (?,?,?)").arg(_tableName));
    query.bindValue(0,data[NAME_COL]);
    query.bindValue(1,newPos);
    query.bindValue(2,newPos+1);
    if(query.exec()){
        return query.lastInsertId().toInt();
    }
    qWarning() << "Failed to save Node. Reason: "<<query.lastError();
    return -1;
}

bool StorageDAO::insertAtEnd(TreeItem * item)
{
    int newPos;
    int parentId = item->parent()->id();
    if(parentId>=0){
        Node parentNode;
        if(!loadNode(parentId, &parentNode)){
            return 0;
        }
        newPos = parentNode.rgt;
    }
    else{
        QSqlQuery auxQuery = DQConnection::defaultConnection().query();
        auxQuery.exec(QString("SELECT MAX(rgt) FROM %1 ").arg(_tableName));
        if(auxQuery.next()){
            newPos = auxQuery.value(0).toInt()+1;
        }
        else{
            qWarning("Unable to find tree position for root entity");
            return 0;
        }
    }

    DQConnection conn;
    QSqlQuery query = conn.query();
    query.prepare(QString("UPDATE %1 SET lft = lft+2 WHERE lft >= ?").arg(_tableName));
    query.bindValue(0,newPos);
    query.exec();
    qDebug("Changed %d rows",query.numRowsAffected());
    query.prepare(QString("UPDATE %1 SET rgt = rgt+2 WHERE rgt >= ?").arg(_tableName));
    query.bindValue(0,newPos);
    query.exec();
    qDebug("Changed %d rows",query.numRowsAffected());

    query.prepare(QString("INSERT INTO %1 (name, lft, rgt) VALUES (?,?,?)").arg(_tableName));
    query.bindValue(0,item->name());
    query.bindValue(1,newPos);
    query.bindValue(2,newPos+1);
    if(query.exec()){
        int id = query.lastInsertId().toInt();
        item->setId(id);
        return true;
    }
    qWarning() << "Failed to save Node. Reason: "<<query.lastError();
    return -1;
}

bool StorageDAO::reload(TreeItem * item)
{
    Node node;
    if(!loadNode(item->id(), &node)){
        qWarning("Entity with id %d not found",item->id());
        return false;
    }
    item->setName(node.name);
    return true;
}

bool StorageDAO::update(TreeItem * item)
{
    int id = item->id();
    Entities::StorageEntity entity;
    if(!entity.load(DQWhere("id")==id)){
        qWarning("Entity with id %d not found", id);
        return false;
    }
    entity.name.set(item->name());
    return entity.save();
}

bool StorageDAO::remove(int nodeId, int newParent)
{
    Node node;
    if(!loadNode(nodeId,&node)) {
        qWarning("Entity with id %d not found", nodeId);
        return false;
    }


    int width = (node.rgt-node.lft)+1;
    if(width>2){
        qWarning("Removing a whole subtree is not supported");
        return false;
    }
    //First we replace the category of the parts belonging to this category with the parent.
    DQConnection conn;
    QSqlQuery query = conn.query();
    query.prepare("UPDATE part SET storage=? WHERE category=?");
    query.bindValue(0,newParent);
    query.bindValue(1,nodeId);
    query.exec();
    qDebug("Updated %d parts",query.numRowsAffected());

    //Then we remove the category

    query.prepare(QString("DELETE FROM %1 WHERE lft BETWEEN ? AND ?").arg(_tableName));
    query.bindValue(0,node.lft);
    query.bindValue(1,node.rgt);
    query.exec();
    qDebug("Deleted %d rows",query.numRowsAffected());

    //Finally we update the remaining tree
    query.prepare(QString("UPDATE %1 SET rgt = rgt - ? WHERE rgt > ?").arg(_tableName));
    query.bindValue(0,width);
    query.bindValue(1,node.rgt);
    query.exec();
    qDebug("Changed %d rows",query.numRowsAffected());

    query.prepare(QString("UPDATE %1 SET lft = lft - ? WHERE lft > ?").arg(_tableName));
    query.bindValue(0,width);
    query.bindValue(1,node.lft);
    query.exec();
    qDebug("Changed %d rows",query.numRowsAffected());

    return true;
}

bool StorageDAO::setPartStorage(const QVector<int> & parts, int parentStorageId)
{
    DQConnection conn;
    QSqlQuery query = conn.query();
    query.prepare("UPDATE part SET storage=? WHERE id=?");
    query.bindValue(0,parentStorageId);
    int partId;
    foreach (partId, parts) {
        query.bindValue(1,partId);
        query.exec();
    }
    return true;
}

QVector<QVariant> StorageDAO::listChildStorage(int nodeId)
{
    QVector<QVariant> list;
    Node node;
    if(!loadNode(nodeId, &node)){
        qWarning("Entity with id %d not found", nodeId);
        return list;
    }
    DQConnection conn;
    QSqlQuery query = conn.query();
    query.prepare(QString("SELECT id FROM %1 WHERE lft BETWEEN ? AND ? ORDER BY lft").arg(_tableName));
    query.bindValue(0, node.lft);
    query.bindValue(1, node.rgt);
    query.exec();
    while(query.next()){
        list.append(query.value(0));
    }
    return list;
}
