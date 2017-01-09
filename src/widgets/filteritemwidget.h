#ifndef FILTERITEMWIDGET_H
#define FILTERITEMWIDGET_H

#include <QWidget>
#include <QVariant>

class QComboBox;
class QToolButton;
class QEvent;
class QAbstractItemModel;
class TreeViewComboBox;

class FilterItemWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FilterItemWidget(const int filterTag, bool removableItem=false, QWidget *parent=0);
    ~FilterItemWidget();
    int filterTag() const {return _filterTag;}
    bool removable() const { return _removableItem;}
signals:
    void deleteItem(const int filterTag);
    void valueChanged(const int filterTag, const QVariant & value);
public slots:
    void reset();
    void deleteItem();
protected:
    virtual void doReset();
    //virtual void doDeleteItem();
    static QToolButton * createDeleteButton();
    const int _filterTag;
    const bool _removableItem;
};

class ComboFilterItemWidget : public FilterItemWidget
{
    Q_OBJECT
public:
    explicit ComboFilterItemWidget(const QString &labelText, const int filterTag, bool removableItem=false, QWidget *parent=0);
    ~ComboFilterItemWidget();
    void setOptionsModel(QAbstractItemModel * model);
    void setDisplayColumn(int displayColumn);
    void setValueColumn(int valueColumn, int role);    
protected slots:       
    void slotCurrentIndexChanged(int row);
protected:
    void doReset();
    bool eventFilter(QObject *obj, QEvent *event);
    QComboBox * _comboBox;
    QToolButton * _deleteBtn;
    int _valueColumn;
    int _valueRole;
};

class TreeComboFilterItemWidget : public FilterItemWidget {
    Q_OBJECT
public:
    explicit TreeComboFilterItemWidget(const QString &labelText, const int filterTag, bool removableItem=false, QWidget *parent=0);
    ~TreeComboFilterItemWidget();
    void setOptionsModel(QAbstractItemModel * model);
protected slots:
    void slotCurrentIndexChanged(int row);
protected:
    void slotDeleteItem();
    void doReset();
    bool eventFilter(QObject *obj, QEvent *event);
    TreeViewComboBox * _comboBox;
    QToolButton * _deleteBtn;
};

#endif // FILTERITEMWIDGET_H
