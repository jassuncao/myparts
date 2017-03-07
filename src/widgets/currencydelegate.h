#ifndef CURRENCYDELEGATE_H
#define CURRENCYDELEGATE_H

#include <QStyledItemDelegate>
#include <QMap>
#include <QStringList>

class CurrencyDelegate : public QStyledItemDelegate
{
public:
    explicit CurrencyDelegate(QObject *parent = 0);
    QString displayText ( const QVariant & value, const QLocale & locale ) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    QWidget *createEditor(QWidget *parent,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option,
                              const QModelIndex &index) const;

    static QStringList currencyCodes() {return _isoCodes;}


protected:
    void initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const;
private:    
    friend class _init;
    class _init {
    public:
        _init();
    };
    static _init _initializer;

    QString _defaultCurrency;
    Qt::Alignment _horizontalAlignment;    
    static QStringList _isoCodes;
    static QMap<QString,QString> _symbols;
};



#endif // CURRENCYDELEGATE_H
