#ifndef FILTERITEMWIDGET_H
#define FILTERITEMWIDGET_H

#include <QWidget>
#include <QVariant>

class QComboBox;
class QToolButton;
class QEvent;
class QAbstractItemModel;

class FilterItemWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FilterItemWidget(const QString &labelText, const int filterTag, bool removableItem=false, QWidget *parent = 0);
    ~FilterItemWidget();
    void setOptionsModel(QAbstractItemModel * model);
    void setDisplayColumn(int displayColumn);
    void setValueColumn(int valueColumn, int role);    
    int filterTag() const {return _filterTag;}
    bool removable() const { return _deleteBtn!=0;}
signals:
    void deleteItem(const int filterTag);
    void valueChanged(const int filterTag, const QVariant & value);
public slots:
    void clear();
protected slots:
    void slotDeleteItem();    
    void slotCurrentIndexChanged(int row);
protected:
    bool eventFilter(QObject *obj, QEvent *event);
    const int _filterTag;
    QComboBox * _comboBox;
    QToolButton * _deleteBtn;    
    int _valueColumn;
    int _valueRole;

};

#endif // FILTERITEMWIDGET_H
