#ifndef QTREEBOX_H
#define QTREEBOX_H

#include <QComboBox>

/**
 * Code copied from a Nokia example
 * (http://developer.nokia.com/community/wiki/Implementing_QTreeView_in_QComboBox_using_Qt-_Part_2)
 * @brief The QTreeBox class
 */
class QTreeBox : public QComboBox
{
    Q_OBJECT
public:
    explicit QTreeBox(QWidget *parent = 0);
    virtual void showPopup();
    virtual void hidePopup();
signals:
    
public slots:
private:
    bool eventFilter(QObject* object, QEvent* event);
    bool _skipNextHide;
};

#endif // QTREEBOX_H
