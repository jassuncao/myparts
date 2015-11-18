#include "currencydelegate.h"
#include "constants.h"
#include <QSettings>

CurrencyDelegate::CurrencyDelegate(QObject *parent) :
    QStyledItemDelegate(parent), _horizontalAlignment(Qt::AlignRight)
{
    QSettings settings;
    _symbol = settings.value(CURRENCY_SYMBOL_KEY).toString();
}

QString CurrencyDelegate::displayText( const QVariant & value, const QLocale & locale ) const
{
    if(!value.isValid())
        return QString();
    if(!value.canConvert(QVariant::Double))
        return QString();    
    double price = value.toDouble();
    return locale.toCurrencyString(price, _symbol);
}

void CurrencyDelegate::initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const
{
    QStyledItemDelegate::initStyleOption(option, index);
    option->displayAlignment = option->displayAlignment | _horizontalAlignment;
}
