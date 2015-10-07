#ifndef PARTSFILTERFORM_H
#define PARTSFILTERFORM_H

#include <QWidget>
#include "parts/partssqlquerymodel2.h"
#include "filterarguments.h"

namespace Ui {
class PartsFilterForm;
}

class QAbstractButton;
class TreeItemModel;

class PartsFilterForm : public QWidget
{
    Q_OBJECT

public:
    explicit PartsFilterForm(QWidget *parent = 0);
    ~PartsFilterForm();
    inline void setFilterBuilder(FilterBuilder * filterBuilder){ _filterBuilder = filterBuilder; }
    void setStorageModel(TreeItemModel * storageModel);
signals:
    void filterChanged();
public slots:
    void resetFilter();
    void applyFilter();
private slots:
    void slotFormChanged();
    void slotFilterButtonBox_clicked(QAbstractButton *button);
    void slotDateFilterOperatorCombo_indexChanged(int index);
private:
    Ui::PartsFilterForm *ui;
    FilterBuilder * _filterBuilder;
};

#endif // PARTSFILTERFORM_H
