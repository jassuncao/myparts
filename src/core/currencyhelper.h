#ifndef CURRENCYHELPER_H
#define CURRENCYHELPER_H

#include <QStringList>
#include <QMap>
#include <QLocale>

class CurrencyHelper
{    
public:
    enum CurrencyCriteria {
        All,
        MostCommon
    };

    enum CurrencySymbolPosition {
        Prefix,
        Suffix
    };
    static CurrencyHelper& instance();
    QStringList currencies(CurrencyCriteria criteria = CurrencyHelper::All);
    QString currencySymbol(const QString & isoCode);
    CurrencySymbolPosition symbolPosition(const QLocale & locale = QLocale());
private:
    CurrencyHelper();    

    QMap<QString,QString> _uniqueCurrencies;
    QStringList _mostCommonCurrencies;
};


#endif // CURRENCYHELPER_H
