#ifndef RECTSELECTOR_H
#define RECTSELECTOR_H

#include "recthandle.hpp"
#include <QDebug>
#include <QGraphicsRectItem>

class RectSelector : public QObject, public QGraphicsRectItem
{
    Q_OBJECT

public:
    RectSelector(QGraphicsItem* parent);

    // functions to initalize initial size and position

    void setRect(const QRectF& rectangle)
    {
        QGraphicsRectItem::setRect(rectangle);
        updateHandlesPos();
    }

    void setSize(const QSizeF& size)
    {
        QRectF r = rect();
        r.setSize(size);
        setRect(r);
    }

    void setXanchor(int anchor) { m_xanchor = anchor; }
    void setYanchor(int anchor) { m_yanchor = anchor; }
    void resize(const QRectF& rectangle);

signals:
    void rectChnaged(QRectF globrect);

private:
    void updateHandlesPos();

    // QGraphicsItem owned
    QList<RectHandle*> m_handles;

    int m_xanchor, m_yanchor;
};

#endif // RECTSELECTOR_H
