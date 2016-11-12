#ifndef TREEPROXYFILTER_H
#define TREEPROXYFILTER_H

#include <QSortFilterProxyModel>

class TreeProxyFilter : public QSortFilterProxyModel
{
public:
    TreeProxyFilter(QObject *parent = 0);
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
};

#endif // TREEPROXYFILTER_H
