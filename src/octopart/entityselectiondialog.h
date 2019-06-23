#ifndef ENTITYSELECTIONDIALOG_H
#define ENTITYSELECTIONDIALOG_H

#include <QDialog>
#include <QModelIndex>

namespace Ui {
class EntitySelectionDialog;
}

class QAbstractItemModel;

class EntitySelectionDialog : public QDialog
{    
    Q_OBJECT

public:
    enum SelectedOption{
        CREATE_NEW,
        USE_EXISTING
    } ;

    explicit EntitySelectionDialog(QWidget *parent = 0);
    ~EntitySelectionDialog();
    void setModel(QAbstractItemModel *model);
    EntitySelectionDialog::SelectedOption  selectedOption() const;
    QModelIndex selectedIndex(int modelColumn) const;
    void setTitle(const QString &title);
    void setMessage(const QString &message);
private slots:
    void slotHandleRadioButtons(bool checked);
private:
    Ui::EntitySelectionDialog *ui;
};

#endif // ENTITYSELECTIONDIALOG_H
