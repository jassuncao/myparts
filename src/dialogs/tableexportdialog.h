#ifndef TABLEEXPORTDIALOG_H
#define TABLEEXPORTDIALOG_H

#include <QDialog>

namespace Ui {
class TableExportDialog;
}

class TableExportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TableExportDialog(QWidget *parent = 0);
    ~TableExportDialog();
private slots:
    void onBrowse();
private:
    void loadCharsets();
    Ui::TableExportDialog *ui;
};

#endif // TABLEEXPORTDIALOG_H
