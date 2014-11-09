#ifndef CURRENCYDELEGATE_H
#define CURRENCYDELEGATE_H

#include <QStyledItemDelegate>


class CurrencyDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit CurrencyDelegate(QObject *parent = 0);
    QString displayText ( const QVariant & value, const QLocale & locale ) const;
signals:

public slots:
private:
    QString _currencyFormat;
};

#endif // CURRENCYDELEGATE_H
