#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>
#include <QItemSelection>
#include <QAbstractItemDelegate>

namespace Ui {
class OptionsDialog;
}

class SimpleSqlTableModel;

class OptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OptionsDialog(QWidget *parent = 0);
    ~OptionsDialog();

private slots:
    void slotCurrentPageChanged(int currentRow);
    void slotMakeCurrentUnitDefault();
    void slotUnitSelectionChanged(const QItemSelection &selected, const QItemSelection);
    void slotUnitDoubleClick(const QModelIndex &index);
    void slotParamUnitRowChanged(const QModelIndex &current, const QModelIndex &);
    void slotDataChanged();
    void slotApplyChanges();
    void slotAddUnit();
    void slotDeleteUnit();
    void slotAddParamUnit();
    void slotDeleteParamUnit();
    void accept();
    void reject();
    void slotCloseEditor(QWidget *, QAbstractItemDelegate::EndEditHint hint = QAbstractItemDelegate::NoHint);
private:
    void setupModels();
    void setupPartUnitsModel();
    void setupParamsUnitsModel();
    void setupConnections();
    Ui::OptionsDialog *ui;
    SimpleSqlTableModel * _partUnitsModel;
    SimpleSqlTableModel * _parameterUnitsModel;
};

#endif // OPTIONSDIALOG_H
