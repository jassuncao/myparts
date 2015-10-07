#ifndef CHECKEDHEADER_H
#define CHECKEDHEADER_H

#include <QObject>
#include <QHeaderView>


/**
 * @brief The CheckedHeader class
 * Original author: Yury Kisliak
 */
class CheckedHeader : public QHeaderView
{
    Q_OBJECT
public:
    explicit CheckedHeader(Qt::Orientation orientation, QWidget *parent = 0);
    void setChecked(bool checked);
signals:
    void toggled(bool checked);

protected:
    void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const;
    void mousePressEvent(QMouseEvent *event);
private:
    QRect checkBoxRect(const QRect &sourceRect) const;
    bool m_isOn;
};

#endif // CHECKEDHEADER_H
