#ifndef MULTISTORAGEDIALOG_H
#define MULTISTORAGEDIALOG_H

#include <QDialog>
#include <QStringList>

namespace Ui {
class MultiStorageDialog;
}

class MultiStorageDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MultiStorageDialog(QWidget *parent = 0);
    ~MultiStorageDialog();
    QStringList listOfNames() const;
protected slots:
    void slotUpdatePreview();
private:
    Ui::MultiStorageDialog *ui;
    QStringList _previewList;
};

#endif // MULTISTORAGEDIALOG_H
