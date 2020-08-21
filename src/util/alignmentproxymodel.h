#ifndef ALIGNMENTPROXYMODEL_H
#define ALIGNMENTPROXYMODEL_H

#include <QIdentityProxyModel>
#include <QVector>

class AlignmentProxyModel : public QIdentityProxyModel
{
public:
    explicit AlignmentProxyModel(QObject * parent = 0);
    void setColumnAlignment(int col, Qt::Alignment alignment);
    QVariant data(const QModelIndex &proxyIndex, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
private:
    QVector<QVariant> _alignments;
};

#endif // ALIGNMENTPROXYMODEL_H
