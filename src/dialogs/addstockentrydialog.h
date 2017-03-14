#ifndef ADDSTOCKENTRYDIALOG_H
#define ADDSTOCKENTRYDIALOG_H

#include <QDialog>
#include <QVariant>

namespace Ui {
class AddStockEntryDialog;
}

class ModelsRepository;
class QSqlQueryModel;

class AddStockEntryDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddStockEntryDialog(ModelsRepository * modelsRepository, QWidget *parent = 0);
    ~AddStockEntryDialog();
    int exec();

    void setPartUnit(const QString & partUnit);

    QVariant selectedCondition() const;
    void setSelectedCondition(const QVariant & condition);

    QVariant selectedStorage() const;
    void setSelectedStorage(const QVariant & storage);

    int stockChange() const;
    double partPrice() const;
    QString comment() const;

private:
    void initCombos();
    Ui::AddStockEntryDialog *ui;
    QVariant _currentCondition;
    QVariant _currentStorage;
    ModelsRepository * _modelsRepository;
    QSqlQueryModel * _partConditionModel;
};

#endif // ADDSTOCKENTRYDIALOG_H
