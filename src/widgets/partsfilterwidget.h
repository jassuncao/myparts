#ifndef PARTSFILTERWIDGET_H
#define PARTSFILTERWIDGET_H

#include <QWidget>
#include <QHash>
#include <QVariant>
#include <QAction>

class QHBoxLayout;
class FlowLayout;
class FilterItemWidget;
class QSearchLineEdit;
class QPushButton;
class QMenu;
class FilterBuilder;

class QFilterItemAction : public QAction {
    Q_OBJECT
public:
    explicit QFilterItemAction(const QString &text, const int filterTag, QObject* parent);
    int filterTag() const {return _filterTag;}
private slots:
    void slotToggled(bool b);
signals:
    void unchecked(const int filterTag);
    void checked(const int filterTag);
private:
    const int _filterTag;
};

class PartsFilterWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PartsFilterWidget(QWidget *parent = 0);
    ~PartsFilterWidget();
    void setFilterBuilder(FilterBuilder * filterBuilder);
signals:

public slots:
private slots:
    void slotDeleteFilterItem(const int filterTag);
    void slotFilterItemValueChange(const int filterTag, const QVariant & value);
    void slotRemoveFilterItem(const int filterTag);
    void slotAddFilterItem(const int filterTag);
    void slotTextFilterItemChanged();
private:
    QMenu * createFilterMoreMenu();
    QAction * createFilterItemAction(QMenu * menu, const QString & title, const int filterTag);
    void registerFilterItemWidget(FilterItemWidget * filterItem);

    FilterItemWidget * createStockFilterItem();
    FilterItemWidget * createPartConditionFilterItem();
    FilterItemWidget * createPartDistributorFilterItem();
    FilterItemWidget * createPartManufacturerFilterItem();
    FilterItemWidget * createPartFootprintFilterItem();

    QHBoxLayout * _fixedItemsLayout;
    FlowLayout * _dynamicItemsLayout;   
    QSearchLineEdit * _textFilterItem;
    QPushButton * _moreButton;
    QHash<int, FilterItemWidget*> _activeFilterItems;
    FilterBuilder * _filterBuilder;
};

#endif // PARTSFILTERWIDGET_H
