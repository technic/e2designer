#ifndef BACKGROUNDPIXMAP_H
#define BACKGROUNDPIXMAP_H

#include <QGraphicsPixmapItem>

class BackgroundPixmap : public QGraphicsPixmapItem
{
public:
    explicit BackgroundPixmap(const QPixmap& pixmap, QGraphicsItem* parent = Q_NULLPTR)
        : QGraphicsPixmapItem(pixmap, parent)
    {
    }

    // QGraphicsItem interface
public:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
};

#endif // BACKGROUNDPIXMAP_H
