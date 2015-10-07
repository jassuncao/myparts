#ifndef PARAMETERUNITDELEGATE_H
#define PARAMETERUNITDELEGATE_H

#include <QStyledItemDelegate>

class ParameterUnitDelegate2 : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit ParameterUnitDelegate2(QObject *parent = 0);
    ~ParameterUnitDelegate2();
    QWidget * createEditor(QWidget *aParent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

};

#endif // PARAMETERUNITDELEGATE_H
