#ifndef PARTSMANAGERVIEW_H
#define PARTSMANAGERVIEW_H

#include "minisplitter.h"
#include <QModelIndex>
#include <QSqlRecord>

namespace Manhattan {
    class StyledBar;
}

class PartsTableView;
class PartDetailsView;
class CategoryTreeModel;
class StorageTreeModel;
class NavigationSubWidget;
class PartsFilterWidget;
class QActionPushButton;
class QPushButton;
class QToolButton;
class FilterBuilder;
class PartsSqlTableModel;
class PartsQueryBuilder;
class CategoryNavigator;
class StorageNavigator;
class CategoryTreeModel;
class TreeItem;
class PartsTableProxyModel;
class ModelsRepository;
class QSettings;

class PartsManagerView : public Manhattan::MiniSplitter
{
    Q_OBJECT
public:
    explicit PartsManagerView(ModelsRepository * modelsProvider, QWidget *parent = 0);
    ~PartsManagerView();
    QVariant selectedCategory() const;
    QVariant selectedStorage() const;
    void writeSettings(QSettings& settings) const;
    void readSettings(QSettings& settings);

protected slots:
    void slotBeforeSubmit();
    void slotAfterSubmit();    
    void slotPartTableCurrentRowChanged(const QModelIndex &current, const QModelIndex &);
    void slotAddPart();
    void slotEditPart();
    void slotDeletePart();
    void slotDuplicatePartAllData();
    void slotDuplicatePartBasicData();
    void slotNavModeChanged(int mode);
    void slotFilterChanged();
    void slotSelectedCategoryChanged(const QList<int> selectedIds);
    void slotSelectedStorageChanged(const QList<int> selectedIds);
    /*
    void slotPartsDroppedInCategory(QVector<int> parts, TreeItem* item);
    void slotPartsDroppedInStorage(QVector<int> parts, TreeItem* item);
    */
    void slotShowDetailsPane();
    void slotHideDetailsPane();
    void slotExportTable();
private:
    void initModels();
    void initPartsTableView();
    void duplicatePart(bool allData);

    NavigationSubWidget * _navWidget;
    PartsTableView * _partsTableView;
    PartDetailsView * _partDetailsView;
    //PartsSqlTableModel * _partsModel;

    /*
    CategoryTreeModel * _categoriesTreeModel;
    StorageTreeModel * _storageTreeModel;
    PartsQueryBuilder * _partsQueryBuilder;
    */
    PartsFilterWidget * _partsFilterWidget;
    QPushButton * _deletePartButton;
    QPushButton * _duplicatePartButton;

    CategoryNavigator * _categoryNavigator;
    StorageNavigator * _storageNavigator;
    QVariant _savedPartId;
    PartsTableProxyModel * _partsTableProxyModel;
    QToolButton * _showDetailsPaneButton;
    QToolButton * _hideDetailsPaneButton;
    QWidget * _detailsPane;
    ModelsRepository * _modelsRepository;
};

#endif // PARTSMANAGERVIEW_H
