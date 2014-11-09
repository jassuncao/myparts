#ifndef QUICKADDSTORAGEDIALOG_H
#define QUICKADDSTORAGEDIALOG_H

#include <QDialog>
#include <QVariant>
class QDialogButtonBox;
class QLineEdit;

class QuickAddStorageDialog : public QDialog
{
    Q_OBJECT
public:
    explicit QuickAddStorageDialog(QWidget *parent = 0);
    inline QVariant addedStorageId() const { return _addedStorageId;}
protected slots:
    void accept();
    void on_nameChanged(const QString &);
signals:
    
public slots:
private:
    QLineEdit * _nameText;
    QDialogButtonBox * _buttonBox;
    QVariant _addedStorageId;
    
};

#endif // QUICKADDSTORAGEDIALOG_H
