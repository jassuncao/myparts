#ifndef PARAMETER_H
#define PARAMETER_H

#include <QSharedDataPointer>
#include "entities/entities.h"

struct ParameterData;

class Parameter
{
public:
    enum ParameterDatatypes {
        INVALID = 0,
        INTEGER = 1,
        DECIMAL = 2,
        TEXT = 3
    };
    Parameter();
    Parameter(const Parameter &);
    Parameter &operator=(const Parameter &);
    ~Parameter();
    bool valid() const;
    int id() const;
    void setId(int id);
    QString name() const;
    void setName(const QString & name);
    ParameterDatatypes datatype() const;
    void setDatatype(ParameterDatatypes datatype);
    QString unitSymbol() const;
    void setUnitSymbol(const QString & symbol);
    QString description() const;
    void setDescription(const QString & description);
private:
    QSharedDataPointer<ParameterData> d;
};

Q_DECLARE_TYPEINFO(Parameter, Q_MOVABLE_TYPE);

#endif // PARAMETER_H
