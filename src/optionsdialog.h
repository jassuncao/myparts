#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>
#include <QItemSelection>
#include <QAbstractItemDelegate>
#include <QMap>

namespace Ui {
class OptionsDialog;
}

class SimpleSqlTableModel;
class ModelsRepository;
class PartConditionModel;
class ParameterModel;
class ParameterUnitModel;

class OptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OptionsDialog(ModelsRepository * provider, QWidget *parent = 0);
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
    void slotAddParameter();
    void slotDeleteParameter();
    void slotParameterRowChanged(const QModelIndex &current, const QModelIndex &);
    void slotCurrencyChanged(const QString &currency);
private:
    void setupModels();
    void setupPartUnitsModel();
    void setupParamsUnitsModel();
    void setupParametersModel();
    void setupPartConditionModel();
    void setupCurrenciesModel();
    void setupConnections();
    void setupGeneralSettings();
    Ui::OptionsDialog *ui;
    ModelsRepository * _modelsProvider;
    SimpleSqlTableModel * _partUnitsModel;
    ParameterUnitModel * _parameterUnitsModel;
    PartConditionModel * _partConditionModel;
    ParameterModel * _parameterModel;    
};

#endif // OPTIONSDIALOG_H
