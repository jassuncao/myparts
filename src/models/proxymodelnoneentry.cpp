#include "proxymodelnoneentry.h"

#include <QDebug>
#include <QBrush>
#include <QFont>

ProxyModelNoneEntry::ProxyModelNoneEntry(const QString &entryText, const QVariant &entryValue, QObject *parent)
    : QIdentityProxyModel(parent),
      _entryText(entryText),
      _entryValue(entryValue)
{
}

int ProxyModelNoneEntry::rowCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent)
  return QIdentityProxyModel::rowCount()+1;
}

QModelIndex ProxyModelNoneEntry::mapFromSource(const QModelIndex &sourceIndex) const
{
  if (!sourceIndex.isValid()) return QModelIndex();
  else if (sourceIndex.parent().isValid()) return QModelIndex();
  return createIndex(sourceIndex.row()+1, sourceIndex.column());
}

QModelIndex ProxyModelNoneEntry::mapToSource(const QModelIndex &proxyIndex) const
{
  if (!proxyIndex.isValid()) return QModelIndex();
  else if (proxyIndex.row() == 0) return QModelIndex();
  return sourceModel()->index(proxyIndex.row()-1, proxyIndex.column());
}

QVariant ProxyModelNoneEntry::data(const QModelIndex &index, int role) const
{
  if (!index.isValid()) return QVariant();

  if (index.row() == 0)
  {
    if (role == Qt::DisplayRole){
        return _entryText;
    }
    else if (role == Qt::DecorationRole){
      return QVariant();
    }
    else if (role == Qt::FontRole) {
        QFont font;
        font.setItalic(true);
        return font;
    }
    else
      return _entryValue;
  }
  return QIdentityProxyModel::data(createIndex(index.row(),index.column()), role);
}

Qt::ItemFlags ProxyModelNoneEntry::flags(const QModelIndex &index) const
{
  if (!index.isValid()) return Qt::NoItemFlags;
  if (index.row() == 0) return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
  return QIdentityProxyModel::flags(createIndex(index.row(),index.column()));
}

QModelIndex ProxyModelNoneEntry::index(int row, int column, const QModelIndex &parent) const
{
  if (row > rowCount()) return QModelIndex();
  return createIndex(row, column);
}

QModelIndex ProxyModelNoneEntry::parent(const QModelIndex &child) const
{
  Q_UNUSED(child)
  return QModelIndex();
}
