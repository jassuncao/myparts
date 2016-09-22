#ifndef PROXYMODELNONEENTRY_H
#define PROXYMODELNONEENTRY_H

/**
 ** Written by Sven Anders (ANDURAS AG). Public domain code.
 **/

#include <QIdentityProxyModel>

class ProxyModelNoneEntry : public QIdentityProxyModel
{
   Q_OBJECT
 public:
  ProxyModelNoneEntry(const QString &entryText = tr("(None)"), const QVariant &entryValue = QVariant(), QObject *parent=0);
  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;
  QModelIndex mapToSource(const QModelIndex &proxyIndex) const;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
  Qt::ItemFlags flags(const QModelIndex &index) const;
  QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
  QModelIndex parent(const QModelIndex &child) const;

private:
  QString _entryText;
  QVariant _entryValue;

};

#endif // PROXYMODELNONEENTRY_H
