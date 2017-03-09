#ifndef CURRENCYDELEGATE_H
#define CURRENCYDELEGATE_H

#include <QStyledItemDelegate>
#include <QMap>
#include <QStringList>

class CurrencyDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit CurrencyDelegate(QObject *parent = 0);
    ~CurrencyDelegate();
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &,
                          const QModelIndex &index) const;

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
