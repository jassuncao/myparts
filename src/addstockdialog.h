#ifndef ADDSTOCKDIALOG_H
#define ADDSTOCKDIALOG_H

#include <QDialog>

namespace Ui {
class AddStockDialog;
}

class AddStockDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit AddStockDialog(QWidget *parent = 0);
    ~AddStockDialog();
    void setPartUnit(const QString & partUnit);
    int getStockChange() const;
    double getPartPrice() const;
    QString getComment() const;
    
private:
    Ui::AddStockDialog *ui;
};

#endif // ADDSTOCKDIALOG_H
