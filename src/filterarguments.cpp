#include "filterarguments.h"

FilterArguments::FilterArguments(QObject *parent) : QObject(parent)
{

}

FilterArguments::~FilterArguments()
{

}

void FilterArguments::setFilterByStorageEnabled(bool value)
{
    _filterByStorageEnabled = value;
}

void FilterArguments::setCategoryFilterMode(CategoryFilterMode mode)
{
    _categoryFilterMode = mode;
}

void FilterArguments::setDateFilterMode(DateFilterMode dateFilterMode)
{
    _dateFilterMode = dateFilterMode;
}

void FilterArguments::setSelectedDate(const QDate & date)
{
    QDateTime t(date);
    _selectedDateUtc = t.toUTC();
}

