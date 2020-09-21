#pragma once

#include <QFont>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>

#include "rectselector.hpp"
#include "repository/skinrepository.hpp"
#include "repository/pixmapstorage.hpp"
#include "base/typelist.hpp"
#include "scene/borderview.hpp"

class ScreenView;
class WidgetGraphicsItem;

using GraphicsItemClasses = TypeList<WidgetGraphicsItem>;

template<typename T>
static constexpr inline int getGraphicsItemType()
{
    using type = std::remove_cv_t<std::remove_pointer_t<T>>;
    // It's OK to cast here, because index can not be very large
    auto index = static_cast<int>(GraphicsItemClasses::getIndex<type>());
    return QGraphicsItem::UserType + index + 1;
}

/**
 * @brief The WidgetGraphicsItem class
 * glue between WidgetItem (data) and GraphicsItem (actual view)
 */
class WidgetGraphicsItem : public QObject, public ResizableGraphicsRectItem, public PixmapWatcher
{
    Q_OBJECT
public:
    WidgetGraphicsItem(ScreenView* screen, QModelIndex index, WidgetGraphicsItem* parent);
    enum
    {
        Type = getGraphicsItemType<WidgetGraphicsItem>()
    };
    int type() const override { return Type; }
    QPersistentModelIndex modelIndex() const { return m_data; }

    // QGraphicsItem interface
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    //    ~WidgetView() override;

    // call me when attribute value changes in the model
    void updateAttribute(int key);

    // Whether to display widget borders
    void showBorder(bool show);

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
    void keyPressEvent(QKeyEvent* event) override;

    void resizeRectEvent(const QRectF& rect) override;

    // PixmapWatcher interface
    void fileChangedEvent() override;

private:
    // refs
    ScreenView* m_screen;
    ScreensModel* m_model;
    QPersistentModelIndex m_data;

    // Observer
    WidgetObserverRegistrator m_observer;

    // Borders graphicsItem
    BorderView* m_border;

    //    // Common
    QColor m_foreground_color;
    QColor m_background_color;
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
    bool m_rectChange;

    void commitPositionChange(const QPoint& point);
    void commitSizeChange(const QSize& size);
    void commitRectChange(const QRect& rect);
    void paintBorder(QPainter* painter, const WidgetData& w);
    void paintScreen(QPainter* painter, const WidgetData& w);
    void paintLabel(QPainter* painter, const WidgetData& w);
    void paintPixmap(QPainter* painter, const WidgetData& w);
    void paintSlider(QPainter* painter, const WidgetData& w);
    void paintListbox(QPainter* painter, const WidgetData& w);

    QPixmap loadPixmap(const QString& fname);
    void updateBorderRect();
};
