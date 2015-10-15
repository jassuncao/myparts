#ifndef VALIDATINGITEMDELEGATE_H
#define VALIDATINGITEMDELEGATE_H

#include <QStyledItemDelegate>

class QValidator;

class ValidatingItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit ValidatingItemDelegate(QValidator * validator, QObject *parent = 0);
    ~ValidatingItemDelegate();
    void setModelData(QWidget *editor,QAbstractItemModel *model, const QModelIndex &index) const;

private:
    QValidator * _validator;
};

#endif // VALIDATINGITEMDELEGATE_H
