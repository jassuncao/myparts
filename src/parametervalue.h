#ifndef PARAMETERVALUE_H
#define PARAMETERVALUE_H

#include <QString>
#include <QMetaType>
#include <QDebug>

struct ParameterValue {
    explicit ParameterValue(const QVariant& value=QVariant(0), const QString & symbol=QString());
    QVariant value;
    QString symbol;
};

Q_DECLARE_METATYPE(ParameterValue)

QDebug operator<<(QDebug dbg, const ParameterValue &value);

#endif // PARAMETERVALUE_H
