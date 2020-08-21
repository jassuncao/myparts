#include "currencyhelper.h"

#include <QLocale>

CurrencyHelper& CurrencyHelper::instance()
{
    static CurrencyHelper singleton;
    return singleton;
}

CurrencyHelper::CurrencyHelper()
{
    QList<QLocale> locales = QLocale::matchingLocales(QLocale::AnyLanguage, QLocale::AnyScript, QLocale::AnyCountry);

    foreach (QLocale locale, locales) {
        QString isoCode = locale.currencySymbol(QLocale::CurrencyIsoCode);
        if(!_uniqueCurrencies.contains(isoCode)){

            QString symbol = locale.currencySymbol(QLocale::CurrencySymbol);
            /*
            if(!symbol.isEmpty() && symbol.at(symbol.length()-1).isLetter()){
                symbol = symbol+" ";
            }
            */
            _uniqueCurrencies.insert(isoCode, symbol);
        }
    }

    QLatin1String usd("USD");
    QLatin1String eur("EUR");
    QLatin1String jpy("JPY");
    QLatin1String gbp("GBP");
    QLatin1String aud("AUD");
    QLatin1String cad("CAD");
    QLatin1String chf("CHF");
    QLatin1String cny("CNY");

    _mostCommonCurrencies << usd << eur << jpy << gbp << aud << cad << chf << cny;

#ifdef DISAMBIGUATE_SYMBOLS

    _mostCommonCurrencies[usd] = QLatin1String("US $");
    _mostCommonCurrencies[aud] = QLatin1String("AU $");
    _mostCommonCurrencies[cad] = QLatin1String("CA $");
    _mostCommonCurrencies[chf] = QLatin1String("CHF");
    _mostCommonCurrencies[cny] = QString("RMB");
    /*
    _mostCommonCurrencies[eur] = QString(QChar(8364));
    _mostCommonCurrencies[jpy] = QString(QChar(165));
    _mostCommonCurrencies[gbp] = QString(QChar(163));
    */
#endif
}

QStringList CurrencyHelper::currencies(CurrencyCriteria criteria)
{
    return criteria == MostCommon ? _mostCommonCurrencies : _uniqueCurrencies.keys();
}

QString CurrencyHelper::currencySymbol(const QString & isoCode)
{
    return _uniqueCurrencies.value(isoCode, isoCode);
    /*
    QString symbol = _uniqueCurrencies.value(isoCode, isoCode);
    if(!symbol.isEmpty() && symbol.at(symbol.length()-1).isLetter()){
        symbol = symbol+" ";
    }
    return symbol;
    */
}

CurrencyHelper::CurrencySymbolPosition CurrencyHelper::symbolPosition(const QLocale & locale)
{
    QString s = locale.toCurrencyString(1, "$");
    int symbolIndex = s.indexOf("$");
    int numberIndex = s.indexOf("1");
    return (symbolIndex < numberIndex) ? Prefix : Suffix;
}
