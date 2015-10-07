#ifndef UNITFORMATTER_H
#define UNITFORMATTER_H

#include <qchar.h>
#include <qstring.h>

class UnitFormatter
{
public:
    UnitFormatter();
    ~UnitFormatter();

    const static ushort ohmSymbol = 0x2126;
   const static ushort microSymbol = 0x03BC;
   const static QChar resistance;
   const static QChar capacitance;
   const static QChar inductance;
   const static QChar power;
   const static QChar percentage;

   static QString format(double value);
   static QString format(double value, QChar unit);
   static QString formatPercentage(double value, QChar unit);

};

#endif // UNITFORMATTER_H
