#include "price.h"

Price::Price() : _value(0), _currency(QString()), _valid(false)
{
}

Price::Price(double value, const QString & currency) :
    _value(value), _currency(currency), _valid(true)
{
}

QDebug operator<<(QDebug dbg, const Price &value)
{
    dbg.nospace()<<value.value()<<value.currency();
    return dbg.maybeSpace();
}

