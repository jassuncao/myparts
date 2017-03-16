#ifndef QUICKADDRESISTORDIALOG_H
#define QUICKADDRESISTORDIALOG_H

#include <QDialog>
#include "models/parameter.h"
#include "smdresistorcalculator.h"

class PartsSqlTableModel;
class ModelsRepository;
class PartParameterTableModel;
class PartStockLogTableModel;
class PartStockTableModel2;
class SaveButtonHelper;

namespace Ui {
class QuickAddResistorDialog;
}

class QuickAddResistorDialog : public QDialog
{
    Q_OBJECT
public:
    explicit QuickAddResistorDialog(ModelsRepository *modelsProvider, QWidget *parent = 0);
    void setSelectedCategory(const QVariant & category);
    void setSelectedStorage(const QVariant & storage);
    ~QuickAddResistorDialog();
protected slots:
    void slotBandChanged(int);
    void slotToleranceBandChanged(int);
    void slotFiveBandToggled(bool checked);
    void slotReset();
    void slotAddResistor();
    void showError(const QString& errorMessage);
    void showSuccess(const QString& successMessage);
    void slotAttemptAutoComplete();
private:
    QVariant selectedCategory() const;
    QVariant selectedStorage() const;
    QVariant selectedCondition() const;
    QVariant selectedPackage() const;

    Ui::QuickAddResistorDialog *ui;
    PartsSqlTableModel * _partsModel;
    PartParameterTableModel * _partParams;
    PartStockLogTableModel * _partStockLogModel;
    PartStockTableModel2 * _partStockModel;
    QString _resistorNameTemplate;
    Parameter _resistanceParam;
    Parameter _powerRatingParam;
    Parameter _resistorToleranceParam;
    SmdResistorCalculator _smdResistorCalc;
    SaveButtonHelper * _saveButtonHelper;
};

#endif // QUICKADDRESISTORDIALOG_H
