#ifndef PRICEITEMDELEGATE_H
#define PRICEITEMDELEGATE_H

#include <QStyledItemDelegate>

class PriceItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit PriceItemDelegate(bool useCustomEditor = false, QObject *parent = 0);
    ~PriceItemDelegate();
    QString displayText ( const QVariant & value, const QLocale & locale ) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option,
                              const QModelIndex &index) const;
protected:
    void initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const;
private:
    bool _useCustomEditor;
    QString _defaultCurrency;
    Qt::Alignment _horizontalAlignment;
};

#endif // PRICEITEMDELEGATE_H
