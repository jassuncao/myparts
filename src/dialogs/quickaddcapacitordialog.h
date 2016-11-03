#ifndef QUICKADDCAPACITORDIALOG_H
#define QUICKADDCAPACITORDIALOG_H

#include <QDialog>
#include "models/parameter.h"

class PartsSqlTableModel;
class ModelsRepository;
class PartParameterTableModel;
class PartStockTableModel;

namespace Ui {
class QuickAddCapacitorDialog;
}

class QuickAddCapacitorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QuickAddCapacitorDialog(ModelsRepository *modelsProvider, QWidget *parent = 0);
    void setSelectedCategory(const QVariant & category);
    void setSelectedStorage(const QVariant & storage);
    ~QuickAddCapacitorDialog();
protected slots:
    void slotReset();
    void slotAddCapacitor();
    void showError(const QString& errorMessage);
    void showSuccess(const QString& successMessage);
protected:
    bool eventFilter(QObject *obj, QEvent *event);
private:    
    QVariant selectedCategory() const;
    QVariant selectedStorage() const;
    QVariant selectedCondition() const;
    QVariant selectedPackage() const;
    void attemptAutoComplete();

    Ui::QuickAddCapacitorDialog *ui;
    PartsSqlTableModel * _partsModel;
    PartParameterTableModel * _partParams;
    PartStockTableModel * _partStockModel;
    QString _capacitorNameTemplate;
    Parameter _capacitanceParam;
    Parameter _voltageRatingParam;
    Parameter _toleranceParam;
};

#endif // QUICKADDCAPACITORDIALOG_H
