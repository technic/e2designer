#include "borderview.hpp"

BorderView::BorderView(QGraphicsRectItem* parent)
    : QGraphicsRectItem(parent)
    , m_borders(nullptr)
{}
void BorderView::setBorderSet(BorderSet* bs)
{
    m_borders = bs;
    // TODO
}
void BorderView::setInnerRect(const QRect& innerRect)
{
    m_rect = innerRect;
    // TODO
}
