#include "borderview.hpp"
#include "repository/skinrepository.hpp"
#include <QPainter>

BorderView::BorderView(QGraphicsRectItem* parent)
    : QGraphicsRectItem(parent)
{
    connect(SkinRepository::borders(), &BorderStorage::changed, this, [this] {
        adjust();
        update();
    });
    setInnerRect(parent->rect());
    setPen(QPen(Qt::blue));
    setBrush(Qt::NoBrush);
}

void BorderView::setInnerRect(const QRectF& innerRect)
{
    m_rect = innerRect;
    adjust();
}

void BorderView::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    auto draw = [&](const QPointF position, const QSizeF size, const QPixmap& pixmap) {
        // qDebug() << "draw:" << pos << size;
        painter->drawPixmap(QRectF(position, size), pixmap, QRectF(QPointF(0, 0), size));
    };

    auto& borders = *SkinRepository::borders();

    const QRectF r = rect();
    qreal x = r.left();
    qreal xm = r.right();
    const qreal t = m_rect.top() - r.top();

    if (auto& px = borders[bp::bpTopLeft]; !px.isNull()) {
        auto sz = QSizeF(px.size()).boundedTo(QSizeF(xm - x, t));
        //        qDebug() << sz;
        draw(r.topLeft(), sz, px);
        x += sz.width();
    }
    if (auto& px = borders[bp::bpTopRight]; !px.isNull()) {
        auto sz = QSizeF(px.size()).boundedTo(QSizeF(xm - x, t));
        xm -= sz.width();
        draw(QPointF(xm, r.top()), sz, px);
    }
    if (auto& px = borders[bp::bpTop]; !px.isNull()) {
        if (xm > x) {
            painter->drawTiledPixmap(QRectF(x, r.top(), xm - x, px.height()), px);
        }
    }

    x = r.left();
    xm = r.right();
    const qreal b = r.bottom() - m_rect.bottom();

    if (auto& px = borders[bp::bpBottomLeft]; !px.isNull()) {
        auto sz = QSizeF(px.size()).boundedTo(QSizeF(xm - x, b));
        draw(QPointF(x, r.bottom() - sz.height()), sz, px);
        x += sz.width();
    }
    if (auto& px = borders[bp::bpBottomRight]; !px.isNull()) {
        auto sz = QSizeF(px.size()).boundedTo(QSizeF(xm - x, b));
        xm -= sz.width();
        draw(QPointF(xm, r.bottom() - sz.height()), sz, px);
    }
    if (auto& px = borders[bp::bpBottom]; !px.isNull()) {
        if (xm > x) {
            painter->drawTiledPixmap(QRectF(x, r.bottom() - px.height(), xm - x, px.height()), px);
        }
    }

    if (auto& px = borders[bp::bpLeft]; !px.isNull()) {
        auto y = r.top() + borders[bp::bpTopLeft].height();
        auto ym = r.bottom() - borders[bp::bpBottomLeft].height();
        QRectF target(QPointF(r.left(), y), QPointF(m_rect.left(), ym));
        painter->drawTiledPixmap(target, px);
    }

    if (auto& px = borders[bp::bpRight]; !px.isNull()) {
        auto y = r.top() + borders[bp::bpTopRight].height();
        auto ym = r.bottom() - borders[bp::bpBottomRight].height();
        QRectF target(QPointF(m_rect.right(), y), QPointF(r.right(), ym));
        painter->drawTiledPixmap(target, px);
    }

    QGraphicsRectItem::paint(painter, option, widget);
}

void BorderView::adjust()
{
    auto& borders = *SkinRepository::borders();
    auto t = qMax(borders[bp::bpTop].height(),
                  qMax(borders[bp::bpTopRight].height(), borders[bp::bpTopLeft].height()));
    auto b = qMax(borders[bp::bpBottom].height(),
                  qMax(borders[bp::bpBottomRight].height(), borders[bp::bpBottomLeft].height()));
    setRect(m_rect.adjusted(-borders[bp::bpLeft].width(), -t, borders[bp::bpRight].width(), b));
}
