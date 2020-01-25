#include "rectselector.hpp"
#include "skin/positionattr.hpp"

ResizableGraphicsRectItem::ResizableGraphicsRectItem(QGraphicsItem* parent)
    : QGraphicsRectItem(parent)
    , m_xanchor(Coordinate::Type::Pixel)
    , m_yanchor(Coordinate::Type::Pixel)
{
    m_handles.append(new RectHandle(RectHandle::Top, this));
    m_handles.append(new RectHandle(RectHandle::Bottom, this));
    m_handles.append(new RectHandle(RectHandle::Left, this));
    m_handles.append(new RectHandle(RectHandle::Right, this));
    m_handles.append(new RectHandle(RectHandle::TopLeft, this));
    m_handles.append(new RectHandle(RectHandle::TopRight, this));
    m_handles.append(new RectHandle(RectHandle::BottomLeft, this));
    m_handles.append(new RectHandle(RectHandle::BottomRight, this));
    updateHandlesPos();
    setHandlesVisible(false);
}

void ResizableGraphicsRectItem::setHandlesVisible(bool visible)
{
    for (auto h : m_handles) {
        h->setVisible(visible);
        h->setEnabled(visible);
    }
}

void ResizableGraphicsRectItem::resizeRect(QPointF p, int handle)
{
    QRectF newRect = rect();
    // Depending on active handle resize rect accordingly
    // Ensure that left <= right and top <= bottom
    if (handle & RectHandle::Left) {
        if (m_xanchor == Coordinate::Type::Center) {
            newRect.setLeft(qMin(p.x(), rect().center().x()));
            newRect.setRight(rect().right() + rect().left() - newRect.left());
        } else {
            newRect.setLeft(qMin(p.x(), rect().right()));
        }
    }
    if (handle & RectHandle::Right) {
        if (m_xanchor == Coordinate::Type::Center) {
            newRect.setRight(qMax(rect().center().x(), p.x()));
            newRect.setLeft(rect().left() + rect().right() - newRect.right());
        } else {
            newRect.setRight(qMax(rect().left(), p.x()));
        }
    }
    if (handle & RectHandle::Top) {
        if (m_yanchor == Coordinate::Type::Center) {
            newRect.setTop(qMin(rect().center().y(), p.y()));
            newRect.setBottom(rect().bottom() + rect().top() - newRect.top());
        } else {
            newRect.setTop(qMin(rect().bottom(), p.y()));
        }
    }
    if (handle & RectHandle::Bottom) {
        if (m_yanchor == Coordinate::Type::Center) {
            newRect.setBottom(qMax(p.y(), rect().center().y()));
            newRect.setTop(rect().top() + rect().bottom() - newRect.bottom());
        } else {
            newRect.setBottom(qMax(p.y(), rect().top()));
        }
    }
    if (rect() != newRect) {
        resizeRectEvent(newRect);
    }
}

void ResizableGraphicsRectItem::updateHandlesPos()
{
    for (RectHandle* h : m_handles) {
        h->updatePosition(rect());
    }
}

void ResizableGraphicsRectItem::resizeRectEvent(const QRectF& r)
{
    // Preserve topLeft corner position in local coordinates,
    // because we want the child graphic items to move along with it for top and left resizing.
    QRectF newRect = r;
    setPos(mapToParent(newRect.topLeft() - rect().topLeft()));
    newRect.translate(rect().topLeft() - newRect.topLeft());
    setRect(newRect);
    updateHandlesPos();
}
