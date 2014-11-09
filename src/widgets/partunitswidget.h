#ifndef PARTUNITSWIDGET_H
#define PARTUNITSWIDGET_H

#include <QWidget>
#include <QModelIndex>
#include <QItemSelection>
class QPushButton;
class QSqlTableModel;
class QDialogButtonBox;
class QTableView;
class QSqlRecord;

class PartUnitsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PartUnitsWidget(QWidget *parent = 0);
    
signals:
    void dataChanged();
    void dataCommited();
public slots:
    bool submit();
    void revert();
private slots:
    void dataChanged(const QModelIndex, const QModelIndex);
    void selectionChanged(const QItemSelection &selected, const QItemSelection);
    void addUnit();
    void deleteUnit();
    void makeDefault();
private:
    QTableView * _view;
    QSqlTableModel * _model;
    QDialogButtonBox *_buttonBox;
    QPushButton * _addButton;
    QPushButton * _removeButton;
    QPushButton * _makeDefaultButton;
};

#endif // PARTUNITSWIDGET_H
