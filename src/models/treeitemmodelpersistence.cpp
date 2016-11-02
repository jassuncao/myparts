#include "treeitemmodelpersistence.h"
#include "treeitem.h"

#include <limits>
#include <QVector>
#include <QPair>
#include <QStack>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QSqlRecord>
#include <QSqlDriver>

typedef QPair<TreeItem*,int> TreeItemPair;

TreeItemModelPersistence::TreeItemModelPersistence(const QString &tableName) :
    _tableName(tableName)
{
    _database = QSqlDatabase::database();
}

TreeItemModelPersistence::~TreeItemModelPersistence()
{
}

void TreeItemModelPersistence::initRecordAndPrimaryIndex()
{

}

bool TreeItemModelPersistence::loadTree(TreeItem *rootItem)
{

    QSqlRecord rec = _database.record(_tableName);
    QSqlRecord updateRec(rec);
    updateRec.setGenerated("id",false);
    updateRec.setGenerated("lft",false);
    updateRec.setGenerated("rgt",false);
    updateRec.setGenerated("path",false);

    QStack<TreeItemPair> stack;
    stack.push(TreeItemPair(rootItem, std::numeric_limits<qint32>::max()));    
    QString selectStatement =_database.driver()->sqlStatement(QSqlDriver::SelectStatement, _tableName, rec, false);
    if(selectStatement.isEmpty()){    
        qWarning()<<"Unable to select fields from table"<<_tableName;
        qWarning()<<"Error:"<<_database.driver()->lastError();
        return false;
    }
    /*
    QString updateStatement =_database.driver()->sqlStatement(QSqlDriver::UpdateStatement, _tableName, _rec, true);
    qDebug()<<"updateStatement is "<<updateStatement;
    */
    selectStatement = selectStatement.append(" ORDER BY lft ASC");
    QSqlQuery query(_database);
    qDebug()<<"Select statement is "<<selectStatement;
    //QSqlQuery query(QString("SELECT id, name, lft, rgt FROM %1 ORDER BY lft ASC").arg(_tableName));
    if(query.exec(selectStatement)){
        QSqlRecord rec = query.record();
        int idIdx = rec.indexOf("id");
        int lftIdx = rec.indexOf("lft");
        int rgtIdx = rec.indexOf("rgt");
        int nameIdx = rec.indexOf("name");
        int descriptionIdx = rec.indexOf("description");
        int iconIdx = rec.indexOf("icon");
        while(query.next()){
            rec = query.record();
            QVariant id = rec.value(idIdx);
            int lft = rec.value(lftIdx).toInt();
            int rgt = rec.value(rgtIdx).toInt();

            //Remove nodes from the stack until we find the parent node of the current item
            //Used to go back in the tree when we find a node that is not part of the same hierarchy
            if(stack.count()>0){
                while(rgt>stack.top().second){
                    stack.pop();
                }
            }
            QVariant name = rec.value(nameIdx);
            QVariant description = rec.value(descriptionIdx);
            QVariant icon = rec.value(iconIdx);
            TreeItem * item = new TreeItem(id.toInt(),name, description, icon);
            //TreeItem * item = createNewItem(rec);
            TreeItem * parentItem = stack.top().first;
            parentItem->appendChild(item);
            //the node has children if the rgt-lft > 1
            if(rgt-lft>1){
                stack.push(TreeItemPair(item, rgt));
            }
        }
        return true;
    }
    qWarning()<<"Unable to load tree nodes. Reason: "<<query.lastError();
    return false;
}

bool TreeItemModelPersistence::reparentNode(TreeItem *item, TreeItem* newParent) const
{
    Node itemNode;
    if(!loadNode(item->id(), &itemNode)){
        return false;
    }

    int newPosition = findNodePositionAsChild(newParent);
    if(newPosition<0){
        return false;
    }

    return moveSubTree(&itemNode, newPosition);
}

