#ifndef MOVESTOCKDIALOG_H
#define MOVESTOCKDIALOG_H

#include <QDialog>

namespace Ui {
class MoveStockDialog;
}

class ModelsRepository;

class MoveStockDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MoveStockDialog(ModelsRepository * modelsRepository, QWidget *parent = 0);
    ~MoveStockDialog();
    int exec();
    void setPartUnit(const QString & partUnit);
    void setAvailableStock(double availableStock);
    double stockToMove() const;
    QVariant selectedCondition() const;
    void setSelectedCondition(const QVariant & condition);
    QVariant selectedStorage() const;
    void setSelectedStorage(const QVariant & storage);
    QString comment() const;
private:
    Ui::MoveStockDialog *ui;
};

#endif // MOVESTOCKDIALOG_H
