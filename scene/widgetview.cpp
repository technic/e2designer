#include "widgetview.hpp"
#include "attr/positionitem.hpp"
#include "repository/skinrepository.hpp"
#include "screenview.hpp"
#include <QCursor>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QTextDocument>

WidgetView::WidgetView(ScreenView* view, QModelIndex index, WidgetView* parent)
    : QGraphicsRectItem(parent)
    , mScreen(view)
    , mModel(SkinRepository::screens())
    , mData(index)
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
        setAttribute(i);
    }

    connect(SkinRepository::screens()->getWidget(index), &WidgetData::attrChanged, this,
            &WidgetView::setAttribute);

    setPen(QPen(QColor(Qt::yellow)));
}

void WidgetView::setSelectorRect(const QRectF& globrect)
{
    FlagTrue ft(&mRectChange);

    // qDebug() << __func__ << globrect;

    // Widget local coordinates origin is always in the topLeft corener
    setPos(mapToParent(mapFromScene(globrect.topLeft())));
    setRect(0., 0., globrect.width(), globrect.height());

    // Forbid moving during resize,
    // because when grabing resize handle with mouse
    // we may also trigger move of the item
    setFlag(ItemIsMovable, false);

    // update data in model
    commitSizeChange(rect().size().toSize());
    commitPositionChange(pos().toPoint());
}

void WidgetView::commitSizeChange(const QSize& size)
{
    FlagTrue ft(&mRectChange);
    mModel->setWidgetAttr(mData, Property::size, size, Roles::GraphicsRole);
}

void WidgetView::commitPositionChange(const QPoint& point)
{
    FlagTrue ft(&mRectChange);
    mModel->setWidgetAttr(mData, Property::position, point, Roles::GraphicsRole);
}

void WidgetView::setAttribute(int key)
{
    if (mRectChange)
        return;

    QVariant value = SkinRepository::screens()->getWidgetAttr(mData, key, Roles::GraphicsRole);

    switch (key) {
    case Property::position: {
        QRectF r = rect();
        setPos(value.toPoint() + r.topLeft().toPoint());
        r.moveTopLeft(QPointF(0, 0));
        setRect(r);

        if (isSelected()) {
            mScreen->selector()->setPos(pos());
            mScreen->selector()->setRect(rect());
        }
        break;
    }
    case Property::size: {
        QRectF r = rect();
        r.setSize(value.toSize());
        setRect(r);
        if (isSelected()) {
            mScreen->selector()->setSize(value.toSize());
        }
        break;
    }
    case Property::zPosition:
        setZValue(value.toInt());
        break;
    case Property::transparent:
        m_transparent = value.toInt();
        break;
    case Property::text:
        m_text = value.toString();
        break;
    case Property::font:
        m_font = qvariant_cast<QFont>(value);
        break;
    case Property::pixmap:
        m_pixmap = QPixmap(SkinRepository::instance().resolveFilename(value.toString()));
        break;
    case Property::alphatest:
        m_alphatest = value.toInt();
        break;
    case Property::scale:
        m_scale = value.toInt();
        break;
    case Property::backgroundColor:
        m_background_color = qvariant_cast<QColor>(value);
        break;
    case Property::foregroundColor:
        m_foreground_color = qvariant_cast<QColor>(value);
        break;
    case Property::valign:
        m_valign = value.toInt();
        break;
    case Property::halign:
        m_halign = value.toInt();
        break;
    case Property::borderColor:
        m_border_color = qvariant_cast<QColor>(value);
        break;
    case Property::borderWidth:
        m_border_width = value.toInt();
        break;
    case Property::orientation:
        m_orientation = value.toInt();
        break;
    case Property::render:
        m_render = value.toInt();
        break;
    case Property::previewRender:
        m_preview_render = value.toInt();
        break;
    case Property::previewValue:
        m_preview = value;
        break;
    default:
        return;
    }
    update();
}

void WidgetView::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    // no blending in the OSD layer
    painter->setCompositionMode(QPainter::CompositionMode_Source);

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
        render = m_render;
        if (render == Property::Widget)
            render = m_preview_render;
        break;
    default:
        render = Property::Widget;
        break;
    }

    switch (render) {
    case Property::Screen:
        paintScreen(painter);
        break;
    case Property::Label:
        paintLabel(painter);
        break;
    case Property::Pixmap:
        paintPixmap(painter);
        break;
    case Property::Slider:
        paintSlider(painter);
        break;
    default:
        qDebug() << "no render to paint";
        break;
    }
    paintBorder(painter);
    QGraphicsRectItem::paint(painter, option, widget);
}