bool TreeItemModelPersistence::moveBeforeNode(TreeItem *item, TreeItem *sibling) const
{
    Node itemNode;
    if(!loadNode(item->id(), &itemNode)){
        return false;
    }

    int newPosition = -1;
    if(sibling){
        Node siblingNode;
        if(loadNode(sibling->id(), &siblingNode)){
            newPosition = siblingNode.lft;
        }
    }
    else{
        //When a sibling node is not specified we will add it as a sibling of the root node(s)
        QSqlQuery query(QString("SELECT MIN(lft) FROM %1 ").arg(_tableName));
        if(query.exec()){
            if(query.next()) {
               newPosition = query.value(0).toInt();
            }
            else {
                newPosition = 1;
            }
        }
        else{
            qWarning()<<"Unable to find position in tree. Reason: "<<query.lastError();
        }
    }
    if(newPosition<0){
        return false;
    }

    return moveSubTree(&itemNode, newPosition);
}

bool TreeItemModelPersistence::insertAtEnd(TreeItem * item) const
{
    int position = findNodePositionAsChild(item->parent());
    if(position<0){
        return false;
    }

    //First make space for the node by increasing lft and rgt of the nodes to the "right" of position
    reserveSpace(2, position);

    //Finally insert the node into the position
    QSqlQuery query;
    query.prepare(QString("INSERT INTO %1 (name, description, icon, lft, rgt) VALUES (?,?,?,?,?)").arg(_tableName));
    query.bindValue(0,item->name());
    query.bindValue(1,item->description());
    query.bindValue(2,item->iconId());
    query.bindValue(3,position);
    query.bindValue(4,position+1);
    if(query.exec()){
        int id = query.lastInsertId().toInt();
        item->setId(id);
        return true;
    }
    qWarning() << "Failed to save Node. Reason: "<<query.lastError();
    return false;
}

bool TreeItemModelPersistence::revert(TreeItem * item) const
{
    QSqlQuery query;
    query.prepare(QString("SELECT name, description, icon FROM %1 WHERE id = ?").arg(_tableName));
    query.bindValue(0, item->id());
    if(!query.exec()){
        qWarning() << "Failed to execute query:" << query.lastError();
        return false;
    }
    if(!query.next()){
        qWarning("Entity with id %d not found",item->id());
        return false;
    }
    item->setName(query.value(0));
    item->setDescription(query.value(1));
    item->setIconId(query.value(2));
    return true;
}

bool TreeItemModelPersistence::save(TreeItem * item) const
{
    QSqlQuery query;    
    query.prepare(QString("UPDATE %1 SET name=?, description=?, icon=? WHERE id=?").arg(_tableName));
    query.bindValue(0,item->name());
    query.bindValue(1,item->description());
    query.bindValue(2,item->iconId());
    query.bindValue(3,item->id());
    if(query.exec()){
        qDebug()<<"Item "<<item->id()<<"("<<item->name()<<") saved";
        return true;
    }
    qWarning() << "Failed to save node name. Reason: "<<query.lastError();
    return false;
}

bool TreeItemModelPersistence::remove(TreeItem * item) const
{
    Node node;
    if(!loadNode(item->id(),&node)) {
        qWarning("Entity with id %d not found", item->id());
        return false;
    }    
    beforeRemoveNode(item->parent()->id(), node);
    int width = (node.rgt-node.lft)+1;

    QSqlQuery query;
    query.prepare(QString("DELETE FROM %1 WHERE lft BETWEEN ? AND ?").arg(_tableName));
    query.bindValue(0,node.lft);
    query.bindValue(1,node.rgt);
    if(!query.exec()){
        qWarning() << "Failed to remove node(s). Reason: "<<query.lastError();
        return false;
    }
    qDebug("Deleted %d rows",query.numRowsAffected());

    //Finally "remove" the empty space left in the old position of the subtree
    return removeEmptySpace(width, node.rgt);
}

