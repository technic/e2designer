#include "widgetview.hpp"
#include "repository/skinrepository.hpp"
#include "screenview.hpp"
#include "base/flagsetter.hpp"
#include "skin/widgetdata.hpp"
#include <QCursor>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QTextDocument>

WidgetGraphicsItem::WidgetGraphicsItem(ScreenView* sreen,
                                       QModelIndex index,
                                       WidgetGraphicsItem* parent)
    : ResizableGraphicsRectItem(parent)
    , mScreen(sreen)
    , mModel(sreen->model())
    , mData(index)
    , mObserver(mModel, index)
    , mRectChange(false)
{
    Q_ASSERT(mData.column() == ScreensModel::ColumnElement);

    qDebug() << "create WidgetView for" << mData.data(Qt::DisplayRole)
             << mData.sibling(mData.row(), ScreensModel::ColumnName).data(Qt::DisplayRole);

    // interact with user
    setFlags(ItemIsSelectable | ItemIsFocusable);
    // allow move after mouseRelease event
    setFlag(ItemIsMovable, false);
    setFlag(ItemSendsGeometryChanges, true);

    m_type = mData.data(ScreensModel::TypeRole).toInt();

    auto props = Property::propertyEnum();
    for (int i = 0; i < props.keyCount(); ++i) {
        updateAttribute(i);
    }
    showBorder(mScreen->haveBorders());
}

void WidgetGraphicsItem::resizeRectEvent(const QRectF& r)
{
    FlagSetter fs(&mRectChange);

    // update data in model
    QPoint oldPos = mapRectToParent(rect()).topLeft().toPoint();
    QPoint p = mapRectToParent(r).topLeft().toPoint();
    if (oldPos == p) {
        commitSizeChange(r.size().toSize());
    } else {
        commitRectChange(mapRectToParent(r).toRect());
    }
    // update QGraphicsRectItem
    ResizableGraphicsRectItem::resizeRectEvent(r);
}

void WidgetGraphicsItem::commitSizeChange(const QSize& size)
{
    FlagSetter fs(&mRectChange);
    mModel->resizeWidget(mData, size);
}

void WidgetGraphicsItem::commitRectChange(const QRect& rect)
{
    FlagSetter fs(&mRectChange);
    mModel->changeWidgetRect(mData, rect);
}

void WidgetGraphicsItem::commitPositionChange(const QPoint& point)
{
    FlagSetter fs(&mRectChange);
    mModel->moveWidget(mData, point);
}

void WidgetGraphicsItem::updateAttribute(int key)
{
    if (mRectChange)
        return;
    FlagSetter fs(&mRectChange);

    auto& w = mModel->widget(mData);

    // To be removed
    //    QVariant value = mModel->widget(mData).getAttr(key);

    switch (key) {
    case Property::position: {
        QRectF r = rect();
        setPos(w.absolutePosition() + r.topLeft().toPoint());
        r.moveTopLeft(QPointF(0, 0));
        setRect(r);
        updateHandlesPos();
        break;
    }
    case Property::size: {
        QRectF r = rect();
        r.setSize(w.selfSize());
        setRect(r);
        updateHandlesPos();
        break;
    }
    case Property::zPosition:
        setZValue(w.zPosition());
        break;
        //    case Property::transparent:
        //        m_transparent = value.toInt();
        //        break;
        //    case Property::text:
        //        m_text = value.toString();
        //        break;
        //    case Property::font:
        //        m_font = qvariant_cast<QFont>(value);
        //        break;
    case Property::pixmap:
        m_pixmap = QPixmap(SkinRepository::instance().resolveFilename(w.pixmap(key)));
        break;
        //    case Property::alphatest:
        //        m_alphatest = value.toInt();
        //        break;
        //    case Property::scale:
        //        m_scale = value.toInt();
        //        break;
    case Property::backgroundColor:
        m_background_color = w.getQColor(key);
        break;
    case Property::foregroundColor:
        m_foreground_color = w.getQColor(key);
        break;
        //    case Property::valign:
        //        m_valign = value.toInt();
        //        break;
        //    case Property::halign:
        //        m_halign = value.toInt();
        //        break;
        //    case Property::borderColor:
        //        m_border_color = qvariant_cast<QColor>(value);
        //        break;
        //    case Property::borderWidth:
        //        m_border_width = value.toInt();
        //        break;
        //    case Property::orientation:
        //        m_orientation = value.toInt();
        //        break;
        //    case Property::render:
        //        m_render = value.toInt();
        //        break;
        //    case Property::previewRender:
        //        m_preview_render = value.toInt();
        //        break;
        //    case Property::previewValue:
        //        m_preview = value;
        //        break;
        //    default:
        //        return;
    }
    update();
}

void WidgetGraphicsItem::showBorder(bool show)
{
    if (show) {
        setPen(QPen(Qt::yellow));
    } else {
        setPen(Qt::NoPen);
    }
}

