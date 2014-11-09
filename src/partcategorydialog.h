#ifndef PARTCATEGORYDIALOG_H
#define PARTCATEGORYDIALOG_H

#include <QDialog>
#include <QModelIndex>

class QLineEdit;
class QPlainTextEdit;
class QDataWidgetMapper;
class QFormLayout;
class QAbstractItemModel;

class PartCategoryDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PartCategoryDialog(QAbstractItemModel *model, QWidget *parent = 0);
    void setCurrentModelIndex(const QModelIndex &index);
    void setRootIndex(const QModelIndex &index);
    void setCurrentIndex(const int i);
signals:
    
public slots:
    void accept();
    void reject();

private:
    QLineEdit *_nameEdit;
    QPlainTextEdit *_descriptionEdit;
    QFormLayout * _formLayout;
    QDataWidgetMapper * _widgetMapper;
};

#endif // PARTCATEGORYDIALOG_H
