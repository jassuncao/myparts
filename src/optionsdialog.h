#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>

namespace Ui {
class OptionsDialog;
}

class QSqlTableModel;

class OptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OptionsDialog(QWidget *parent = 0);
    ~OptionsDialog();

private slots:
    void slotCurrentPageChanged(int currentRow);
private:
    void setupModels();
    void setupConnections();
    Ui::OptionsDialog *ui;
    QSqlTableModel * _partUnits;
};

#endif // OPTIONSDIALOG_H
