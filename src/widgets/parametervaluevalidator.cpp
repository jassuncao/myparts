#include "parametervaluevalidator.h"
#include "unitparser.h"
#include <QLocale>

ParameterValueValidator::ParameterValueValidator(const QString& unit, QObject *parent) :
    QRegExpValidator(parent), _unit(unit)
{
    QLocale locale;
    QString exp=QString("^-?\\d+([kKMGTPEZYmunpfazy\\x{03BC}]|\\%1)?\\d*[kKMGTPEZYmunpfazy\\x{03BC}]?").arg(locale.decimalPoint());
    if(unit.isEmpty()){
        exp.append('$');
    }
    else{
        exp.append(unit).append("?$");
    }
    QRegExp regExp(exp, Qt::CaseSensitive, QRegExp::RegExp2);
    setRegExp(regExp);
}

ParameterValueValidator::~ParameterValueValidator()
{}

/*
void ParameterValueValidator::setUnit(const QString& unit)
{
    _unit = unit;
}

QString ParameterValueValidator::unit() const
{
    return _unit;
}

QValidator::State ParameterValueValidator::validate(QString & input, int &) const
{
    bool ok=false;
    if(input.isEmpty())
        return QValidator::Intermediate;
    if(_unit.isEmpty()){
        UnitParser::parseUnit(input, &ok);
    }
    else{
        UnitParser::parseUnit(input, _unit, &ok);
    }
    return ok ? QValidator::Acceptable : QValidator::Invalid;
}
*/
