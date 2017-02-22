#ifndef PRICE_H
#define PRICE_H

#include <QString>
#include <QMetaType>
#include <QDebug>

class Price
{    
public:
    explicit Price();
    explicit Price(double value, const QString & currency);
    bool isValid() const {return _valid;}
    double value() const  {return _value;}
    QString currency() const {return _currency;}
private:
    double _value;
    QString _currency;
    bool _valid;
};


Q_DECLARE_METATYPE(Price)

QDebug operator<<(QDebug dbg, const Price &value);

#endif // PRICE_H
