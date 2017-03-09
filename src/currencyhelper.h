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

    static QStringList currencies(CurrencyCriteria criteria = CurrencyHelper::All);
    static QString currencySymbol(const QString & isoCode);
    static CurrencySymbolPosition symbolPosition(const QLocale & locale = QLocale());
private:
    CurrencyHelper();

    friend class _init;
    class _init {
    public:
        _init();
    };
    static _init _initializer;

    static QMap<QString,QString> _uniqueCurrencies;
    static QStringList _mostCommonCurrencies;
};

#endif // CURRENCYHELPER_H
