#include <QBrush>
#include <QCursor>
#include <QGraphicsSceneMouseEvent>
//#include <QGraphicsScene>
#include "recthandle.hpp"
#include "widgetview.hpp"

RectHandle::RectHandle(int position, ResizableGraphicsRectItem* parent)
    : QGraphicsRectItem(parent)
    , m_place(position)
    , is_moving(false)
{
    // TODO: scale handles depending on QGraphicsView scale
    static_assert(handle_size % 2 == 0, "HandleItem size must be even");
    setRect(-handle_size / 2, -handle_size / 2, handle_size, handle_size);
    setBrush(QBrush(Qt::lightGray));

    switch (m_place) {
    case Top:
    case Bottom:
        setCursor(QCursor(Qt::CursorShape::SizeVerCursor));
        break;
    case Left:
    case Right:
        setCursor(QCursor(Qt::CursorShape::SizeHorCursor));
        break;
    case TopLeft:
    case BottomRight:
        setCursor(QCursor(Qt::CursorShape::SizeFDiagCursor));
        break;
    case TopRight:
    case BottomLeft:
        setCursor(QCursor(Qt::CursorShape::SizeBDiagCursor));
        break;
    }
}

void RectHandle::updatePosition(const QRectF& r)
{
    switch (m_place) {
    case TopLeft:
        setPos(r.topLeft());
        break;
    case Top:
        setPos((r.left() + r.right()) / 2, r.top());
        break;
    case TopRight:
        setPos(r.topRight());
        break;
    case Right:
        setPos(r.right(), (r.top() + r.bottom()) / 2);
        break;
    case BottomRight:
        setPos(r.bottomRight());
        break;
    case Bottom:
        setPos((r.left() + r.right()) / 2, r.bottom());
        break;
    case BottomLeft:
        setPos(r.bottomLeft());
        break;
    case Left:
        setPos(r.left(), (r.top() + r.bottom()) / 2);
        break;
    default:
        break;
    }
}

void RectHandle::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        event->accept();
        is_moving = true;
    } else {
        return QGraphicsRectItem::mousePressEvent(event);
    }
}

void RectHandle::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (is_moving) {
        event->accept();
        auto parent = dynamic_cast<ResizableGraphicsRectItem*>(parentItem());
        if (parent) {
            parent->resizeRect(mapToParent(event->pos()), flag());
        } else {
            qCritical() << "Orphan RectHandle item!";
        }
    } else {
        return QGraphicsRectItem::mouseMoveEvent(event);
    }
}

void RectHandle::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        event->accept();
        is_moving = false;
    } else {
        return QGraphicsRectItem::mouseReleaseEvent(event);
    }
}
