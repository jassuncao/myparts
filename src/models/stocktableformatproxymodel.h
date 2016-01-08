#ifndef STOCKTABLEFORMATPROXYMODEL_H
#define STOCKTABLEFORMATPROXYMODEL_H

#include <QIdentityProxyModel>

class StockTableFormatProxyModel : public QIdentityProxyModel {
public:
    explicit StockTableFormatProxyModel(QObject *parent);
    QVariant data(const QModelIndex &proxyIndex, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
};


#endif // STOCKTABLEFORMATPROXYMODEL_H
