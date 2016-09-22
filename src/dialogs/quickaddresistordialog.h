#ifndef QUICKADDRESISTORDIALOG_H
#define QUICKADDRESISTORDIALOG_H

#include <QDialog>
#include "models/parameter.h"

//class ColorQComboBox;
class PartsSqlTableModel;
//class TreeItemModel;
class ModelsRepository;
class PartParameterTableModel;
class PartStockTableModel;

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
private:
    QVariant selectedCategory() const;
    QVariant selectedStorage() const;
    QVariant selectedCondition() const;

    Ui::QuickAddResistorDialog *ui;
    PartsSqlTableModel * _partsModel;
    PartParameterTableModel * _partParams;
    PartStockTableModel * _partStockModel;
    QString _resistorNameTemplate;
    Parameter _resistanceParam;
    Parameter _powerRatingParam;
    Parameter _resistorToleranceParam;
};

#endif // QUICKADDRESISTORDIALOG_H
