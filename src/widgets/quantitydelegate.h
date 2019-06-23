#ifndef QUANTITYDELEGATE_H
#define QUANTITYDELEGATE_H

#include <QStyledItemDelegate>

class QuantityDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    QuantityDelegate(QObject * parent = 0);
    ~QuantityDelegate();
    virtual QString displayText(const QVariant &value, const QLocale &locale) const;
    void setUnit(const QString & unit);
private:
    QString _unit;
};


#endif // QUANTITYDELEGATE_H
