#ifndef PARTSFILTERWIDGET_H
#define PARTSFILTERWIDGET_H

#include <QWidget>

class QHBoxLayout;
class FlowLayout;
class FilterItemWidget;
class QSearchLineEdit;
class QPushButton;
class QMenu;

class PartsFilterWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PartsFilterWidget(QWidget *parent = 0);
    ~PartsFilterWidget();

signals:

public slots:
private slots:
    void slotAddFilterItem();
    void slotDeleteFilterItem();
private:
    void setupModels();
    QHBoxLayout * _fixedItemsLayout;
    FlowLayout * _dynamicItemsLayout;
    FilterItemWidget * _stockFilterItem;
    FilterItemWidget * _conditionFilterItem;
    FilterItemWidget * _distributorFilterItem;
    QSearchLineEdit * _textFilterItem;
    QPushButton * _moreButton;
    QMenu * _moreMenu;
};

#endif // PARTSFILTERWIDGET_H
