#ifndef TREEITEMMODEL_H
#define TREEITEMMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QIcon>
#include <QQueue>
#include <QMap>
#include <QList>
#include <QMimeData>

class TreeItem;
class TreeItemModelPersistence;
class IconsRepository;

/**
 * @brief The TreeItemModel class provides a tree model that is used for the category and storage views.
 * This model is kept in memory with persistence performed by subclasses of TreeItemModelPersistence
 */
class TreeItemModel : public QAbstractItemModel
{
    Q_OBJECT
public:   

    explicit TreeItemModel(TreeItemModelPersistence * modelPersistence, const QString & mimeType, QObject *parent = 0);
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
    QString getItemIconId(const QModelIndex &index) const;
    bool setItemIconId(const QModelIndex &index, const QString & iconId);
    QList<int> getSubTreeIds(const QModelIndex &index) const;
    int rootItemId() const;
    bool select();
    void setToolTipColumn(int column);
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    Qt::DropActions supportedDropActions() const;
    QStringList mimeTypes() const;
    QMimeData * mimeData(const QModelIndexList &indexes) const;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);
    void setIconsRepository(IconsRepository * iconsRepository);    
protected:   
    TreeItem * getItem(const QModelIndex &index) const;
    QModelIndex internalFindIndex(int nodeId, const TreeItem *parentNode) const;
    TreeItem * rootItem() {return _invisibleRootItem;}
    void setDefaultIcon(const QIcon & icon) {_defaultIcon = icon;}
signals:
    void partsDropped(QVector<int> parts, TreeItem * item);
public slots:
    bool submit();
    void revert();
private:
    TreeItem * _invisibleRootItem;
    QIcon _defaultIcon;
    QQueue<TreeItem*> _uncommited;  
    TreeItemModelPersistence * _modelPersistence;
    const QString _mimeType;
    IconsRepository * _iconsRepository;
};

#endif // TREEITEMMODEL_H
