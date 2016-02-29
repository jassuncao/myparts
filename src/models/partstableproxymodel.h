#ifndef PARTSTABLEPROXYMODEL_H
#define PARTSTABLEPROXYMODEL_H

#include <QIdentityProxyModel>

class PartStockTableModel;

class PartsTableProxyModel : public QIdentityProxyModel
{
public:
    explicit PartsTableProxyModel(QObject *parent);
    void setPartStockTableModel(PartStockTableModel* stockTableModel);
    Qt::ItemFlags flags(const QModelIndex& index) const;
    //bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    QVariant data(const QModelIndex &proxyIndex, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool submit();
private:
    PartStockTableModel * _stockTableModel;
};

#endif // PARTSTABLEPROXYMODEL_H
