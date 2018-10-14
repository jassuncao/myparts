#ifndef OCTOPARTIMPORTDIALOG_H
#define OCTOPARTIMPORTDIALOG_H

#include <QDialog>

namespace Ui {
class OctopartImportDialog;
}

class OctopartImportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OctopartImportDialog(QWidget *parent = 0);
    ~OctopartImportDialog();

private:
    Ui::OctopartImportDialog *ui;
};

#endif // OCTOPARTIMPORTDIALOG_H
