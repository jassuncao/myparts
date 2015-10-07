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
class QMenu;
class QDataWidgetMapper;
class QAbstractButton;
class StockHistoryProxyModel;
class StorageTreeModel;

class PartManagerForm : public QWidget
{
    Q_OBJECT
    
public:
    explicit PartManagerForm(QWidget *parent = 0);
    ~PartManagerForm();

private slots:
    void on_mainTreeView_customContextMenuRequested(const QPoint &pos);
    void mainTreeView_currentChanged(const QModelIndex &current, const QModelIndex &);
    void on_actionAddCategory_triggered();
    void on_actionDeleteCategory_triggered();
    void on_actionEditCategory_triggered();
    void filterSplitterMoved(int, int index);
    void on_colapseFilterPanelButton_clicked();
    void on_expandFilterPanelButton_clicked();
    void partsTableViewHeaderContextMenu(QPoint point);
    void onToggleTableColumn(bool checked);

    void slotAddStock();
    void slotRemoveStock();
    void slotAddPart();
    void slotEditPart();
    void slotDeletePart();
    void slotDuplicatePart();
    void slotFilterChanged();
    void slotPartsModelPrimeInsert(int, QSqlRecord &record);

    void partsTableView_currentRowChanged(const QModelIndex &current, const QModelIndex&);

    void on_lineEdit_returnPressed();

    void on_textSearchLineEdit_returnPressed();

    void refreshPartsModel();

    void on_pushButton_2_clicked();

    void on_deletePartButton_clicked();



    //void on_duplicateParteButton_clicked();

    void on_expandTreeButton_clicked();

    void on_collapseTreeButton_clicked();    

    void navigator_customContextMenuRequested(const QPoint &);

private:
    QAction * createShowColumnAction(QMenu *parentMenu, const int columnNumber);
    void updatePartDetailView(const QModelIndex & current);
    void updateStockView(const QModelIndex & current);

    QByteArray _savedfilterPanelState;
    Ui::PartManagerForm *ui;
    CategoryTreeModel * _categoriesTreeModel;    
    StorageTreeModel * _storageTreeModel;
    PartsSqlQueryModel2 * _partsModel;
    QMenu * _categoriesContextMenu;
    QMenu * _tableHeaderContextMenu;
    QModelIndex _categoriesCtxMenuIndex; //Keeps the index of the item (if any) where the user right clicked       
    FilterBuilder _filterBuilder;
    void createMenus();
    void appyFilter();
    void resetFilter();
};

#endif // PARTMANAGERFORM_H
