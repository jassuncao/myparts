#ifndef FILTERITEMWIDGET_H
#define FILTERITEMWIDGET_H

#include <QWidget>

class QComboBox;
class QToolButton;
class QEvent;
class QAbstractItemModel;

class FilterItemWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FilterItemWidget(const QString &labelText, bool removableItem=false, QWidget *parent = 0);
    ~FilterItemWidget();
    QComboBox * comboBox() {return _comboBox;}
    void setOptionsModel(QAbstractItemModel * model, int visibleColumn);
signals:
    void deleteItem();
protected slots:
    void slotDeleteItem();
protected:
    bool eventFilter(QObject *obj, QEvent *event);
    QComboBox * _comboBox;
    QToolButton * _deleteBtn;
};

#endif // FILTERITEMWIDGET_H
