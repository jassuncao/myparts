#include "unitformatter.h"
#include "qtextstream.h"
#include "qdebug.h"
#include <math.h>

static const ushort BIG_PREFIXES[] = {0,'k','M','G','T','P','E','Z','Y'};
static const ushort SMALL_PREFIXES[] = {0,'m',0x03BC,'n','p','f','a','z','y'};
static const double SMALLEST_VALUE = 10e-24;

UnitFormatter::UnitFormatter()
{
}

const QChar UnitFormatter::resistance(0x2126);
const QChar UnitFormatter::capacitance('F');
const QChar UnitFormatter::inductance('H');
const QChar UnitFormatter::power('W');
const QChar UnitFormatter::percentage('%');

static void removeTrailingZeros(QString & str, const QChar& decimalPoint)
{    
    int idx = str.indexOf(decimalPoint);
    if(idx<0) return;
    int i=str.length()-1;
    while(i>0){
        QChar c = str.at(i);
        if(c=='0'){
            --i;
        }
        else if(c==decimalPoint){
            --i;
            break;
        }
        else{
            break;
        }
    }
    str.resize(i+1);
}

QString UnitFormatter::format(double value)
{
    return format(value, 0);
}

float epsilonEqual( double a, double b, double epsilon )
{
    return fabsf( a - b ) < epsilon;
}

QString UnitFormatter::format(const double value, QString unit)
{
    QLocale locale;
    QString result;
    ushort prefix;
    QTextStream out(&result);
    out.setLocale(locale);
    if(value>0.999999999){
        double aux = value;
        int multiplier = 0;
        while(aux>=1000){
            ++multiplier;
            aux=aux/1000;
        }
        prefix = BIG_PREFIXES[multiplier];
        out.setRealNumberPrecision(2);
        out.setNumberFlags(out.numberFlags() & ~QTextStream::ForcePoint);
        out.setRealNumberNotation(QTextStream::FixedNotation);
        out<<aux;
    }
    else {
        double aux = value;
        int divider = 0;
        if(aux>SMALLEST_VALUE){
            while(aux<0.999999999){
                ++divider;
                aux=aux*1000;
            }
        }
        prefix = SMALL_PREFIXES[divider];
        out.setRealNumberPrecision(2);
        out.setRealNumberNotation(QTextStream::FixedNotation);
        out<<aux;
    }
    removeTrailingZeros(result, locale.decimalPoint());
    if(prefix){
        out<<QChar(prefix);
    }
    if(unit!=0){
        out<<unit;
    }
    return result;
}

QString UnitFormatter::formatPercentage(double value, QString unit){
    QString result;
    QTextStream out(&result);
    out.setRealNumberPrecision(4);
    out.setRealNumberNotation(QTextStream::SmartNotation);
    out<<value;
    if(unit!=0){
        out<<unit;
    }
    return result;
}

