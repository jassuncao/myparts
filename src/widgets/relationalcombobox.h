#ifndef RELATIONALCOMBOBOX_H
#define RELATIONALCOMBOBOX_H

#include <QObject>
#include <QComboBox>
#include <QVariant>

class RelationalComboBox : public QComboBox
{
    Q_OBJECT
    Q_PROPERTY(QVariant currentKey READ currentKey WRITE setCurrentKey NOTIFY currentKeyChanged)
    Q_PROPERTY(int modelKeyColumn READ modelKeyColumn WRITE setModelKeyColumn)
public:
    explicit RelationalComboBox(QWidget *parent = 0);
    ~RelationalComboBox();

    int modelKeyColumn() const;
    void setModelKeyColumn(int keyColumn);
    QVariant currentKey() const;
    void setCurrentKey(const QVariant & key);
signals:
    void currentKeyChanged();
private:
    int _modelKeyColumn;
};

#endif // RELATIONALCOMBOBOX_H
