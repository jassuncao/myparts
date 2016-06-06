#include "stockinlinedelegate.h"

#include <QDebug>
#include <QItemEditorFactory>
#include <QSpinBox>
#include <QMessageBox>
#include "models/partstocktablemodel.h"
#include "models/partssqltablemodel.h"

StockInlineDelegate::StockInlineDelegate(QObject * parent) :
    QStyledItemDelegate(parent)
{
}

StockInlineDelegate::~StockInlineDelegate()
{
}

QWidget * StockInlineDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;
    QSpinBox *le = new QSpinBox(parent);
    le->setFrame(le->style()->styleHint(QStyle::SH_ItemView_DrawDelegateFrame, 0, le));
    return le;
}

void StockInlineDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QSpinBox* lineEdit = qobject_cast<QSpinBox*>(editor);
    int newValue = lineEdit->value();
    model->setData(index, newValue, PartsSqlTableModel::SET_STOCK_ROLE);
    model->submit();
}

void StockInlineDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QVariant v = index.data(Qt::EditRole);
    if(v.isValid()){
        QSpinBox* spinBox = qobject_cast<QSpinBox*>(editor);
        spinBox->setValue(v.toInt());
    }
    else{
        qWarning()<<"setEditorData invalid ";
    }
}
