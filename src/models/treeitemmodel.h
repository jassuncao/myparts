#ifndef TREEITEMMODEL_H
#define TREEITEMMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QIcon>
#include <QQueue>
#include <QMap>

class TreeItem;

class TreeItemModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit TreeItemModel(int columns, QObject *parent = 0);
    ~TreeItemModel();

    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column, const QModelIndex &parentIndex = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    QModelIndex findIndex(int nodeId) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    bool insertItem(const QModelIndex &parent);
    bool removeItem(const QModelIndex &index);
    int getItemId(const QModelIndex &index) const;
    int rootItemId() const;
    bool select();
    void setToolTipColumn(int column);

protected:
    virtual bool fillTree(TreeItem * rootItem) = 0;
    virtual bool doInsert(TreeItem * item);
    virtual bool doUpdate(TreeItem * item);
    virtual bool doDelete(TreeItem * item);
    virtual bool doRevert(TreeItem * item);

    TreeItem * getItem(const QModelIndex &index) const;
    QModelIndex internalFindIndex(int nodeId, const TreeItem *parentNode) const;
    TreeItem * rootItem() {return _invisibleRootItem;}
    void setFolderIcon(const QIcon & icon) {_folderIcon = icon;}
signals:
    
public slots:
    bool submit();
    void revert();

private:
    TreeItem * _invisibleRootItem;
    QIcon _folderIcon;
    const QModelIndex * _uncommitedItemParent;
    TreeItem * _uncommitedItem;
    QQueue<TreeItem*> _uncommited;
    int _toolTipColumn;
    QMap<int,QModelIndex> _indexesLookup;
};

#endif // TREEITEMMODEL_H
