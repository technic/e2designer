#pragma once

#include "model/bordersmodel.hpp"
#include <QGraphicsRectItem>

class BorderView : public QObject, public QGraphicsRectItem
{
    Q_OBJECT

    using bp = Property::BorderPosition;

public:
    BorderView(QGraphicsRectItem* parent);
    void setBorderSet(BorderStorage* bs);
    void setInnerRect(const QRectF& innerRect);

    // QGraphicsItem interface
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    void adjust();
    QRectF m_rect;
};
