#ifndef COMBOITEMDELEGATE_H
#define COMBOITEMDELEGATE_H

#include <QStyledItemDelegate>
#include <QModelIndex>

class QAbstractItemModel;

class ComboItemDelegate : public QStyledItemDelegate
{
public:
    explicit ComboItemDelegate(QObject *parent = 0);
    virtual ~ComboItemDelegate();
    void setComboModel(QAbstractItemModel * model);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;    
private:
    QAbstractItemModel * _comboModel;
};

#endif // COMBOITEMDELEGATE_H
