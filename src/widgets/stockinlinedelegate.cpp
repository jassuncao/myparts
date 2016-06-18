#include "stockinlinedelegate.h"

#include <QDebug>
#include <QItemEditorFactory>
#include <QSpinBox>
#include <QMessageBox>
#include "models/partstocktablemodel.h"
#include "models/partssqltablemodel.h"
#include <QSqlQuery>

StockInlineDelegate::StockInlineDelegate(QSqlDatabase & database, QObject * parent) :
    QStyledItemDelegate(parent),
    _database(database)
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
    int oldValue = index.data(Qt::EditRole).toInt();
    int stockChange = newValue - oldValue;
    QVariant partId = model->index(index.row(), PartsSqlTableModel::ColumnId).data(Qt::EditRole);
    model->setData(index, newValue);
    _database.transaction();
    bool res = model->submit();
    if(res){
        res = insertStockChange(partId, stockChange);
    }

    if(res){
        _database.commit();
    }
    else{
        _database.rollback();
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

bool StockInlineDelegate::insertStockChange(const QVariant& partId, const QVariant & stockChange)
{
    QSqlQuery query(_database);
    query.prepare("INSERT INTO stock_change (change, dateTime, part) "
                  "VALUES(?,?,?)");
    query.bindValue(0, stockChange);
    query.bindValue(1, QDateTime::currentDateTimeUtc().toTime_t());
    query.bindValue(2, partId);
    return query.exec();
}
