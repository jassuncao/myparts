#ifndef PARAMETERVALUEVALIDATOR_H
#define PARAMETERVALUEVALIDATOR_H

#include <QValidator>
#include <QVector>

class ParameterValueValidator : public QValidator
{
    Q_OBJECT
public:
    ParameterValueValidator(QObject * parent=0);
    ~ParameterValueValidator();
    virtual QValidator::State validate(QString & input, int &) const;
};




#endif // PARAMETERVALUEVALIDATOR_H
