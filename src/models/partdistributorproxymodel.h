#ifndef PARTDISTRIBUTORPROXYMODEL_H
#define PARTDISTRIBUTORPROXYMODEL_H

#include <QIdentityProxyModel>

class PartDistributorProxyModel : public QIdentityProxyModel
{
public:
    explicit PartDistributorProxyModel(QObject *parent);
    virtual ~PartDistributorProxyModel();
    Qt::ItemFlags flags(const QModelIndex& index) const;
    QVariant data(const QModelIndex &proxyIndex, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
};

#endif // PARTDISTRIBUTORPROXYMODEL_H