void WidgetGraphicsItem::paint(QPainter* painter,
                               const QStyleOptionGraphicsItem* option,
                               QWidget* widget)
{
    // no blending in the OSD layer
    painter->setCompositionMode(QPainter::CompositionMode_Source);

    auto& w = mModel->widget(mData);

    int render;
    switch (m_type) {
    case WidgetData::Screen:
        render = Property::Screen;
        break;
    case WidgetData::Pixmap:
        render = Property::Pixmap;
        break;
    case WidgetData::Label:
        render = Property::Label;
        break;
    case WidgetData::Widget:
        render = w.render();
        if (render == Property::Widget)
            render = w.previewRender();
        break;
    default:
        render = Property::Widget;
        break;
    }

    switch (render) {
    case Property::Screen:
        paintScreen(painter, w);
        break;
    case Property::Label:
    case Property::FixedLabel:
        paintLabel(painter, w);
        break;
    case Property::Pixmap:
    case Property::Picon:
        paintPixmap(painter, w);
        break;
    case Property::Slider:
        paintSlider(painter, w);
        break;
    default:
        qDebug() << "no render to paint";
        break;
    }
    paintBorder(painter, w);
    QGraphicsRectItem::paint(painter, option, widget);
}

void WidgetGraphicsItem::paintBorder(QPainter* painter, const WidgetData& w)
{
    int bw = w.borderWidth();
    if (!(bw > 0))
        return;
    const QRectF& r = rect();
    if (bw > r.width() || bw > r.height())
        return;

    QBrush brush(w.getQColor(Property::borderColor));
    auto fillRect = [&painter, &brush](qreal x, qreal y, qreal w, qreal h) {
        painter->fillRect(QRectF(x, y, w, h), brush);
    };
    fillRect(r.x(), r.y(), r.width(), bw);
    fillRect(r.x(), r.y() + bw, bw, r.height() - bw);
    fillRect(r.x() + bw, r.bottom() - bw, r.width() - bw, bw);
    fillRect(r.right() - bw, r.y() + bw, bw, r.height() - bw);
}

void WidgetGraphicsItem::paintScreen(QPainter* painter, const WidgetData& w)
{
    if (!w.transparent())
        painter->fillRect(rect(), QBrush(m_background_color));
}

void WidgetGraphicsItem::paintLabel(QPainter* painter, const WidgetData& w)
{
    if (!w.transparent()) {
        //		painter->setCompositionMode(QPainter::CompositionMode_Difference);
        painter->fillRect(rect(), QBrush(m_background_color));
    }
    QString text = w.text();
    if (text.isNull()) {
        text = w.scenePreview().toString();
    }
    painter->setPen(m_foreground_color);
    painter->setFont(w.font().getFont());
    painter->drawText(rect(), w.halign() | w.valign() | Qt::TextWordWrap, text);
}

void WidgetGraphicsItem::paintPixmap(QPainter* painter, const WidgetData& w)
{
    painter->save();

    if (w.alphatest() == Property::Alphatest::blend || w.alphatest() == Property::Alphatest::on
        || m_pixmap.isNull()) {
        painter->setCompositionMode(QPainter::CompositionMode_SourceOver);
    }

    if (w.scale()) {
        painter->drawPixmap(rect(), m_pixmap, QRect(QPoint(0, 0), m_pixmap.size()));
    } else {
        painter->drawPixmap(rect(), m_pixmap, QRect(QPoint(0, 0), rect().size().toSize()));
    }
    painter->restore();
}

void WidgetGraphicsItem::paintSlider(QPainter* painter, const WidgetData& w)
{
    int percent = qBound(0, w.scenePreview().toInt(), 100);
    QRect r = rect().toRect();
    if (w.orientation() == Property::orHorizontal) {
        r.setWidth(r.width() * percent / 100);
    } else {
        r.setHeight(r.height() * percent / 100);
    }
    if (!w.transparent()) {
        painter->fillRect(rect(), QBrush(m_background_color));
    }
    painter->fillRect(r, QBrush(m_foreground_color));
}

void WidgetGraphicsItem::keyPressEvent(QKeyEvent* event)
{
    qDebug() << "WidgetView keyPressEvent";
    if (!isSelected()) {
        qWarning() << "press on non selected!";
    }
    QPoint mv(0, 0);
    int delta;
    if (event->modifiers() & Qt::ShiftModifier)
        delta = 1;
    else
        delta = 5;
    switch (event->key()) {
    case Qt::Key_Up:
        mv.setY(-delta);
        break;
    case Qt::Key_Down:
        mv.setY(delta);
        break;
    case Qt::Key_Right:
        mv.setX(delta);
        break;
    case Qt::Key_Left:
        mv.setX(-delta);
        break;
    case Qt::Key_Escape:
        setSelected(false);
        break;
    default:
        QGraphicsRectItem::keyPressEvent(event);
    }
    setPos(pos() + mv);
}

QVariant WidgetGraphicsItem::itemChange(QGraphicsItem::GraphicsItemChange change,
                                        const QVariant& value)
{
    switch (change) {
    case ItemPositionHasChanged:
        if (!mRectChange) {
            QPointF position = value.toPointF();
            commitPositionChange((position + rect().topLeft()).toPoint());
        }
        break;
    case ItemSelectedHasChanged:
        if (isSelected() && scene()) {
            setHandlesVisible(true);
            auto pos = mModel->widget(mData).position();
            setXanchor(pos.x().type());
            setYanchor(pos.y().type());
            setFlag(ItemIsFocusable, true);
            setFlag(ItemIsMovable, true);
            setFocus();
        } else {
            setHandlesVisible(false);
            setFlag(ItemIsFocusable, false);
            setFlag(ItemIsMovable, false);
        }
        break;
    default:
        break;
    }
    return QGraphicsRectItem::itemChange(change, value);
}
