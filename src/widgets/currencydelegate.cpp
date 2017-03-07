#include "currencydelegate.h"
#include "constants.h"
#include <QSettings>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QBoxLayout>
#include "price.h"

QStringList CurrencyDelegate::_isoCodes;
QMap<QString,QString> CurrencyDelegate::_symbols;
CurrencyDelegate::_init CurrencyDelegate::_initializer;

CurrencyDelegate::_init::_init(){
    _isoCodes.append(QLatin1String("USD"));
    _isoCodes.append(QLatin1String("EUR"));
    _isoCodes.append(QLatin1String("JPY"));
    _isoCodes.append(QLatin1String("GBP"));
    _isoCodes.append(QLatin1String("AUD"));
    _isoCodes.append(QLatin1String("CAD"));
    _isoCodes.append(QLatin1String("CHF"));
    _isoCodes.append(QLatin1String("CNY"));

    QList<QLocale> locales = QLocale::matchingLocales(QLocale::AnyLanguage, QLocale::AnyScript, QLocale::AnyCountry);
    foreach (QLocale locale, locales) {
        const QString isoCode = locale.currencySymbol(QLocale::CurrencyIsoCode);
        if(!_symbols.contains(isoCode)){
            const QString symbol = locale.currencySymbol(QLocale::CurrencySymbol);
            _symbols[isoCode] = symbol;
        }
    }
}

class CurrencyEditor : public QWidget {
public:
    CurrencyEditor(const QStringList & currencyCodes, const QString & defaultCurrency, QWidget* parent = 0):
        QWidget(parent),
        _defaultCurrency(defaultCurrency)
    {
       _currenciesCombo = new QComboBox(this);
       _currenciesCombo->addItems(currencyCodes);
       _currenciesCombo->setEditable(true);
       _currenciesCombo->setInsertPolicy(QComboBox::InsertAtTop);
       _currenciesCombo->setDuplicatesEnabled(false);

       _valueSpinner = new QDoubleSpinBox(this);

       QHBoxLayout * layout = new QHBoxLayout;
       layout->addWidget(_currenciesCombo, 0);
       layout->addWidget(_valueSpinner, 1);
       layout->setMargin(0);
       layout->setSpacing(4);
       setLayout(layout);       
       _currenciesCombo->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
       _valueSpinner->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }

    void setValue(Price price)
    {
        _valueSpinner->setValue(price.value());
        QString currency = price.currency().isEmpty() ? _defaultCurrency : price.currency();
        int idx = _currenciesCombo->findText(currency);
        if(idx >= 0) {            
            _currenciesCombo->setCurrentIndex(idx);
        }
        else{
            _currenciesCombo->insertItem(0, currency);
            _currenciesCombo->setCurrentIndex(0);
        }
    }

    Price value() const
    {
        double aValue = _valueSpinner->value();
        QString currency = _currenciesCombo->currentText();
        if(currency.isEmpty()){
            currency = _defaultCurrency;
        }
        return Price(aValue, currency);
    }

private:
    QString _defaultCurrency;
    QComboBox * _currenciesCombo;
    QDoubleSpinBox * _valueSpinner;
};

CurrencyDelegate::CurrencyDelegate(QObject *parent) :
    QStyledItemDelegate(parent), _horizontalAlignment(Qt::AlignRight)
{
    QSettings settings;
    _defaultCurrency = settings.value(CURRENCY_CODE_KEY).toString();
    const QStringList & codes = currencyCodes();
    if(_defaultCurrency.isEmpty()){
        _defaultCurrency = codes.first();
    }    
}

QWidget * CurrencyDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;
    return new CurrencyEditor(currencyCodes(), _defaultCurrency, parent);
}

void CurrencyDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                                            const QModelIndex &index) const
{
    QStyledItemDelegate::updateEditorGeometry(editor, option, index);
}

void CurrencyDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QVariant value = index.data(Qt::EditRole);
    Price price = qvariant_cast<Price>(value);
    CurrencyEditor * currencyEditor = static_cast<CurrencyEditor *>(editor);
    currencyEditor->setValue(price);

}

QString CurrencyDelegate::displayText( const QVariant & value, const QLocale & locale ) const
{
    if(!value.isValid())
        return QString();
    double price;
    QString currency;
    if(value.canConvert<Price>()){
        Price aPrice = qvariant_cast<Price>(value);
        price = aPrice.value();
        currency = aPrice.currency();
    }
    else if(value.canConvert(QVariant::Double)){
        price = value.toDouble();
        currency = _defaultCurrency;
    }
    else {
        return QString();    
    }
    QString symbol = _symbols.value(currency, currency);
    return locale.toCurrencyString(price,symbol);
}

void CurrencyDelegate::initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const
{
    QStyledItemDelegate::initStyleOption(option, index);
    option->displayAlignment = option->displayAlignment | _horizontalAlignment;
}

void CurrencyDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{

    CurrencyEditor* currencyEditor = static_cast<CurrencyEditor*>(editor);
    Price price = currencyEditor->value();
    model->setData(index, QVariant::fromValue(price), Qt::EditRole);
}

