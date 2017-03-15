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

class PartDetailsView : public QWidget
{
    Q_OBJECT
public:
    explicit PartDetailsView(QWidget *parent = 0);
    ~PartDetailsView();
    void setPartsModel(PartsSqlTableModel *model);
signals:    
    void editPartSelected();
public slots:
    void setCurrentIndex(const QModelIndex &current);
private slots:
    void onAddStock();
    void onRemoveStock();
    void onEditPart();
private:
    Ui::PartDetailsView *ui;
    QDataWidgetMapper * _widgetMapper;
    PartsSqlTableModel * _partsModel;
    PartStockLogTableModel * _partStockLogModel;
    PartStockTableModel2 * _partStockModel;
    QModelIndex _currentIndex;
    void updateStockView(const QModelIndex & current);    
};

#endif // PARTDETAILSVIEW_H
