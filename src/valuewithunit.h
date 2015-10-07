#ifndef VALUEWITHUNIT_H
#define VALUEWITHUNIT_H

#include <QString>
#include <QMetaType>
#include <QDebug>

class ValueWithUnit
{
public:
    ValueWithUnit();
    ValueWithUnit(const double value, const QString & symbol);
    ValueWithUnit(const ValueWithUnit &other);
    ~ValueWithUnit();

    inline double value() const {
        return _value;
    }

    inline QString symbol() const{
        return _symbol;
    }

private:
    double _value;
    QString _symbol;
};

Q_DECLARE_METATYPE(ValueWithUnit)

QDebug operator<<(QDebug dbg, const ValueWithUnit &value);

#endif // VALUEWITHUNIT_H
