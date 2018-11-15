#include "rectselector.hpp"
#include "skin/positionattr.hpp"

#include <QGraphicsScene>

RectSelector::RectSelector(QGraphicsItem* parent)
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
    setEnabled(false);
    setVisible(false);

    // interaction is done with RectHandles
    setFlag(ItemIsMovable, false);
    setFlag(ItemIsSelectable, false);
}

void RectSelector::resize(const QRectF& rectangle)
{
    QRectF r = rect();

    qreal dw;
    switch (m_xanchor) {
    case Coordinate::Type::Pixel:
    case Coordinate::Type::Percent:
        r.setLeft(rectangle.left());
        r.setRight(rectangle.right());
        break;
    case Coordinate::Type::Center:
        dw = rectangle.width() - r.width();
        r.adjust(-dw, 0., +dw, 0.);
        break;
    default:
        Q_ASSERT(false);
        break;
    }
    qreal dh;
    switch (m_yanchor) {
    case Coordinate::Type::Pixel:
    case Coordinate::Type::Percent:
        r.setTop(rectangle.top());
        r.setBottom(rectangle.bottom());
        break;
    case Coordinate::Type::Center:
        dh = rectangle.height() - r.height();
        r.adjust(0., -dh, 0., +dh);
        break;
    default:
        Q_ASSERT(false);
        break;
    }

    for (auto h : m_handles) {
        h->setFlag(ItemSendsGeometryChanges, false);
    }

    setRect(r);
    updateHandlesPos();

    for (auto h : m_handles) {
        h->setFlag(ItemSendsGeometryChanges, true);
    }

    emit rectChnaged(mapRectToScene(r));
}

void RectSelector::updateHandlesPos()
{
    for (RectHandle* h : m_handles) {
        h->updatePosition();
    }
}