QStringList TreeItemModelPersistence::getPath(TreeItem * item) const
{
    QStringList list;
    QSqlQuery query;
    query.prepare(QString("SELECT parent.name FROM %1 AS node, %1 AS parent "\
                          " WHERE node.lft BETWEEN parent.lft AND parent.rgt AND node.id = ? ORDER BY parent.lft ASC").arg(_tableName));
    query.bindValue(0,item->id());
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

bool TreeItemModelPersistence::loadNode(int nodeId, NodeT *dest) const
{
    QSqlQuery query;
    query.prepare(QString("SELECT lft, rgt, name FROM %1 WHERE id = ?").arg(_tableName));
    query.bindValue(0, nodeId);
    if(!query.exec()){
        qWarning() << "Failed to execute query:" << query.lastError();
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

bool TreeItemModelPersistence::moveSubTree(NodeT* node, int newPosition) const
{
    int width = (node->rgt-node->lft)+1;
    int distance = newPosition - node->lft;
    int tmppos = node->lft;
    int oldPos = node->rgt;
    if (distance < 0)
    {
        distance -= width;
        tmppos += width;
    }
    qDebug("Moving node: newPosition=%d, width=%d, distance=%d, tmppos=%d",newPosition, width, distance, tmppos);
    if(distance==0)
        return false;
    //First make space for the subtree by increasing lft and rgt of the nodes to the "right" of the new position
    if(!reserveSpace(width, newPosition))
        return false;

    QSqlQuery query;
    //Now move the subtree to the position by offseting lft and rgt with the distance from the old position to the new position
    query.prepare(QString("UPDATE %1 SET lft = lft + ?, rgt = rgt + ? WHERE lft >= ? AND rgt < ?").arg(_tableName));
    query.bindValue(0,distance);
    query.bindValue(1,distance);
    query.bindValue(2,tmppos);
    query.bindValue(3,tmppos+width);
    if(query.exec()){
        qDebug("Changed %d rows", query.numRowsAffected());
    }
    else{
        qWarning()<<"Unable to move subtree. Reason: "<<query.lastError();
        return false;
    }

    //Finally "remove" the empty space left in the old position of the subtree
    return removeEmptySpace(width, oldPos);

}

bool TreeItemModelPersistence::reserveSpace(int width, int newPosition) const
{
    QSqlQuery query;
    query.prepare(QString("UPDATE %1 SET lft = lft + ? WHERE lft >= ?").arg(_tableName));
    query.bindValue(0,width);
    query.bindValue(1,newPosition);
    if(query.exec()){
        qDebug("Changed %d rows", query.numRowsAffected());
    }
    else{
        qWarning()<<"Unable to make space for the subtree. Reason: "<<query.lastError();
        return false;
    }

    query.prepare(QString("UPDATE %1 SET rgt = rgt + ? WHERE rgt >= ?").arg(_tableName));
    query.bindValue(0,width);
    query.bindValue(1,newPosition);
    if(query.exec()){
        qDebug("Changed %d rows", query.numRowsAffected());
    }
    else{
        qWarning()<<"Unable to make space for the subtree. Reason: "<<query.lastError();
        return false;
    }
    return true;
}

bool TreeItemModelPersistence::removeEmptySpace(int width, int oldPosition) const
{
    QSqlQuery query;
    query.prepare(QString("UPDATE %1 SET lft = lft - ? WHERE lft > ?").arg(_tableName));
    query.bindValue(0,width);
    query.bindValue(1,oldPosition);
    if(query.exec()){
        qDebug("Changed %d rows", query.numRowsAffected());
    }
    else{
        qWarning()<<"Unable to remove empty space. Reason: "<<query.lastError();
        return false;
    }

    query.prepare(QString("UPDATE %1 SET rgt = rgt - ? WHERE rgt > ?").arg(_tableName));
    query.bindValue(0,width);
    query.bindValue(1,oldPosition);
    if(query.exec()){
        qDebug("Changed %d rows", query.numRowsAffected());
    }
    else{
        qWarning()<<"Unable to remove empty space. Reason: "<<query.lastError();
        return false;
    }
    return true;
}

int TreeItemModelPersistence::findNodePositionAsChild(TreeItem * parent) const
{
    int position = -1;
    if(parent){
        Node parentNode;
        if(loadNode(parent->id(), &parentNode)){
            position = parentNode.rgt;
        }
    }
    else{
        //When a parent node is not specified we will add it as a sibling of the root node(s)
        QSqlQuery query(QString("SELECT MAX(rgt) FROM %1 ").arg(_tableName));
        if(query.exec()){
            if(query.next()){
               position = query.value(0).toInt()+1;
            }
            else{
                position = 1;
            }
        }
        else{
            qWarning()<<"Unable to find position in tree. Reason: "<<query.lastError();
        }
    }
    return position;
}

QSqlDatabase TreeItemModelPersistence::database() const
{
    return _database;
}

void TreeItemModelPersistence::beforeRemoveNode(int newParentId, const Node & node) const
{
}

