#ifndef PARAMETERVALUEVALIDATOR_H
#define PARAMETERVALUEVALIDATOR_H

#include <QRegExpValidator>
#include <QVector>

class ParameterValueValidator : public QRegExpValidator
{
    Q_OBJECT
public:
    ParameterValueValidator(const QString& unit=QString(), QObject * parent=0);
    ~ParameterValueValidator();
/*
    virtual QValidator::State validate(QString & input, int &) const;
    void setUnit(const QString& unit);
    QString unit() const;
    */
private:
    QString _unit;
};




#endif // PARAMETERVALUEVALIDATOR_H
