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
    QString filename() const;
    QString charsetName() const;
    QChar fieldDelimiter() const;
    bool includeHeader() const;
private slots:
    void onBrowse();
private:
    void loadCharsets();
    void loadDelimiter();
    Ui::TableExportDialog *ui;
};

#endif // TABLEEXPORTDIALOG_H
