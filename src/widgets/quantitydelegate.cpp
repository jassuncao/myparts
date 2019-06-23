#include "quantitydelegate.h"

QuantityDelegate::QuantityDelegate(QObject * parent) :
    QStyledItemDelegate(parent)
{
}

QuantityDelegate::~QuantityDelegate()
{
}

QString QuantityDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    bool ok;
    double quantity = value.toDouble(&ok);
    if(ok) {
        QByteArray buffer = QByteArray::number(quantity);
        if(!_unit.isNull()){
            buffer.append(' ').append(_unit);
        }
        return buffer;
    }
    return QStyledItemDelegate::displayText(value, locale);
}

void QuantityDelegate::setUnit(const QString &unit)
{
    _unit = unit;
}
