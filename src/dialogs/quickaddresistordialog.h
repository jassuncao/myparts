#ifndef QUICKADDRESISTORDIALOG_H
#define QUICKADDRESISTORDIALOG_H

#include <QDialog>
#include "utils.h"
#include "models/parameter.h"

class ColorQComboBox;
class PartsSqlTableModel;
class TreeItemModel;
class ModelsProvider;
class PartParameterTableModel;

namespace Ui {
class QuickAddResistorDialog;
}

class QuickAddResistorDialog : public QDialog
{
    Q_OBJECT
public:
    explicit QuickAddResistorDialog(ModelsProvider *modelsProvider, QWidget *parent = 0);
    ~QuickAddResistorDialog();
protected slots:
    void slotBandChanged(int);
    void slotToleranceBandChanged(int);
    void slotFiveBandToggled(bool checked);
    void slotReset();
    void slotAddResistor();
private:
    QVariant getSelectedCategory() const;
    QVariant getSelectedStorage() const;
    QVariant getSelectedCondition() const;

    Ui::QuickAddResistorDialog *ui;
    PartsSqlTableModel * _partsModel;
    PartParameterTableModel * _partParams;
    QString _resistorNameTemplate;
    Parameter _resistanceParam;
    Parameter _powerRatingParam;
    Parameter _resistorToleranceParam;
};

#endif // QUICKADDRESISTORDIALOG_H
