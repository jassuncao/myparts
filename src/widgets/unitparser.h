#ifndef UNITPARSER_H
#define UNITPARSER_H

#include <QString>

class QLocale;

class UnitParser
{
private:
    UnitParser();
public:
    static double parseUnit(const QString& input, bool * ok=0);
    static double parseUnit(const QString& input, const QString& unit, bool * ok=0);
    static QString formatUnit(const double value, const QString& unit, const QLocale& locale);
    static QString formatPercentage(double value, QChar unit);
};


#endif // UNITPARSER_H
