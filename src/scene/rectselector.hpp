#ifndef RECTSELECTOR_H
#define RECTSELECTOR_H

#include "recthandle.hpp"
#include "skin/positionattr.hpp"
#include <QDebug>
#include <QGraphicsRectItem>

class RectSelector : public QObject, public QGraphicsRectItem
{
    Q_OBJECT

public:
    RectSelector(QGraphicsItem* parent);

    // functions to initalize initial size and position

    void setSceneRect(const QRectF& rectangle)
    {
        QGraphicsRectItem::setRect(mapRectFromScene(rectangle));
        updateHandlesPos();
    }

    void setSize(const QSizeF& size)
    {
        QRectF r = rect();
        r.setSize(size);
        setRect(r);
        updateHandlesPos();
    }

    void setXanchor(Coordinate::Type anchor) { m_xanchor = anchor; }
    void setYanchor(Coordinate::Type anchor) { m_yanchor = anchor; }
    void resize(const QRectF& rectangle);

signals:
    void rectChanged(QRectF globrect);

private:
    void updateHandlesPos();

    // QGraphicsItem owned
    QList<RectHandle*> m_handles;

    Coordinate::Type m_xanchor, m_yanchor;
};

#endif // RECTSELECTOR_H
