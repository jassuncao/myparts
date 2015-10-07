#ifndef STORAGEMANAGERFORM_H
#define STORAGEMANAGERFORM_H

#include <QWidget>

namespace Ui {
class StorageManagerForm;
}

class StorageManagerForm : public QWidget
{
    Q_OBJECT

public:
    explicit StorageManagerForm(QWidget *parent = 0);
    ~StorageManagerForm();

private:
    Ui::StorageManagerForm *ui;
};

#endif // STORAGEMANAGERFORM_H
