#include "treeproxyfilter.h"
#include <QtDebug>

TreeProxyFilter::TreeProxyFilter(QObject *parent):
    QSortFilterProxyModel(parent)
{
}

bool TreeProxyFilter::filterAcceptsRow(int row, const QModelIndex &parent) const
{
    QModelIndex index = sourceModel()->index(row, 0, parent);

    if (!index.isValid())
        return false;

    if (index.data().toString().contains(filterRegExp()))
        return true;

    int rows = sourceModel()->rowCount(index);
    for (row = 0; row < rows; row++)
        if (filterAcceptsRow(row, index))
            return true;

    return false;
}
