#ifndef RECTSELECTOR_H
#define RECTSELECTOR_H

#include "recthandle.hpp"
#include "skin/positionattr.hpp"
#include <QGraphicsRectItem>

class RectSelector : public QGraphicsRectItem
{
public:
    RectSelector(QGraphicsItem* parent);
    void setHandlesVisible(bool visible);

    /// To be called from ::RectHandle::mouseMoveEvent
    void resizeRect(QPointF p, int handle);

protected:
    void setXanchor(Coordinate::Type anchor) { m_xanchor = anchor; }
    void setYanchor(Coordinate::Type anchor) { m_yanchor = anchor; }
    void updateHandlesPos();
    virtual void resizeRectEvent(const QRectF& r);

private:
    // QGraphicsItem owned
    QList<RectHandle*> m_handles;

    Coordinate::Type m_xanchor, m_yanchor;
};

#endif // RECTSELECTOR_H
