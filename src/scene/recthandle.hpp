#ifndef RECTHANDLE_H
#define RECTHANDLE_H

#include <QGraphicsRectItem>

class ResizableGraphicsRectItem;

class RectHandle : public QGraphicsRectItem
{
public:
    static constexpr int handle_size = 8;

    enum HandlePosition
    {
        None = 0x0,
        Top = 0x1,
        Bottom = 0x2,
        Left = 0x4,
        TopLeft = Top | Left,
        BottomLeft = Bottom | Left,
        Right = 0x8,
        TopRight = Top | Right,
        BottomRight = Bottom | Right
    };

    explicit RectHandle(int position, ResizableGraphicsRectItem* parent);
    int flag() const { return m_place; }
    void updatePosition(const QRectF& r);

protected:
    // QGraphicsItem interface
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

private:
    int m_place;
    bool is_moving;
};

#endif // RECTHANDLE_H
