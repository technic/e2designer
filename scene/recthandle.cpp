#include <QCursor>
#include <QGraphicsSceneMouseEvent>
#include <QBrush>
//#include <QGraphicsScene>
#include "recthandle.hpp"
#include "widgetview.hpp"

RectHandle::RectHandle(int position, RectSelector *parent) :
    QGraphicsRectItem(parent),
    m_parent(parent),
    m_place(position)
{
    setFlags(ItemIsMovable | ItemSendsGeometryChanges);
    setFlag(ItemIsSelectable, false);

    // TODO: scale handles depending on QGraphicsView scale
    static_assert(handle_size % 2 == 0, "HandleItem size must be even");
    setRect(-handle_size/2, -handle_size/2, handle_size, handle_size);
    setBrush(QBrush(Qt::lightGray));
    updatePosition();

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

QVariant RectHandle::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange) {
        QPointF p = value.toPointF();
        p = restrictPosition(p);

//        QRectF r = m_parent->rect();
//        if (m_place & Left)
//            r.setLeft(p.x());
//        if (m_place & Right)
//            r.setRight(p.x());
//        if (m_place & Top)
//            r.setTop(p.y());
//        if (m_place & Bottom)
//            r.setBottom(p.y());

//        qDebug() << "New Rect" << r;
//        m_parent->resize(r);

        return QGraphicsRectItem::itemChange(change, p);
    }
    if (change == ItemPositionHasChanged) {
        QPointF point = value.toPointF();

        QRectF r = m_parent->rect();
        if (m_place & Left)
            r.setLeft(point.x());
        if (m_place & Right)
            r.setRight(point.x());
        if (m_place & Top)
            r.setTop(point.y());
        if (m_place & Bottom)
            r.setBottom(point.y());
        m_parent->resize(r);

        return QGraphicsRectItem::itemChange(change, value);
    }
    return QGraphicsRectItem::itemChange(change, value);
}

QPointF RectHandle::restrictPosition(QPointF newPos)
{
    QPointF pos = this->pos();
    if (m_place & Left || m_place & Right) {
        pos.setX(newPos.x());
    }
    if (m_place & Top || m_place & Bottom) {
        pos.setY(newPos.y());
    }
    const QRectF &r = m_parent->rect();
    if (m_place & Left && pos.x() > r.right()) {
        pos.setX(r.right());
    }
    if (m_place & Right && pos.x() < r.left()) {
        pos.setX(r.left());
    }
    if (m_place & Top && pos.y() > r.bottom()) {
        pos.setY(r.bottom());
    }
    if (m_place & Bottom && pos.y() < r.top()) {
        pos.setY(r.top());
    }
    return pos;
}

void RectHandle::updatePosition()
{
    setFlag(ItemSendsGeometryChanges, false);
    const QRectF &r = m_parent->rect();
    switch (m_place) {
    case TopLeft:
        setPos(r.topLeft());
        break;
    case Top:
        setPos((r.left() + r.right())/2, r.top());
        break;
    case TopRight:
        setPos(r.topRight());
        break;
    case Right:
        setPos(r.right(), (r.top() + r.bottom())/2);
        break;
    case BottomRight:
        setPos(r.bottomRight());
        break;
    case Bottom:
        setPos((r.left() + r.right())/2, r.bottom());
        break;
    case BottomLeft:
        setPos(r.bottomLeft());
        break;
    case Left:
        setPos(r.left(), (r.top() + r.bottom())/2);
        break;
    default:
        break;
    }
    setFlag(ItemSendsGeometryChanges, true);
}

void RectHandle::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
//        m_parent->setResizeStatus(true);
    }
    return QGraphicsRectItem::mousePressEvent(event);
}

void RectHandle::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
//        m_parent->setResizeStatus(false);
    }
    return QGraphicsRectItem::mouseReleaseEvent(event);
}
