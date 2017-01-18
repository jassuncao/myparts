#ifndef SMDRESISTORCALCULATOR_H
#define SMDRESISTORCALCULATOR_H

#include <QString>
#include <QLocale>

class SmdResistorCalculator
{
public:
    SmdResistorCalculator();
    double parse(const QString &text, bool * ok = 0) const;
private:
    double parseEIACode(const QString & text, bool *ok) const;

    const QLocale _cLocale;
};

#endif // SMDRESISTORCALCULATOR_H
