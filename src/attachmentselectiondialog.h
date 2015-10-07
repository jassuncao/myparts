#ifndef ATTACHMENTSELECTIONDIALOG_H
#define ATTACHMENTSELECTIONDIALOG_H

#include <QDialog>
#include <QUrl>

namespace Ui {
class AttachmentSelectionDialog;
}

class AttachmentSelectionDialog : public QDialog
{
    Q_OBJECT

public:
    typedef enum {
        LocalResource,
        RemoteResource
    } ResourceKind;
    explicit AttachmentSelectionDialog(QWidget *parent = 0);
    ~AttachmentSelectionDialog();
    ResourceKind resourceKind();
    QUrl value() const;
private slots:
    void slotBrowseButtonClicked();
private:
    Ui::AttachmentSelectionDialog *ui;
};

#endif // ATTACHMENTSELECTIONDIALOG_H
