#include "smdresistorcalculator.h"
#include <QString>
#include <QDebug>

#include <math.h>

#ifdef __MINGW32__

inline double pow10(int x){
    return pow(10, x);
}
#endif

static int EIA_VALUES[] = {
    100, 102, 105, 107, 110, 113, 115, 118, 121, 124, 127, 130, 133, 137,
    140, 143, 147, 150, 154, 158, 162, 165, 169, 174, 178, 182, 187, 191,
    196, 200, 205, 210, 215, 221, 226, 232, 237, 243, 249, 255, 261, 267,
    274, 280, 287, 294, 301, 309, 316, 324, 332, 340, 348, 357, 365, 374,
    383, 392, 402, 412, 422, 432, 442, 453, 464, 475, 487, 499, 511, 523,
    536, 549, 562, 576, 590, 604, 619, 634, 649, 665, 681, 698, 715, 732,
    750, 768, 787, 806, 825, 845, 866, 887, 909, 931, 953, 976,
};

struct EIAMuliplier {
    char code;
    double multiplier;
};

static EIAMuliplier EIA_MULTIPLIERS[] = { {'Z',0.001}, {'Y',0.01}, {'R',0.01},
    {'X',0.1}, {'S',0.1}, {'A',1}, {'B',10}, {'H',10}, {'C',100},
    {'D',1000}, {'E',10000}, {'F',100000},
};

static int EIA_MULTIPLIERS_COUNT = sizeof(EIA_MULTIPLIERS)/sizeof(EIAMuliplier);

inline char to_char(QChar c){
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    return c.toLatin1();
#else
    return c.toAscii();
#endif
}


SmdResistorCalculator::SmdResistorCalculator() :
    _cLocale(QLocale::C)
{

}


double SmdResistorCalculator::parse(const QString & text, bool *ok) const
{

    QString s = text.trimmed().toUpper();
    int len = s.length();
    double value(-1);
    bool valid(false);

    if(len == 3 && s.at(2).isLetter()){
        value = parseEIACode(s, &valid);
    }
    else if(len == 3 || len == 4){
        int decPoint = s.indexOf('R');
        if (decPoint >= 0){
            s = s.replace(decPoint, 1, '.');
            value = _cLocale.toDouble(s, &valid);
        }
        else{
            QString digitsStr = s.left(len - 1);
            QChar multiplierChar = s.at(len-1);
            int digits = _cLocale.toInt(digitsStr, &valid);
            if(valid && multiplierChar.isDigit()){
                int multiplier = to_char(multiplierChar) - '0';
                value = digits * pow10(multiplier);
                valid = true;
            }
        }
    }

    if(ok){
        *ok = valid;
    }
    return value;
}

double SmdResistorCalculator::parseEIACode(const QString & text, bool *ok) const
{
    Q_ASSERT(ok);
    bool valid;
    double value = -1;
    double multiplier = -1;

    QString codeStr = text.left(2);
    const char multiplierChar = to_char(text.at(2));
    int code = codeStr.toInt(&valid);

    for(int i=0; i<EIA_MULTIPLIERS_COUNT; ++i){
        if(multiplierChar == EIA_MULTIPLIERS[i].code){
            multiplier = EIA_MULTIPLIERS[i].multiplier;
            break;
        }
    }
    if(valid && code >= 0 && code <= 96 && multiplier > 0){
        int digits = EIA_VALUES[code - 1];
        value = digits * multiplier;
        valid = true;
    }
    *ok = valid;
    return value;
}
