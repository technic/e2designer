#ifndef WIDGETVIEW_H
#define WIDGETVIEW_H

#include <QFont>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>

#include "rectselector.hpp"
#include "repository/skinrepository.hpp"

class ScreenView;

/**
 * @brief The WidgetView class
 * glue between WidgetItem (data) and GraphicsItem (actual view)
 */
class WidgetView : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
public:
    WidgetView(ScreenView* view, QModelIndex index, WidgetView* parent);

    enum
    {
        Type = UserType + 1
    };
    int type() const override { return Type; }
    QPersistentModelIndex modelIndex() const { return mData; }

    // QGraphicsItem interface
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    //    ~WidgetView() override;

    // call me when attribute value changes in the model
    void updateAttribute(int key);

    // Whether to display widget borders
    void showBorder(bool show);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void setSelectorRect(const QRectF& globrect);

private:
    // refs
    ScreenView* mScreen;
    ScreensModel* mModel;
    QPersistentModelIndex mData;
    //    RectSelector *mSelector;

    // Observer
    WidgetObserverRegistrator mObserver;

    // Widget Type
    int m_type;

    //    // Common
    QColor m_foreground_color;
    QColor m_background_color;
    //    bool m_transparent;
    //    int m_border_width;
    //    QColor m_border_color;

    //    // Label
    //    QString m_text;
    //    int m_valign;
    //    int m_halign;
    //    QFont m_font;

    // Pixmap
    QPixmap m_pixmap;
    //    int m_alphatest;
    //    int m_scale;

    //    // Slider
    //    QPixmap m_slider_pixmap;
    //    QPixmap m_background_pixmap;
    //    int m_orientation;

    //    // Widget
    //    int m_render;

    // Preview
    //    int m_preview_render;
    //    QVariant m_preview;

    // Flag to reduce recursion:
    // don't call setData
    bool mRectChange;

    void initHandles();
    void setHandlesEnabled(bool enabled);
    void updateHandlesPos();

    void commitPositionChange(const QPoint& point);
    void commitSizeChange(const QSize& size);
    void commitRectChange(const QRect& rect);
    void paintBorder(QPainter* painter, const WidgetData& w);
    void paintScreen(QPainter* painter, const WidgetData& w);
    void paintLabel(QPainter* painter, const WidgetData& w);
    void paintPixmap(QPainter* painter, const WidgetData& w);
    void paintSlider(QPainter* painter, const WidgetData& w);

    QPixmap loadPixmap(const QString& fname);
};

#endif // WIDGETVIEW_H
