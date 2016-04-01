#include "unitparser.h"
#include <QVarLengthArray>
#include <QTextStream>
#include <QLocale>
#include <QChar>
#include <QDebug>
#include <math.h>

static const ushort ALLOWED_SI_SYMBOLS[] = { 'k','K','M','G','T', 'P', 'E', 'Z', 'Y', 'm','u',0x03BC,'n', 'p', 'f',  'a',  'z',  'y'};
static const double SI_EXPONENTIALS[] = { 1E3,1E3,1E6,1E9,1E12,1E15,1E18,1E21,1E24,1E-3,1E-6,1E-6,1E-9,1-12,1E-15,1E-18,1E-21,1E-24};

static const ushort BIG_PREFIXES[] = {0,'k','M','G','T','P','E','Z','Y'};
static const ushort SMALL_PREFIXES[] = {0,'m',0x03BC,'n','p','f','a','z','y'};
static const double SMALLEST_VALUE = 10e-24;

static int findSIPrefix(int c){
    int count = sizeof(ALLOWED_SI_SYMBOLS)/sizeof(ushort);
    for(int i=0; i<count; ++i){
        if(ALLOWED_SI_SYMBOLS[i]==c)
            return i;
    }
    return -1;
}

static void removeTrailingZeros(QString & str, const QChar decimalPoint)
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

static float epsilonEqual( double a, double b, double epsilon )
{
    return fabsf( a - b ) < epsilon;
}

UnitParser::UnitParser()
{
}

//TODO: Handle expressions like 1/4
double UnitParser::parseUnit(const QString& input, bool * ok)
{
     QByteArray buff;
     buff.reserve(input.length());
     //QVarLengthArray<QChar, 16> charBuff(input.length());
     int idx = 0;
     int prefixIdx = -1;
     QLocale locale;
     QChar decimalPoint = locale.decimalPoint();
     bool decimalPointFound = false;

     int inputLen = input.length();
     //Skip white spaces
     while (idx < inputLen && input.at(idx).isSpace())
         ++idx;

     while(idx<inputLen){
         const QChar c = input.at(idx);
         if(c.isDigit()){
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
             buff.append(c.toLatin1());
#else
             buff.append(c.toAscii());
#endif
         }
         else if(c.isLetter() && prefixIdx<0){
            prefixIdx = findSIPrefix(c.unicode());
            if(prefixIdx<0){//Invalid char found
                if(ok!=0)
                    *ok=false;
                return 0;
            }
            if(!decimalPointFound){
                decimalPointFound = true;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
                buff.append(decimalPoint.toLatin1());
#else
                buff.append(decimalPoint.toAscii());
#endif
            }
         }
         else if(c==decimalPoint && !decimalPointFound){
             decimalPointFound = true;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
             buff.append(c.toLatin1());
#else
             buff.append(c.toAscii());
#endif
         }
         else{
             //Found something else.
             //Check if it is a white space in the next
             break;
         }
         ++idx;
     }

     //Check if the remaining chars are white spaces
     //Something else is considered invalid input
     for (; idx < inputLen; ++idx) {
        const QChar c = input.at(idx);
        if (!c.isSpace()){
            if(ok!=0)
                *ok=false;
            return 0;
        }
     }
     QString aux(buff);
     double value = locale.toDouble(aux, ok);

     if(prefixIdx>=0){
         value*=SI_EXPONENTIALS[prefixIdx];
     }   
     return value;
}


QString UnitParser::formatUnit(const double value, const QString & unit, const QLocale& locale)
{
    QString result;
    ushort prefix;
    QTextStream out(&result);
    out.setLocale(locale);

    if(value>0.999999999){
        double aux = value;
        int multiplier = 0;
        while(aux>1000){
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

QString UnitParser::formatPercentage(double value, QChar unit){
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

