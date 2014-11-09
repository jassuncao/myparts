#ifndef QPARTSFILTERMODEL_H
#define QPARTSFILTERMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include <QVariant>
#include <QModelIndex>

class QPartsFilterModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit QPartsFilterModel(QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
signals:
    
public slots:
private:
    QList<QVariant> _items;
};

#endif // QPARTSFILTERMODEL_H
