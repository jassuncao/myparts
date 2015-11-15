#ifndef PARTDETAILSVIEW_H
#define PARTDETAILSVIEW_H

#include <QWidget>
#include <QModelIndex>

namespace Ui {
class PartDetailsView;
}

class PartStockTableModel;

class PartDetailsView : public QWidget
{
    Q_OBJECT

public:
    explicit PartDetailsView(QWidget *parent = 0);
    ~PartDetailsView();
    void setPartsModel(QAbstractItemModel * model);
signals:
    void addStockSelected();
    void removeStockSelected();
    void editPartSelected();
public slots:
    void setCurrentIndex(const QModelIndex &current);
private slots:
    void onAddStock();
    void onRemoveStock();
    void onEditPart();
private:
    Ui::PartDetailsView *ui;
    QAbstractItemModel * _partsModel;
    PartStockTableModel * _partStockModel;
    void updatePartDetailView(const QModelIndex & current);
    void updateStockView(const QModelIndex & current);
    QString columnDisplayData(QAbstractItemModel * model, const QModelIndex & current, int column);
    QString columnDisplayData(QAbstractItemModel * model, const QModelIndex & current, int column1, int column2);
    QString columnDisplayDate(QAbstractItemModel * model, const QModelIndex & current, int column);
};

#endif // PARTDETAILSVIEW_H
