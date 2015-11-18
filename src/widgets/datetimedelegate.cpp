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
    QDateTime utcDateTime;
    if(value.canConvert(QVariant::DateTime)){
        utcDateTime = value.toDateTime();
    }
    else if(value.canConvert(QVariant::UInt)){
        utcDateTime = QDateTime::fromTime_t(value.toUInt());
    }
    if(utcDateTime.isValid())
        return locale.toString(utcDateTime, QLocale::ShortFormat);
    return QString();
}

DateDelegate::DateDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

QString DateDelegate::displayText( const QVariant & value, const QLocale & locale ) const
{
    if(!value.isValid())
        return QString();
    QDateTime utcDateTime;
    if(value.canConvert(QVariant::DateTime)){
        utcDateTime = value.toDateTime();
    }
    else if(value.canConvert(QVariant::UInt)){
        utcDateTime = QDateTime::fromTime_t(value.toUInt());
    }
    if(utcDateTime.isValid())
        return locale.toString(utcDateTime.date(), QLocale::ShortFormat);
    return QString();
}

