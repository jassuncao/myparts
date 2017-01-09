#ifndef PARTSASSIGNMENTDIALOG_H
#define PARTSASSIGNMENTDIALOG_H

#include <QDialog>

namespace Ui {
class PartsAssignmentDialog;
}

class PartsAssignmentDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PartsAssignmentDialog(QWidget *parent = 0);
    ~PartsAssignmentDialog();

private:
    Ui::PartsAssignmentDialog *ui;
};

#endif // PARTSASSIGNMENTDIALOG_H
