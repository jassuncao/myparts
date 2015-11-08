#include "extrarowproxymodel.h"
#include <QFont>

ExtraRowProxyModel::ExtraRowProxyModel(QObject *parent) : QAbstractProxyModel(parent),
    _valueColumn(0), _displayColumn(1)
{    
}

ExtraRowProxyModel::~ExtraRowProxyModel()
{
}

void ExtraRowProxyModel::setEmptyDisplayText(const QString & displayText)
{
    _emptyDisplayText =  displayText;
    QModelIndex topLeft = createIndex(0, _displayColumn);
    emit dataChanged(topLeft, topLeft);
}

int ExtraRowProxyModel::rowCount(const QModelIndex &parent) const
{
  return sourceModel()->rowCount(parent)+1;
}

int ExtraRowProxyModel::columnCount(const QModelIndex &parent) const
{
  return sourceModel()->columnCount(parent);
}

QModelIndex ExtraRowProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
  if (!sourceModel() || !sourceIndex.isValid())
      return QModelIndex();
  if (sourceIndex.parent().isValid())
      return QModelIndex();
  return createIndex(sourceIndex.row()+1, sourceIndex.column(), sourceIndex.internalPointer());
}

QModelIndex ExtraRowProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
  if (!sourceModel() || !proxyIndex.isValid())
      return QModelIndex();
  if (proxyIndex.row() == 0)
      return QModelIndex();
  return sourceModel()->index(proxyIndex.row()-1, proxyIndex.column());
}

QVariant ExtraRowProxyModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid()) return QVariant();

  if (index.row() == 0) {
      if (role == Qt::DisplayRole || role==Qt::EditRole){
          if(index.column()==_displayColumn){
            return _emptyDisplayText;
          }
          if(index.column()==_valueColumn){
              return _emptyValue;
          }
      }
      else if(role == Qt::FontRole && index.column()==_displayColumn){
          QFont boldFont;
          boldFont.setItalic(true);
          return boldFont;
      }
      return QVariant();
  }
  /*
  if(role==Qt::AccessibleDescriptionRole && index.row() == 1 && index.column()==_displayColumn){
      return QVariant(QString::fromLatin1("separator"));
  }
  */
  return QAbstractProxyModel::data(index, role);
}

Qt::ItemFlags ExtraRowProxyModel::flags(const QModelIndex &index) const
{
  if (!index.isValid()) return Qt::NoItemFlags;
  if (index.row() == 0)
      return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
  return QAbstractProxyModel::flags(index);
}

QModelIndex ExtraRowProxyModel::index(int row, int column, const QModelIndex &parent) const
{
    if(!hasIndex(row,column, parent)){
        return QModelIndex();
    }
    if(row==0){
        return createIndex(row, column);
    }
   const QModelIndex sourceIndex = sourceModel()->index(row-1, column, parent);
  return mapFromSource(sourceIndex);
}

QModelIndex ExtraRowProxyModel::parent(const QModelIndex &child) const
{
  Q_UNUSED(child)
  return QModelIndex();
}

