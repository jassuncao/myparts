#ifndef QSEARCHLINEEDIT_H
#define QSEARCHLINEEDIT_H


#include "widgets_global.h"
#include <QLineEdit>
#include <QIcon>
#include <QRect>
#include "qprogressindicator.h"

class QToolButton;
class QPaintEvent;


class WIDGETS_SHARED_EXPORT QSearchLineEdit  : public QLineEdit
{
    Q_OBJECT
    Q_PROPERTY(QIcon searchIcon READ searchIcon WRITE setSearchIcon)
    Q_PROPERTY(QIcon clearIcon READ clearIcon WRITE setClearIcon)
public:
    explicit QSearchLineEdit(QWidget *parent = 0);
    bool isBusy() const;
    void setBusy(bool busy);    
    QIcon searchIcon() const;
    void setSearchIcon(const QIcon &icon);
    QIcon clearIcon() const;
    void setClearIcon(const QIcon &icon);
#ifdef WITH_PROGRESS
    QProgressIndicator *progressIndicator() const;
#endif
    QSize minimumSizeHint() const;
signals:
    void textClear();    
protected:
    void resizeEvent(QResizeEvent *);
    void keyPressEvent(QKeyEvent *event);
    void paintEvent(QPaintEvent *event);
    void updateTextMargins();
    void updateSearchIconGeometry();
private slots:
    void updateClearButton(const QString &text);
    void slotClearText();
private:
    QIcon _searchIcon;
    QSize _searchIconSize;
    QToolButton *_clearButton;
    QSize _styleMargins;
#ifdef WITH_PROGRESS
    QProgressIndicator *_progressIndicator;
#endif
};

#endif // QSEARCHLINEEDIT_H
