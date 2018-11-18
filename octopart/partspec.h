#ifndef PARTSPEC_H
#define PARTSPEC_H

#include "octopart_global.h"
#include <QString>
#include <QVariant>
#include <QSharedDataPointer>

namespace Octopart {

class PartSpecData;

class OCTOPART_SHARED_EXPORT PartSpec
{
public:
    PartSpec();
    PartSpec(const PartSpec &);
    PartSpec &operator=(const PartSpec &);
    ~PartSpec();

    QVariant maxValue() const;
    void setMaxValue(const QVariant &value);

    QVariant minValue() const;
    void setMinValue(const QVariant &value);

    QVariant value() const;
    void setValue(const QVariant &value);

    QString displayValue() const;
    void setDisplayValue(const QString &value);

    QString datatype() const;
    void setDatatype(const QString &value);

    QString key() const;
    void setKey(const QString &value);

    QString name() const;
    void setName(const QString &value);

    QString unitName() const;
    void setUnitName(const QString &value);

    QString unitSymbol() const;
    void setUnitSymbol(const QString &value);

private:
    QSharedDataPointer<PartSpecData> data;

};

} //namespace

#endif // PARTSPEC_H
