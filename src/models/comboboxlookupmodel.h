#ifndef COMBOBOXLOOKUPMODEL_H
#define COMBOBOXLOOKUPMODEL_H

#include <QAbstractListModel>
#include <QList>

class ComboBoxLookupModel : public QAbstractListModel
{
    Q_OBJECT
public:
    ComboBoxLookupModel(const QList<QPair<QString,int> > & items, QObject *parent = 0);
    ComboBoxLookupModel(QObject *parent = 0);
    ~ComboBoxLookupModel();
    void setItems(QList<QPair<QString,int> > & items);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    int findIdIndex(const int id) const;
private:
    QList<QPair<QString,int> > _items;
};

#endif // COMBOBOXLOOKUPMODEL_H
