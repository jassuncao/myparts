#include "parametervaluevalidator.h"
#include "unitparser.h"
#include <QLocale>

ParameterValueValidator::ParameterValueValidator(const QString& unit, QObject *parent) :
    QRegExpValidator(parent), _unit(unit)
{    
    QLocale locale;
    /*This regex allows some flexibility when using fractions. if not, the user would be unable to change a value from
     * 1/4W to 1W because it would be invalid when the 4 is deleted.
     */
    QString exp=QString("^-?\\d+/?\\d*([kKMGTPEZYmunpfazy\\x{03BC}]|\\%1)?\\d*[kKMGTPEZYmunpfazy\\x{03BC}]?").arg(locale.decimalPoint());
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

QValidator::State ParameterValueValidator::validate(QString& input, int& pos) const
{
    QValidator::State res = QRegExpValidator::validate(input, pos);
    if(res==QValidator::Invalid){
        input.replace(_unit.toLower(), _unit);
        res = QRegExpValidator::validate(input, pos);
    }
    return res;
}
