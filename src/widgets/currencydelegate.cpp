#include "currencydelegate.h"
#include "constants.h"
#include <QSettings>

CurrencyDelegate::CurrencyDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
    QString defaultCurrency(QChar(8364));
    QSettings settings;
    QString currency = settings.value(CURRENCY_SYMBOL_KEY, defaultCurrency).toString();
    bool currencyAfter = settings.value(CURRENCY_POSITION_KEY,true).toBool();
    if(currencyAfter){
        _currencyFormat = QString("%L2%1").arg(currency);
    }
    else{
        _currencyFormat = QString("%1%L2").arg(currency);
    }
}

QString CurrencyDelegate::displayText( const QVariant & value, const QLocale & locale ) const
{
    if(!value.isValid())
        return QString();
    if(!value.canConvert(QVariant::Double))
        return QString();
    double price = value.toDouble();
    return _currencyFormat.arg(price ,0,'f',2,' ');
}
