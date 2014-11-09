#ifndef PARTDIALOG_H
#define PARTDIALOG_H

#include <QDialog>
#include <QModelIndex>

namespace Ui {
class PartDialog;
}

class QSqlRelationalTableModel;
class QSqlTableModel;
class QModelIndex;
class QDataWidgetMapper;

class PartDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit PartDialog(QSqlTableModel *model, bool addMode, QWidget *parent = 0);
    ~PartDialog();
    void setCurrentModelIndex(const QModelIndex &index);
    int initialStock() const;
    double partPrice() const;
private slots:
    void on_buttonBox_accepted();

    void on_quickStorageButton_clicked();

private:
    Ui::PartDialog *ui;
    QDataWidgetMapper * _mapper;
    QModelIndex _currentModelIndex;
};

#endif // PARTDIALOG_H
