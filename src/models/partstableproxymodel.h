#ifndef PARTSTABLEPROXYMODEL_H
#define PARTSTABLEPROXYMODEL_H

#include <QIdentityProxyModel>

class PartsTableProxyModel : public QIdentityProxyModel
{
public:
    explicit PartsTableProxyModel(QObject *parent);
    QVariant data(const QModelIndex &proxyIndex, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
};

#endif // PARTSTABLEPROXYMODEL_H
