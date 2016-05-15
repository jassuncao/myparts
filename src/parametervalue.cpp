#include "parametervalue.h"

ParameterValue::ParameterValue(const QVariant &value, const QString & symbol) :
    value(value), symbol(symbol)
{}

QDebug operator<<(QDebug dbg, const ParameterValue &value)
{
    dbg.nospace()<<value.value<<value.symbol;
    return dbg.maybeSpace();
}


