#ifndef QUICKADDRESISTORDIALOG_H
#define QUICKADDRESISTORDIALOG_H

#include <QDialog>
#include "utils.h"

class ColorQComboBox;
class PartsSqlTableModel;
class TreeItemModel;
class ModelsProvider;
class PartParametersTableModel3;

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
    PartParametersTableModel3 * _partParams;
    QString _resistorNameTemplate;
    Utils::StandardUnit _resistanceUnit;
    Utils::StandardUnit _powerUnit;
    Utils::StandardUnit _toleranceUnit;
    QString _resistanceSymbol;
    QString _powerSymbol;
    QString _toleranceSymbol;
};

#endif // QUICKADDRESISTORDIALOG_H
