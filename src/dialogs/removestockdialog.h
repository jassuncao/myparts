#ifndef REMOVESTOCKDIALOG_H
#define REMOVESTOCKDIALOG_H

#include <QDialog>

namespace Ui {
class RemoveStockDialog;
}

class RemoveStockDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit RemoveStockDialog(QWidget *parent = 0);
    ~RemoveStockDialog();
    int exec();
    void setPartUnit(const QString & partUnit);
    void setAvailableStock(int availableStock);
    int getStockChange() const;
    QString getComment() const;
private:
    Ui::RemoveStockDialog *ui;
};

#endif // REMOVESTOCKDIALOG_H
