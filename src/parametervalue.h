#ifndef PARAMETERVALUE_H
#define PARAMETERVALUE_H

#include <QString>
#include <QMetaType>
#include <QDebug>


/*
class ParameterValue
{
public:
    ParameterValue();
    ~ParameterValue();

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
*/

struct ParameterValue {
    explicit ParameterValue(const QVariant& value=QVariant(0), const QString & symbol=QString());
    QVariant value;
    QString symbol;
};

Q_DECLARE_METATYPE(ParameterValue)

QDebug operator<<(QDebug dbg, const ParameterValue &value);

#endif // PARAMETERVALUE_H
