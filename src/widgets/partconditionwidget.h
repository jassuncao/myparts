#ifndef PARTCONDITIONWIDGET_H
#define PARTCONDITIONWIDGET_H

#include <QWidget>
#include <QModelIndex>
#include <QItemSelection>
class QPushButton;
class QSqlTableModel;
class QDialogButtonBox;
class QTableView;
class QSqlRecord;

class PartConditionWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PartConditionWidget(QWidget *parent = 0);
    
signals:
    void dataChanged();
    void dataCommited();
public slots:
    bool submit();
    void revert();
private slots:
    void dataChanged(const QModelIndex, const QModelIndex);
    void selectionChanged(const QItemSelection &selected, const QItemSelection);
    void addElement();
    void deleteElement();
    void makeAsDefault();
private:
    QTableView * _view;
    QSqlTableModel * _model;
    QDialogButtonBox *_buttonBox;
    QPushButton * _addButton;
    QPushButton * _removeButton;
    QPushButton * _makeDefaultButton;
    
};

#endif // PARTCONDITIONWIDGET_H
