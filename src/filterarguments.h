#ifndef FILTERARGUMENTS_H
#define FILTERARGUMENTS_H

#include <QObject>
#include <QDate>

class FilterArguments : public QObject
{        
    Q_OBJECT
public:
    enum CategoryFilterMode {
        AllCategories=1,
        SubCategories=2,
        SelectedCategory=3
    };

    enum DateFilterMode {
        DateFilterNone,
        DateFilterBefore,
        DateFilterOn,
        DateFilterAfter
    };

    explicit FilterArguments(QObject *parent = 0);
    ~FilterArguments();

signals:

public slots:
    void setFilterByStorageEnabled(bool value);
    void setCategoryFilterMode(CategoryFilterMode mode);
    void setDateFilterMode(DateFilterMode dateFilterMode);
    void setSelectedDate(const QDate & date);
private:
    bool _filterByStorageEnabled;
    CategoryFilterMode _categoryFilterMode;
    DateFilterMode _dateFilterMode;
    QDateTime _selectedDateUtc;
};

#endif // FILTERARGUMENTS_H
