#ifndef UNITPROXYMODEL_H
#define UNITPROXYMODEL_H

#include <QIdentityProxyModel>

class UnitProxyModel : public QIdentityProxyModel
{
    Q_OBJECT
public:
    UnitProxyModel(QObject* parent = 0);
    ~UnitProxyModel();
    //int columnCount(const QModelIndex& parent = QModelIndex()) const;
    //QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &proxyIndex, int role = Qt::DisplayRole) const;
    /*
    virtual QModelIndex mapToSource(const QModelIndex &proxyIndex) const;
    virtual QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;
    */
};

#endif // UNITPROXYMODEL_H
