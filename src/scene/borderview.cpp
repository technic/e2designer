#include "borderview.hpp"

BorderView::BorderView(QGraphicsRectItem* parent)
    : QGraphicsRectItem(parent)
    , mBorders(nullptr)
{
}
void BorderView::setBorderSet(BorderSet* bs)
{
    mBorders = bs;
    // TODO
}
void BorderView::setInnerRect(const QRect& innerRect)
{
    mRect = innerRect;
    // TODO
}
