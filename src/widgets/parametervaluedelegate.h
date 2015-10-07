#ifndef PARAMETERVALUEDELEGATE_H
#define PARAMETERVALUEDELEGATE_H

#include <QStyledItemDelegate>
class ParameterValueValidator;

class ParameterValueDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
     ParameterValueDelegate(QObject * parent = 0);
     ~ParameterValueDelegate();
     virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &index) const;
     virtual QString displayText(const QVariant &value, const QLocale &locale) const;
     virtual void setModelData(QWidget *editor,QAbstractItemModel *model, const QModelIndex &index) const;
     virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;
private:
     ParameterValueValidator * _validator;
};



#endif // PARAMETERVALUEDELEGATE_H
