#include "parametervaluevalidator.h"
#include "unitparser.h"

ParameterValueValidator::ParameterValueValidator(QObject *parent) :
    QValidator(parent)
{
}

ParameterValueValidator::~ParameterValueValidator()
{

}

QValidator::State ParameterValueValidator::validate(QString & input, int &) const
{
    bool ok=false;
    if(input.isEmpty())
        return QValidator::Intermediate;
    UnitParser::parseUnit(input, &ok);
    return ok ? QValidator::Acceptable : QValidator::Invalid;
/*
    QByteArray buff;
    buff.reserve(input.length());
    for (int i = 0; i < input.length(); ++i) {
          QChar c = input.at(i);
          if(c.isLetter()){
              if(!_allowedSISymbols.contains(c)){
                  return QValidator::Invalid;
              }
              buff.append(locale().decimalPoint().toAscii());
          }
          else{
             buff.append(c.toAscii());
          }
    }
    QString text(buff);
    return QDoubleValidator::validate(text, pos);
    */
}
