#ifndef STORAGEDAO_H
#define STORAGEDAO_H

#include <QStandardItemModel>
#include <QStringList>
#include <QVector>
#include <QList>

class TreeItem;
class QString;
struct NodeT;

class StorageDAO
{
public:   
    static const int NAME_COL = 0;
    static const int ID_COL = 1;

    StorageDAO();
    void createTree(TreeItem *rootItem);
    bool moveNode(int nodeId, int parentId);
    bool moveBeforeNode(int nodeId, int siblingId);
    //static bool setCategoryName(int nodeId, const QString &name);
    QStringList getPath(int nodeId);
    int insertAtEnd(const QVector<QVariant> & data, int parentId);
    bool insertAtEnd(TreeItem * item);
    bool reload(TreeItem * item);
    bool update(TreeItem * item);
    bool remove(int nodeId, int newParent);
    bool setPartStorage(const QVector<int> & parts, int parentStorageId);
    QVector<QVariant> listChildStorage(int nodeId);
private:
    const QLatin1String _tableName;
    bool loadNode(int nodeId, NodeT * dest);
};

#endif // STORAGEDAO_H
