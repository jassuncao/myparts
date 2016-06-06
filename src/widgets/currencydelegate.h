#ifndef CURRENCYDELEGATE_H
#define CURRENCYDELEGATE_H

#include <QStyledItemDelegate>


class CurrencyDelegate : public QStyledItemDelegate
{
public:
    explicit CurrencyDelegate(QObject *parent = 0);
    QString displayText ( const QVariant & value, const QLocale & locale ) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
protected:
    void initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const;
private:
    QString _symbol;
    Qt::Alignment _horizontalAlignment;
};

#endif // CURRENCYDELEGATE_H
