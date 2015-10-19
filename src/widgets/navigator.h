#ifndef NAVIGATOR_H
#define NAVIGATOR_H

#include <QListView>

class Navigator : public QListView
{
    Q_OBJECT

public:
    explicit Navigator(QWidget *parent = 0);

    int iconSize() const
    {
        return mIconSize;
    }

    bool showIcons() const
    {
        return mShowIcons;
    }

    bool showText() const
    {
        return mShowText;
    }

    QSize sizeHint() const;


protected:
    void dragEnterEvent(QDragEnterEvent *event) ;
    void dragMoveEvent(QDragMoveEvent *event) ;
    void dropEvent(QDropEvent *event) ;
    void showEvent(QShowEvent *event) ;

private Q_SLOTS:
    void slotCurrentChanged(const QModelIndex &current);
    void slotActionTriggered(bool checked);
    void updateNavigatorSize();

private:
    void setHelpText(QAction *act, const QString &text);

    int mIconSize;
    bool mShowIcons;
    bool mShowText;

    QAction *mShowIconsAction;
    QAction *mShowTextAction;
    QAction *mShowBothAction;
    QAction *mBigIconsAction;
    QAction *mNormalIconsAction;
    QAction *mSmallIconsAction;
};
#endif // NAVIGATOR_H
