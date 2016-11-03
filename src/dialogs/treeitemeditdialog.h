#ifndef TREEITEMEDITDIALOG_H
#define TREEITEMEDITDIALOG_H

#include <QDialog>

class QLineEdit;
class QPlainTextEdit;
class QFormLayout;
class QComboBox;
class QAbstractItemModel;
class IconsRepository;

class TreeItemEditDialog : public QDialog
{
    Q_OBJECT
public:
    explicit TreeItemEditDialog(IconsRepository * iconsRepo, QWidget *parent = 0);
    ~TreeItemEditDialog();
    void setItemName(const QString & name);
    QString itemName() const;
    void setItemDescription(const QString & description);
    QString itemDescription() const;    
    void setItemIconId(QString iconId);
    QString itemIconId() const;
public slots:
    void accept();
    void reject();

private:
    QLineEdit *_nameEdit;
    QPlainTextEdit *_descriptionEdit;
    QComboBox * _iconCombo;
    QFormLayout * _formLayout;
    QString _defaultIconId;
};

#endif // TREEITEMEDITDIALOG_H
