#ifndef PARTSTABLEPROXYMODEL_H
#define PARTSTABLEPROXYMODEL_H

#include <QIdentityProxyModel>

class PartStockTableModel;

class PartsTableProxyModel : public QIdentityProxyModel
{
public:
    explicit PartsTableProxyModel(QObject *parent);
    virtual ~PartsTableProxyModel();
    Qt::ItemFlags flags(const QModelIndex& index) const;
    QVariant data(const QModelIndex &proxyIndex, int role = Qt::DisplayRole) const;
    //QVariant headerData(int section, Qt::Orientation orientation, int role) const;
};

#endif // PARTSTABLEPROXYMODEL_H
