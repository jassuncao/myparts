#ifndef PARTPARAMETERDIALOG_H
#define PARTPARAMETERDIALOG_H

#include <QDialog>
#include <QStyledItemDelegate>

class ParameterValueValidator;
class QComboBox;
class QSqlQueryModel;

namespace Ui {
class PartParameterDialog;
}

class PartParameterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PartParameterDialog(QWidget *parent = 0);
    ~PartParameterDialog();

private slots:
    void valueCurrentIndexChanged(int index);
private:
    Ui::PartParameterDialog *ui;
};

class ValueDelegate : public QStyledItemDelegate {
    Q_OBJECT
private slots:
    void valueCurrentIndexChanged(QString text);
public:
     ValueDelegate(QObject * parent = 0);
     virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;
     virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &) const;
     virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &) const;
};

class ParameterUnitDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
     ParameterUnitDelegate(QSqlQueryModel * unitsModel, QObject * parent = 0);
     virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;
     virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
     virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &) const;
private:
     QSqlQueryModel * _unitsModel;
};



#endif // PARTPARAMETERDIALOG_H
