#ifndef EXTRAROWPROXYMODEL_H
#define EXTRAROWPROXYMODEL_H

#include <QAbstractProxyModel>
#include <QModelIndex>

class ExtraRowProxyModel : public QAbstractProxyModel
{
    Q_OBJECT
public:
    explicit ExtraRowProxyModel(QObject *parent = 0);
    ~ExtraRowProxyModel();
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const;
    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;
    void setEmptyDisplayText(const QString & displayText);
    void setEmptyValue(const QVariant & emptyValue) { _emptyValue = emptyValue;}
private:
    int _valueColumn;
    int _displayColumn;
    QVariant _emptyValue;
    QString _emptyDisplayText;
};

#endif // EXTRAROWPROXYMODEL_H
