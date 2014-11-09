#ifndef PARTUNITDIALOG_H
#define PARTUNITDIALOG_H

#include <QDialog>
#include <QModelIndex>

class QLineEdit;
class QDataWidgetMapper;
class QFormLayout;
class QAbstractItemModel;
class QCheckBox;

class PartUnitDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PartUnitDialog(QAbstractItemModel *model, QWidget *parent = 0);
    void setCurrentModelIndex(const QModelIndex &index);
    void setRootIndex(const QModelIndex &index);
    void setCurrentIndex(const int i);
signals:

public slots:
    void accept();
    void reject();

private:
    void boldifyField(QWidget * widget);

    QLineEdit *_nameEdit;
    QLineEdit *_abbreviationEdit;
    QCheckBox *_defaultCheckBox;
    QFormLayout * _formLayout;
    QDataWidgetMapper * _widgetMapper;
    
};

#endif // PARTUNITDIALOG_H
