#ifndef PARTMANAGERFORM_H
#define PARTMANAGERFORM_H

#include <QWidget>
#include <QModelIndex>
#include <QByteArray>
#include "parts/partssqlquerymodel2.h"

namespace Ui {
class PartManagerForm;
}

class CategoryTreeModel;
class QStandardItemModel;
class QStandardItem;
class QModelIndex;
class QMenu;
class QDataWidgetMapper;
class QAbstractButton;
class StockHistoryProxyModel;

class PartManagerForm : public QWidget
{
    Q_OBJECT
    
public:
    explicit PartManagerForm(QWidget *parent = 0);
    ~PartManagerForm();

private slots:
    void on_categoriesTreeView_customContextMenuRequested(const QPoint &pos);    
    void categoriesTreeView_currentChanged(const QModelIndex &current, const QModelIndex &);
    void on_actionAddCategory_triggered();
    void on_actionDeleteCategory_triggered();
    void on_actionEditCategory_triggered();
    void filterSplitterMoved(int, int index);
    void on_colapseFilterPanelButton_clicked();
    void on_expandFilterPanelButton_clicked();
    void partsTableViewHeaderContextMenu(QPoint point);
    void onToggleTableColumn(bool checked);
    void onAddStock();
    void onRemoveStock();
    void onEditPart();

    void partsTableView_currentRowChanged(const QModelIndex &current, const QModelIndex&);

    void on_filterButtonBox_clicked(QAbstractButton *button);

    void on_filterByStorageCombo_currentIndexChanged(int index);

    void on_filterByStorageCheck_toggled(bool checked);

    void on_filterByStorageCheck_stateChanged(int arg1);

    void on_lineEdit_returnPressed();

    void on_textSearchLineEdit_returnPressed();

    void on_addPartButton_clicked();

    void partCategoryChanged();

    void on_pushButton_2_clicked();

    void on_deletePartButton_clicked();

    void part_primeInsert(int row, QSqlRecord &record);

    void on_duplicateParteButton_clicked();

    void on_expandTreeButton_clicked();

    void on_collapseTreeButton_clicked();

    void dateFilterOperatorCombo_indexChanged(int index);

private:
    QAction * createShowColumnAction(QMenu *parentMenu, const int columnNumber);
    void updatePartDetailView(const QModelIndex & current);
    void updateStockView(const QModelIndex & current);

    QByteArray _savedfilterPanelState;
    Ui::PartManagerForm *ui;
    CategoryTreeModel * _categoriesTreeModel;
    QMenu * _categoriesContextMenu;
    //QMenu * _visibleColumnsContextMenu;
    QMenu * _tableHeaderContextMenu;
    QModelIndex _categoriesCtxMenuIndex; //Keeps the index of the item (if any) where the user right clicked
    //QPartsFilterModel * _filterModel;
    PartsSqlQueryModel2 * _partsModel;        
    FilterBuilder _filterBuilder;
    void createMenus();
    void appyFilter();
    void resetFilter();
};

#endif // PARTMANAGERFORM_H
