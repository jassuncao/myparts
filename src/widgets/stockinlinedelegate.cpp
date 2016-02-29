#include "stockinlinedelegate.h"

#include <QDebug>
#include <QItemEditorFactory>
#include <QSpinBox>
#include <QMessageBox>
#include "models/partstocktablemodel.h"
#include "models/partssqltablemodel.h"

StockInlineDelegate::StockInlineDelegate(PartsSqlTableModel * partsModel, QObject * parent) :
    QStyledItemDelegate(parent)
{
    _partStockTableModel = PartStockTableModel::createNew(0);
}

StockInlineDelegate::~StockInlineDelegate()
{
    delete _partStockTableModel;
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
    int oldValue = index.data(Qt::EditRole).toInt();
    int newValue = lineEdit->value();
    int diff = newValue - oldValue;

    QModelIndex partIdIndex = model->index(index.row(), PartsSqlTableModel::ColumnId);
    _partStockTableModel->setCurrentPartId(partIdIndex.data(Qt::EditRole));
    _partStockTableModel->appendRow(diff, QVariant(), QString());
    if(_partStockTableModel->submitAll()){
        model->setData(index,QVariant(newValue));
        model->submit();
    }
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
