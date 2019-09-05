#pragma once

#include <QGraphicsPixmapItem>

class BackgroundPixmap : public QGraphicsPixmapItem
{
public:
    explicit BackgroundPixmap(const QPixmap& pixmap, QGraphicsItem* parent = Q_NULLPTR)
        : QGraphicsPixmapItem(pixmap, parent)
    {}

    // QGraphicsItem interface
public:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) final;
};

class BackgroundRect : public QGraphicsRectItem
{
public:
    explicit BackgroundRect(const QRectF rect, QGraphicsItem* parent = Q_NULLPTR)
        : QGraphicsRectItem(rect, parent)
    {}

    // QGraphicsItem interface
public:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) final;
};

