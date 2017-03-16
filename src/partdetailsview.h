#ifndef PARTDETAILSVIEW_H
#define PARTDETAILSVIEW_H

#include <QWidget>
#include <QModelIndex>

namespace Ui {
class PartDetailsView;
}

class PartStockLogTableModel;
class PartStockTableModel2;
class PartsSqlTableModel;
class QDataWidgetMapper;
class ModelsRepository;

class PartDetailsView : public QWidget
{
    Q_OBJECT
public:
    explicit PartDetailsView(QWidget *parent = 0);
    ~PartDetailsView();
    void setModelsRepository(ModelsRepository * repository);
signals:    
    void editPartSelected();
public slots:
    void setCurrentIndex(const QModelIndex &current);
private slots:
    void onAddStock();
    void onRemoveStock();
    void onEditPart();
private:
    void setPartsModel(PartsSqlTableModel *model);
    Ui::PartDetailsView *ui;
    QDataWidgetMapper * _widgetMapper;
    ModelsRepository * _modelsRepository;
    PartsSqlTableModel * _partsModel;
    PartStockLogTableModel * _partStockLogModel;
    PartStockTableModel2 * _partStockModel;
    QModelIndex _currentIndex;
    void updateStockView(const QModelIndex & current);    
};

#endif // PARTDETAILSVIEW_H
