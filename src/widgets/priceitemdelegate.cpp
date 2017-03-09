#include "priceitemdelegate.h"
#include "constants.h"
#include <QSettings>
#include <QComboBox>
#include <QSpinBox>
#include <QBoxLayout>
#include "currencyhelper.h"
#include "price.h"

class PriceEditor : public QWidget {
public:
    PriceEditor(const QStringList & currencyCodes, const QString & defaultCurrency, QWidget* parent = 0):
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

       //_valueSpinner->setFocusProxy(this);
       setFocusPolicy(Qt::WheelFocus);
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
        _valueSpinner->interpretText();
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


PriceItemDelegate::PriceItemDelegate(bool useCustomEditor, QObject *parent) :
    QStyledItemDelegate(parent), _useCustomEditor(useCustomEditor), _horizontalAlignment(Qt::AlignRight)
{
    QSettings settings;
    _defaultCurrency = settings.value(CURRENCY_CODE_KEY).toString();    
    if(_defaultCurrency.isEmpty()){
        _defaultCurrency = CurrencyHelper::currencies(CurrencyHelper::MostCommon).first();
    }
}

PriceItemDelegate::~PriceItemDelegate()
{
}


QWidget * PriceItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(_useCustomEditor){
        if (!index.isValid())
            return 0;
        const QStringList isoCodes = CurrencyHelper::currencies(CurrencyHelper::MostCommon);
        return new PriceEditor(isoCodes, _defaultCurrency, parent);
    }
    else{
        return QStyledItemDelegate::createEditor(parent, option, index);
    }
}


void PriceItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                                            const QModelIndex &index) const
{
    QStyledItemDelegate::updateEditorGeometry(editor, option, index);
}

void PriceItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if(_useCustomEditor){
        PriceEditor* priceEditor = dynamic_cast<PriceEditor*>(editor);
        if(priceEditor){
            QVariant value = index.data(Qt::DisplayRole);
            Price price = qvariant_cast<Price>(value);
            priceEditor->setValue(price);
        }
        else{
            qWarning()<< "Invalid price editor.";
        }
    }
    else{
        QStyledItemDelegate::setEditorData(editor, index);
    }

}

QString PriceItemDelegate::displayText( const QVariant & value, const QLocale & locale ) const
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
    const QString symbol = CurrencyHelper::currencySymbol(currency);
    return locale.toCurrencyString(price, symbol);
}

void PriceItemDelegate::initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const
{
    QStyledItemDelegate::initStyleOption(option, index);
    option->displayAlignment = option->displayAlignment | _horizontalAlignment;
}

void PriceItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if(_useCustomEditor){
        PriceEditor* priceEditor = dynamic_cast<PriceEditor*>(editor);
        if(priceEditor){
            Price price = priceEditor->value();
            model->setData(index, QVariant::fromValue(price), Qt::EditRole);
        }
        else{
            qWarning()<< "Invalid price editor.";
        }
    }
    else{
        QStyledItemDelegate::setModelData(editor, model, index);
    }

}
