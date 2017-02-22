#include "currencydelegate.h"
#include "constants.h"
#include <QSettings>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QBoxLayout>
#include "price.h"

class CurrencyEditor : public QWidget {
public:
    CurrencyEditor(QWidget* parent = 0): QWidget(parent){
       _currencies = new QComboBox(this);
       _currencies->addItem("EUR");
       _currencies->addItem("USD");

       _value = new QSpinBox(this);
       QHBoxLayout * layout = new QHBoxLayout;
       layout->addWidget(_currencies, 0);
       layout->addWidget(_value, 1);
       layout->setMargin(0);
       layout->setSpacing(4);
       setLayout(layout);
       //setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
    }

    void setValue(Price price){
        _value->setValue(price.value());
        int idx = _currencies->findText(price.currency());
        _currencies->setCurrentIndex(idx);
    }

private:
    QComboBox * _currencies;
    QSpinBox * _value;
};

CurrencyDelegate::CurrencyDelegate(QObject *parent) :
    QStyledItemDelegate(parent), _horizontalAlignment(Qt::AlignRight)
{
    QSettings settings;
    _symbol = settings.value(CURRENCY_SYMBOL_KEY).toString();
    QList<QLocale> allLocales = QLocale::matchingLocales(QLocale::AnyLanguage, QLocale::AnyScript, QLocale::AnyCountry);
    for(int i=0; i<allLocales.size(); ++i){
        const QString isoCode = allLocales.at(i).currencySymbol(QLocale::CurrencyIsoCode);
        const QString symbol = allLocales.at(i).currencySymbol(QLocale::CurrencySymbol);
        if(!_symbols.contains(isoCode)) {
            _symbols.insert(isoCode, symbol);
        }
    }
}

QWidget * CurrencyDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;
    return new CurrencyEditor(parent);
}

void CurrencyDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                                            const QModelIndex &index) const
{
    //QStyledItemDelegate::updateEditorGeometry(editor, option, index);
    qDebug()<<"Geom "<< option.rect;
    editor->setGeometry(option.rect);
}

/*
QSize CurrencyDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.data().canConvert<Price>()) {
        StarRating starRating = qvariant_cast<StarRating>(index.data());
        return starRating.sizeHint();
    } else {
        return QStyledItemDelegate::sizeHint(option, index);
    }
}
*/

void CurrencyDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QVariant value = index.data(Qt::EditRole);
    Price price = qvariant_cast<Price>(value);
    CurrencyEditor * currencyEditor = dynamic_cast<CurrencyEditor *>(editor);
    currencyEditor->setValue(price);

}

QString CurrencyDelegate::displayText( const QVariant & value, const QLocale & locale ) const
{
    if(!value.isValid())
        return QString();
    if(value.canConvert<Price>()){
        Price price = qvariant_cast<Price>(value);
        QString symbol = _symbols.value(price.currency());
        return locale.toCurrencyString(price.value(), symbol);
    }
    if(!value.canConvert(QVariant::Double))
        return QString();    
    double price = value.toDouble();
    return locale.toCurrencyString(price, _symbol);
}

void CurrencyDelegate::initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const
{
    QStyledItemDelegate::initStyleOption(option, index);
    option->displayAlignment = option->displayAlignment | _horizontalAlignment;
}

void CurrencyDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QStyledItemDelegate::setModelData(editor, model, index);
    /*
    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editor);
    const QString & text = lineEdit->text();
    bool ok;
    double value = text.toDouble(&ok);
    if(ok)
        model->setData(index, value, Qt::EditRole);
        */
}
