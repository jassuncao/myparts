#ifndef TREEITEMEDITDIALOG_H
#define TREEITEMEDITDIALOG_H

#include <QDialog>

class QLineEdit;
class QPlainTextEdit;
class QFormLayout;
class QAbstractItemModel;

class TreeItemEditDialog : public QDialog
{
    Q_OBJECT
public:
    explicit TreeItemEditDialog(QWidget *parent = 0);
    ~TreeItemEditDialog();
    void setItemName(const QString & name);
    QString itemName() const;
    void setItemDescription(const QString & description);
    QString itemDescription() const;

public slots:
    void accept();
    void reject();

private:
    QLineEdit *_nameEdit;
    QPlainTextEdit *_descriptionEdit;
    QFormLayout * _formLayout;
};

#endif // TREEITEMEDITDIALOG_H
