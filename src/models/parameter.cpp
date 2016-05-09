#include "parameter.h"

struct ParameterData : public QSharedData
{
    int id;
    QString name;
    Entities::ParameterDatatypes datatype;
    QString symbol;
    QString description;

    ParameterData() :
        id(-1),
        name(),
        datatype(Entities::INVALID),
        symbol(),
        description()
    {
    }
};

Parameter::Parameter() : d(new ParameterData)
{

}

Parameter::Parameter(const Parameter &rhs) : d(rhs.d)
{

}

Parameter &Parameter::operator=(const Parameter &rhs)
{
    if (this != &rhs)
        d.operator=(rhs.d);
    return *this;
}

Parameter::~Parameter()
{
}

bool Parameter::valid() const
{
    return d->id!=-1 && d->datatype != Entities::INVALID;
}

int Parameter::id() const
{
    return d->id;
}

void Parameter::setId(int id)
{
    d->id=id;
}

QString Parameter::name() const
{
    return d->name;
}

void Parameter::setName(const QString & name)
{
    d->name = name;
}

Entities::ParameterDatatypes Parameter::datatype() const
{
    return d->datatype;
}

void Parameter::setDatatype(Entities::ParameterDatatypes datatype)
{
    d->datatype = datatype;
}

QString Parameter::unitSymbol() const
{
    return d->symbol;
}

void Parameter::setUnitSymbol(const QString & symbol)
{
    d->symbol = symbol;
}

QString Parameter::description() const
{
    return d->description;
}

void Parameter::setDescription(const QString & description)
{
    d->description = description;
}
