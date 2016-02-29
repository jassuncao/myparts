#ifndef QUICKADDRESISTORDIALOG_H
#define QUICKADDRESISTORDIALOG_H

#include <QDialog>

class ColorQComboBox;
class PartsSqlTableModel;
class TreeItemModel;
class ModelsProvider;

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
    void slotChangeColor(int index);
private:
    Ui::QuickAddResistorDialog *ui;
};

#endif // QUICKADDRESISTORDIALOG_H
