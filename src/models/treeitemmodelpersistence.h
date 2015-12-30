#ifndef TREEITEMMODELPERSISTENCE_H
#define TREEITEMMODELPERSISTENCE_H

#include <QString>
#include <QStringList>
#include <QSqlRecord>
#include <QSqlDatabase>

class TreeItem;

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

class TreeItemModelPersistence
{
public:
    static const int NAME_COL = 0;
    static const int DESCRIPTION_COL = 1;
    static const int ID_COL = 2;

    TreeItemModelPersistence(const QString &tableName);
    virtual ~TreeItemModelPersistence();

    bool loadTree(TreeItem *rootItem);
    bool reparentNode(TreeItem *item, TreeItem* newParent) const;
    bool moveBeforeNode(TreeItem *item, TreeItem *sibling) const;
    bool insertAtEnd(TreeItem * item) const;
    bool revert(TreeItem * item) const;
    bool save(TreeItem * item) const;
    bool remove(TreeItem * item) const;
    QStringList getPath(TreeItem * item) const;
protected:
    void initRecordAndPrimaryIndex();
    QSqlDatabase database() const;
    virtual void beforeRemoveNode(int newParentId, const Node & node) const;
private:
    bool loadNode(int nodeId, NodeT * dest) const;
    int findNodePositionAsChild(TreeItem * parent) const;
    bool moveSubTree(NodeT* node, int newPosition) const;
    bool reserveSpace(int width, int newPosition) const;
    bool removeEmptySpace(int width, int oldPosition) const;
    const QString _tableName;
    QSqlDatabase _database;
};

#endif // TREEITEMMODELPERSISTENCE_H
