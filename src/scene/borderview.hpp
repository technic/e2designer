#pragma once

#include "model/bordersmodel.hpp"
#include <QGraphicsRectItem>

class BorderView : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
public:
    BorderView(QGraphicsRectItem* parent);
    void setBorderSet(BorderSet* bs);
    void setInnerRect(const QRect& innerRect);

private:
    BorderSet* m_borders;
    QRect m_rect;
};
