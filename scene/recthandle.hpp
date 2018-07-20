#ifndef RECTHANDLE_H
#define RECTHANDLE_H

#include <QGraphicsRectItem>

class RectSelector;

class RectHandle : public QGraphicsRectItem
{
public:
    static constexpr int handle_size = 8;

    enum {
        Top = 0x1,
        Bottom = 0x2,
        Left = 0x4,
        TopLeft = Top | Left,
        BottomLeft = Bottom | Left,
        Right = 0x8,
        TopRight = Top | Right,
        BottomRight = Bottom | Right
    };

    explicit RectHandle(int position, RectSelector* parent);
    int flag() const { return m_place; }
    void updatePosition();

protected:
    /* QGraphicsItem */
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

private:
    QPointF restrictPosition(QPointF newPos);

    RectSelector* m_parent;
    int m_place;
};

#endif // RECTHANDLE_H
