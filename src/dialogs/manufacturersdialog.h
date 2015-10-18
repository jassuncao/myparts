#ifndef MANUFACTURERSDIALOG_H
#define MANUFACTURERSDIALOG_H

#include <QDialog>

namespace Ui {
class ManufacturersDialog;
}

class ManufacturersDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ManufacturersDialog(QWidget *parent = 0);
    ~ManufacturersDialog();

private:
    Ui::ManufacturersDialog *ui;
};

#endif // MANUFACTURERSDIALOG_H
