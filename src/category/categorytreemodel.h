#ifndef CATEGORYTREEMODEL_H
#define CATEGORYTREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QIcon>
#include <QQueue>

class CategoryTreeItem;
class TreeItem;

class CategoryTreeModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit CategoryTreeModel(QObject *parent = 0);
    ~CategoryTreeModel();

    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column, const QModelIndex &parentIndex = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    Qt::DropActions supportedDropActions() const;
    virtual QStringList mimeTypes() const;
    virtual QMimeData *mimeData(const QModelIndexList &indexes) const;
    virtual bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);
    //virtual bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    //virtual bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    //virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    //bool QAbstractItemModel::setItemData ( const QModelIndex & index, const QMap<int, QVariant> & roles )
    //virtual bool beginMoveRows(const QModelIndex &sourceParent, int sourceFirst, int sourceLast, const QModelIndex &destinationParent, int destinationRow);
    //QModelIndex insertCategory(const QString &name, const QString &description, const QModelIndex &parent);
    bool insertCategory(const QModelIndex &parent);
    bool removeCategory(const QModelIndex &index);
    int getItemId(const QModelIndex &index) const;
    int rootCategory() const;
public slots:
    bool submit();
    void revert();
signals:
    void partDropped();
private:
    TreeItem * getItem(const QModelIndex &index) const;
    TreeItem * _invisibleRootItem;
    QIcon _folderIcon;
    const QModelIndex * _uncommitedItemParent;
    TreeItem * _uncommitedItem;
    QQueue<TreeItem*> _uncommited;
};

#endif // CATEGORYTREEMODEL_H
