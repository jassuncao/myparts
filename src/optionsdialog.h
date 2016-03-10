#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>
#include <QItemSelection>
#include <QAbstractItemDelegate>

namespace Ui {
class OptionsDialog;
}

class SimpleSqlTableModel;
class ModelsProvider;
class PartConditionModel;

class OptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OptionsDialog(ModelsProvider * provider, QWidget *parent = 0);
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
    void slotAddPartCondition();
    void slotPartConditionRowChanged(const QModelIndex &current, const QModelIndex &);
    void slotDeletePartCondition();
    void slotMakeCurrentConditionDefault();
    void slotConditionDoubleClick(const QModelIndex &index);
    void accept();
    void reject();
    void slotClosePartUnitEditor(QWidget *, QAbstractItemDelegate::EndEditHint hint = QAbstractItemDelegate::NoHint);
    void slotClosePartConditionEditor(QWidget *, QAbstractItemDelegate::EndEditHint hint = QAbstractItemDelegate::NoHint);
private:
    void setupModels();
    void setupPartUnitsModel();
    void setupParamsUnitsModel();
    void setupPartConditionModel();
    void setupConnections();
    void setupGeneralSettings();
    Ui::OptionsDialog *ui;
    ModelsProvider * _modelsProvider;
    SimpleSqlTableModel * _partUnitsModel;
    SimpleSqlTableModel * _parameterUnitsModel;
    PartConditionModel * _partConditionModel;
};

#endif // OPTIONSDIALOG_H
