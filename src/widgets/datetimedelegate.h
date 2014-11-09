#ifndef DATETIMEDELEGATE_H
#define DATETIMEDELEGATE_H

#include <QStyledItemDelegate>

class DateTimeDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit DateTimeDelegate(QObject *parent = 0);
    QString displayText ( const QVariant & value, const QLocale & locale ) const;
signals:
    
public slots:
    
};

class DateDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit DateDelegate(QObject *parent = 0);
    QString displayText ( const QVariant & value, const QLocale & locale ) const;
signals:

public slots:

};


#endif // DATETIMEDELEGATE_H
