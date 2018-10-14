#include "partspec.h"
#include <QSharedData>

namespace Octopart {

class PartSpecData: public QSharedData {
public:
    PartSpecData() {}
    PartSpecData(const PartSpecData &other) : QSharedData(other),
        maxValue(other.maxValue),
        minValue(other.minValue),
        value(other.value),
        displayValue(other.displayValue),
        datatype(other.datatype),
        key(other.key),
        name(other.name),
        unitName(other.unitName),
        unitSymbol(other.unitSymbol)
    {}
    ~PartSpecData() { }

    QVariant maxValue;
    QVariant minValue;
    QVariant value;
    QString displayValue;
    QString datatype;
    QString key;
    QString name;
    QString unitName;
    QString unitSymbol;
};

PartSpec::PartSpec() : data(new PartSpecData)
{
}

PartSpec::PartSpec(const PartSpec &rhs) : data(rhs.data)
{
}

PartSpec &PartSpec::operator=(const PartSpec &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

PartSpec::~PartSpec()
{
}

QVariant PartSpec::maxValue() const {return data->maxValue;}

void PartSpec::setMaxValue(const QVariant &value)
{
    data->maxValue = value;
}

QVariant PartSpec::minValue() const{return data->minValue;}

void PartSpec::setMinValue(const QVariant &value)
{
    data->minValue = value;
}

QVariant PartSpec::value() const {return data->value;}

void PartSpec::setValue(const QVariant &value)
{
    data->value = value;
}

QString PartSpec::displayValue() const {return data->displayValue;}

void PartSpec::setDisplayValue(const QString &value)
{
    data->displayValue = value;
}

QString PartSpec::datatype() const {return data->datatype;}

void PartSpec::setDatatype(const QString &value)
{
    data->datatype = value;
}

QString PartSpec::key() const {return data->key;}

void PartSpec::setKey(const QString &value)
{
    data->key = value;
}
QString PartSpec::name() const {return data->name;}

void PartSpec::setName(const QString &value)
{
    data->name = value;
}

QString PartSpec::unitName() const {return data->unitName;}

void PartSpec::setUnitName(const QString &value)
{
    data->unitName = value;
}

QString PartSpec::unitSymbol() const {return data->unitSymbol;}

void PartSpec::setUnitSymbol(const QString &value)
{
    data->unitSymbol = value;
}


} //namespace

