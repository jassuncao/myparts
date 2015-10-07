#include "valuewithunit.h"

ValueWithUnit::ValueWithUnit()
{
}

ValueWithUnit::ValueWithUnit(const double value, const QString & symbol) :
    _value(value), _symbol(symbol)
{

}

ValueWithUnit::ValueWithUnit(const ValueWithUnit &other)
{
    _value = other._value;
    _symbol = other._symbol;
}

ValueWithUnit::~ValueWithUnit()
{
}

QDebug operator<<(QDebug dbg, const ValueWithUnit &value)
{
    dbg.nospace()<<value.value()<<value.symbol();
    return dbg.maybeSpace();
}

