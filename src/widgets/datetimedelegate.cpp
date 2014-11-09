#include "datetimedelegate.h"
#include <QDateTime>
#include <QDebug>

DateTimeDelegate::DateTimeDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

QString DateTimeDelegate::displayText( const QVariant & value, const QLocale & locale ) const
{    
    if(!value.isValid())
        return QString();
    if(!value.canConvert(QVariant::LongLong))
        return QString();    
    QDateTime utcDateTime = QDateTime::fromTime_t(value.toLongLong());
    return utcDateTime.toLocalTime().toString(Qt::SystemLocaleShortDate);
}

DateDelegate::DateDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

QString DateDelegate::displayText( const QVariant & value, const QLocale & locale ) const
{
    if(!value.isValid())
        return QString();
    if(!value.canConvert(QVariant::LongLong))
        return QString();
    QDateTime utcDateTime = QDateTime::fromTime_t(value.toLongLong());
    QDate asDate = utcDateTime.toLocalTime().date();
    return asDate.toString(Qt::SystemLocaleShortDate);
}

