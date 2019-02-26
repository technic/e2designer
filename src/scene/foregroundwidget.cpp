#include "foregroundwidget.hpp"
#include <QPainter>

void ForegroundWidget::paint(QPainter* painter,
                             const QStyleOptionGraphicsItem* option,
                             QWidget* widget)
{
    // no blending in the OSD layer
    painter->setCompositionMode(QPainter::CompositionMode_Source);
    painter->fillRect(rect(), QBrush(Qt::transparent));
    QGraphicsRectItem::paint(painter, option, widget);
}