void WidgetView::paintBorder(QPainter* painter)
{
    if (!(m_border_width > 0))
        return;
    const QRectF& r = rect();
    if (m_border_width > r.width() || m_border_width > r.height())
        return;

    qDebug() << "border" << m_border_width;

    QBrush brush(m_border_color);
    auto fillRect = [&painter, &brush](qreal x, qreal y, qreal w, qreal h) {
        painter->fillRect(QRectF(x, y, w, h), brush);
    };
    fillRect(r.x(), r.y(), r.width(), m_border_width);
    fillRect(r.x(), r.y() + m_border_width, m_border_width, r.height() - m_border_width);
    fillRect(r.x() + m_border_width, r.bottom() - m_border_width, r.width() - m_border_width,
             m_border_width);
    fillRect(r.right() - m_border_width, r.y() + m_border_width, m_border_width,
             r.height() - m_border_width);
}

void WidgetView::paintScreen(QPainter* painter)
{
    if (!m_transparent)
        painter->fillRect(rect(), QBrush(m_background_color));
    paintBorder(painter);
}

void WidgetView::paintLabel(QPainter* painter)
{
    if (!m_transparent) {
        //		painter->setCompositionMode(QPainter::CompositionMode_Difference);
        painter->fillRect(rect(), QBrush(m_background_color));
    }

    auto text = [this]() {
        if (m_text.isNull() || m_text.isEmpty()) {
            return m_preview.toString();
        } else {
            return m_text;
        }
    };
    painter->setPen(m_foreground_color);
    painter->setFont(m_font);
    painter->drawText(rect(), m_halign | m_valign | Qt::TextWordWrap, text());
    paintBorder(painter);
}

void WidgetView::paintPixmap(QPainter* painter)
{
    painter->save();

    if (m_alphatest == Property::Alphatest::blend || m_pixmap.isNull()) {
        painter->setCompositionMode(QPainter::CompositionMode_SourceOver);
    }

    if (m_scale) {
        painter->drawPixmap(rect(), m_pixmap, QRect(QPoint(0, 0), m_pixmap.size()));
    } else {
        painter->drawPixmap(rect(), m_pixmap, QRect(QPoint(0, 0), rect().size().toSize()));
    }
    painter->restore();
    paintBorder(painter);
}

void WidgetView::paintSlider(QPainter* painter)
{
    int percent = qBound(0, m_preview.toInt(), 100);
    QRect r = rect().toRect();
    if (m_orientation == Property::orHorizontal) {
        r.setWidth(r.width() * percent / 100);
    } else {
        r.setHeight(r.height() * percent / 100);
    }
    if (!m_transparent) {
        painter->fillRect(rect(), QBrush(m_background_color));
    }
    painter->fillRect(r, QBrush(m_foreground_color));
}

void WidgetView::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    //    if (event->button() == Qt::LeftButton) {
    //        emit m_view->selectionChanged(mData);
    //    }
    if (isSelected() || event->modifiers() & Qt::ControlModifier) {
        setFlag(ItemIsMovable, true);
    }
    return QGraphicsRectItem::mousePressEvent(event);
}

void WidgetView::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    return QGraphicsRectItem::mouseReleaseEvent(event);
}

void WidgetView::keyPressEvent(QKeyEvent* event)
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

QVariant WidgetView::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value)
{
    RectSelector* selector = mScreen->selector();

    switch (change) {
    case ItemPositionHasChanged:
        qDebug() << mRectChange << "pos" << value.toPointF() << rect();
        qDebug() << "sel" << selector->pos() << selector->rect();
        // move is not due to RectSelector action
        if (!mRectChange) {
            QPointF position = value.toPointF();
            selector->setPos(mapToScene(position));
            selector->setSceneRect(mapRectToScene(rect()));
            commitPositionChange(position.toPoint());
        }
        break;
    case ItemSelectedHasChanged:
        if (isSelected() && scene()) {
            // Init selector at widget position and track its resize signals
            connect(selector, &RectSelector::rectChnaged, this, &WidgetView::setSelectorRect);
            selector->setSceneRect(mapRectToScene(rect()));
            selector->setEnabled(true);
            selector->setVisible(true);
            QVariant v = mModel->getWidgetAttr(mData, Property::position, AnchorRole);
            AnchorPair anchors = v.value<AnchorPair>();
            selector->setXanchor(anchors.first);
            selector->setYanchor(anchors.second);
            setFlag(ItemIsFocusable, true);
            setFocus();
        } else {
            // Disable selector and disconnect from signals
            selector->setEnabled(false);
            selector->setVisible(false);
            setFlag(ItemIsFocusable, false);
            setFlag(ItemIsMovable, false);
            disconnect(mScreen->selector(), &RectSelector::rectChnaged, this,
                       &WidgetView::setSelectorRect);
        }
        break;

    case ItemSceneHasChanged:
        //        mSelector->setParentItem(parentItem());
        //        if (mSelector->parentItem() == nullptr) {
        //            if (scene())
        //                scene()->addItem(mSelector);
        //        }
        break;
    default:
        break;
    }
    return QGraphicsRectItem::itemChange(change, value);
}
