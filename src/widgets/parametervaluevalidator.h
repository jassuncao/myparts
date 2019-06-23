#ifndef PARAMETERVALUEVALIDATOR_H
#define PARAMETERVALUEVALIDATOR_H

#include <QRegExpValidator>
#include <QVector>

class ParameterValueValidator : public QRegExpValidator
{
    Q_OBJECT
public:
    ParameterValueValidator(const QString& unit=QString(), bool emptyAllowed=false,  QObject * parent=0);
    ~ParameterValueValidator();
    virtual QValidator::State validate(QString& input, int& pos) const;
private:
    QString _unit;
    bool _emptyAllowed;
};




#endif // PARAMETERVALUEVALIDATOR_H
