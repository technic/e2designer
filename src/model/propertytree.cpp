#include "propertytree.hpp"
#include "skin/enums.hpp"
#include <QApplication>
#include <QPalette>

PropertyTree::PropertyTree(const WidgetData* widget)
    : m_widget(widget)
    , m_root(new AttrItem())
{
    Q_CHECK_PTR(m_widget);

    using R = Property::Render;
    auto global = addGroup("General", R::Widget);
    add<TextItem>(Property::name, global);
    add<PositionItem>(Property::position, global);
    add<SizeItem>(Property::size, global);
    add<IntegerItem>(Property::zPosition, global);
    add<ColorItem>(Property::foregroundColor, global);
    add<ColorItem>(Property::backgroundColor, global);
    add<IntegerItem>(Property::transparent, global);
    add<ColorItem>(Property::borderColor, global);
    add<IntegerItem>(Property::borderWidth, global);

    auto text = addGroup("Label", R::Label);
    add<TextItem>(Property::text, text);
    add<FontItem>(Property::font, text);
    add<VAlignItem>(Property::valign, text);
    add<HAlignItem>(Property::halign, text);
    add<ColorItem>(Property::shadowColor, text);
    add<TextItem>(Property::shadowOffset, text);

    auto pixmap = addGroup("Pixmap", R::Pixmap);
    add<TextItem>(Property::pixmap, pixmap);
    add<AlphatestItem>(Property::alphatest, pixmap);
    add<IntegerItem>(Property::scale, pixmap);

    auto screen = addGroup("Screen", R::Screen);
    add<TextItem>(Property::title, screen);
    add<FlagsItem>(Property::flags, screen);

    auto listbox = addGroup("Listbox", R::Listbox);
    add<IntegerItem>(Property::itemHeight, listbox);
    add<PixmapItem>(Property::selectionPixmap, listbox);
    add<ScrollbarModeItem>(Property::scrollbarMode, listbox);
    add<IntegerItem>(Property::enableWrapAround, listbox);
    add<ColorItem>(Property::foregroundColorSelected, listbox);
    add<ColorItem>(Property::backgroundColorSelected, listbox);

    auto slider = addGroup("Slider", R::Slider);
    add<PixmapItem>(Property::sliderPixmap, slider);
    add<PixmapItem>(Property::backgroundPixmap, slider);
    add<OrientationItem>(Property::orientation, slider);

    auto gauge = addGroup("PositionGauge", R::Slider); // TODO
    add<PixmapItem>(Property::pointer, gauge);
    add<PixmapItem>(Property::seek_pointer, gauge);

    auto widgetItem = addGroup("Widget", R::Widget);
    add<RenderItem>(Property::render, widgetItem);
    add<TextItem>(Property::source, widgetItem);
    add<RenderItem>(Property::previewRender, widgetItem);
    add<VariantItem>(Property::previewValue, widgetItem);
}

AttrItem* PropertyTree::getItemPtr(const int key) const
{
    auto it = m_adapters.find(key);
    if (it != m_adapters.end()) {
        return *it;
    } else {
        return nullptr;
    }
}

PropertyTree::~PropertyTree()
{
    delete m_root;
}

const WidgetData& PropertyTree::widget()
{
    Q_ASSERT(m_widget);
    return *m_widget;
}

void PropertyTree::setWidget(const WidgetData* widget)
{
    m_widget = widget;
}

AttrGroupItem* PropertyTree::addGroup(const QString& title, Property::Render render)
{
    auto group = new AttrGroupItem(title, render);
    m_root->appendChild(group);
    group->setRoot(this);
    return group;
}

template<typename T>
void PropertyTree::add(int k, AttrItem* parent)
{
    auto* item = new T(k);
    item->setRoot(this);
    parent->appendChild(item);
    if (k != Property::invalid) {
        Q_ASSERT(!m_adapters.contains(k));
        m_adapters[k] = item;
    }
}

// AttrItem

AttrItem::AttrItem(int key)
    : MixinTreeNode<AttrItem>(crtp_guard())
    , m_key(key)
    , m_tree(nullptr)
{}

void AttrItem::setRoot(PropertyTree* tree)
{
    m_tree = tree;
}

QVariant AttrItem::keyData(int role) const
{
    switch (role) {
    case Qt::DisplayRole:
        return Property::propertyEnum().valueToKey(m_key);
    default:
        return QVariant();
    }
}

QVariant AttrItem::data(int role) const
{
    Q_UNUSED(role);
    return QVariant();
}
QVariant AttrItem::convert(const QVariant& value, int role)
{
    Q_UNUSED(role);
    Q_UNUSED(value);
    return QVariant();
}

// AttrGroupItem

QVariant AttrGroupItem::keyData(int role) const
{
    switch (role) {
    case Qt::DisplayRole:
        return m_title;
    case Qt::BackgroundRole:
        return QBrush(QApplication::palette().color(QPalette::Button));
    case ShallExpandRole:
        return shallExpand();
    default:
        return AttrItem::keyData(role);
    }
}

QVariant AttrGroupItem::data(int role) const
{
    switch (role) {
    case Qt::BackgroundRole:
        return QBrush(QApplication::palette().color(QPalette::Button));
    default:
        return AttrItem::data(role);
    }
}

bool AttrGroupItem::shallExpand() const
{
    auto r = widget().sceneRender();
    return r == m_render || m_render == Property::Render::Widget;
}

// ColorItem

QVariant ColorItem::data(int role) const
{
    switch (role) {
    case Qt::DecorationRole:
    case Qt::EditRole:
        return widget().getQColor(key());
    case Qt::DisplayRole:
        return widget().color(key()).toString();
    default:
        return AttrItem::data(role);
    }
}

QVariant ColorItem::convert(const QVariant& value, int role)
{
    switch (role) {
    case Qt::EditRole:
        return QVariant::fromValue(ColorAttr(qvariant_cast<QString>(value)));
    default:
        return AttrItem::convert(value, role);
    }
}

// EnumItem

template<class Enum>
QVariant EnumItem<Enum>::data(int role) const
{
    auto attr = [this]() -> EnumAttr<Enum> {
        Enum x = widget().getAttr(key()).template value<Enum>();
        return EnumAttr<Enum>(x);
    };
    switch (role) {
    case Qt::DisplayRole:
        return attr().toStr(false);
    case Qt::EditRole: {
        AttrType a = attr();
        return QVariant::fromValue(SkinEnumList(a.toStr(false), a.toStrList()));
    }
    default:
        return AttrItem::data(role);
    }
}

template<class Enum>
QVariant EnumItem<Enum>::convert(const QVariant& value, int role)
{
    switch (role) {
    case Qt::EditRole:
        return QVariant::fromValue(AttrType(qvariant_cast<SkinEnumList>(value).first).value());
    default:
        return AttrItem::convert(value, role);
    }
}

// FontItem

QVariant FontItem::data(int role) const
{
    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
        return widget().font().toStr();
    default:
        return AttrItem::data(role);
    }
}

QVariant FontItem::convert(const QVariant& value, int role)
{
    switch (role) {
    case Qt::EditRole:
        return QVariant::fromValue(FontAttr(value.toString()));
    default:
        return AttrItem::convert(value, role);
    }
}

// IntegerItem

QVariant IntegerItem::data(int role) const
{
    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
        return widget().getAttr(key()).toInt();
    default:
        return AttrItem::data(role);
    }
}

QVariant IntegerItem::convert(const QVariant& value, int role)
{
    switch (role) {
    case Qt::EditRole:
        return value.toInt();
    default:
        return AttrItem::convert(value, role);
    }
}

// PixmapItem

PixmapItem::PixmapItem(int key)
    : AttrItem(key)
{}

QVariant PixmapItem::data(int role) const
{
    // TODO: implement!
    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
        return "";
    default:
        return AttrItem::data(role);
    }
}

QVariant PixmapItem::convert(const QVariant& value, int role)
{
    // TODO: implement!
    switch (role) {
    case Qt::EditRole:
        return false;
    default:
        return AttrItem::convert(value, role);
    }
}

// PositionItem

PositionItem::PositionItem(int key)
    : AttrItem(key)
{
    // TODO: test it
    //    appendChild(new AbsolutePositionItem(pWidget));
    //    appendChild(new CoordinateItem(pWidget, Coordinate::X));
    //    appendChild(new CoordinateItem(pWidget, Coordinate::Y));
}

QVariant PositionItem::data(int role) const
{
    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
        return widget().position().toStr();
    default:
        return AttrItem::data(role);
    }
}

QVariant PositionItem::convert(const QVariant& value, int role)
{
    switch (role) {
    case Qt::EditRole:
        return QVariant::fromValue(PositionAttr(value.toString()));
    default:
        return AttrItem::convert(value, role);
    }
}

// PositionItem::CoordinateItem

QVariant PositionItem::CoordinateItem::data(int role) const
{
    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole: {
        switch (axis) {
        case AxisName::X:
            return widget().position().x().value();
        case AxisName::Y:
            return widget().position().y().value();
        }
    }
    default:
        return AttrItem::data(role);
    }
}

QVariant PositionItem::CoordinateItem::convert(const QVariant& value, int role)
{
    switch (role) {
    case Qt::EditRole: {
        auto coord = Coordinate(value.toString());
        const PositionAttr& pos = widget().position();
        PositionAttr newPos;
        switch (axis) {
        case AxisName::X:
            newPos = PositionAttr(coord, pos.y());
            break;
        case AxisName::Y:
            newPos = PositionAttr(pos.x(), coord);
            break;
        }
        return QVariant::fromValue(newPos);
    }
    default:
        return AttrItem::convert(value, role);
    }
}

// AbsolutePositionItem

QVariant AbsolutePositionItem::data(int role) const
{
    switch (role) {
    case Qt::DisplayRole: {
        QPoint pos = widget().absolutePosition();
        return QString("[X=%1 Y=%2]").arg(pos.x(), pos.y());
    }
    default:
        return AttrItem::data(role);
    }
}

// SizeItem

QVariant SizeItem::data(int role) const
{
    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
        return widget().size().toStr();
    default:
        return AttrItem::data(role);
    }
}

QVariant SizeItem::convert(const QVariant& value, int role)
{
    switch (role) {
    case Qt::EditRole:
        return QVariant::fromValue(SizeAttr(value.toString()));
    default:
        return AttrItem::convert(value, role);
    }
}

// TextItem

QVariant TextItem::data(int role) const
{
    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
        return widget().getAttr(key());
    default:
        return AttrItem::data(role);
    }
}

QVariant TextItem::convert(const QVariant& value, int role)
{
    switch (role) {
    case Qt::EditRole: {
        auto str = value.toString();
        if (str.isEmpty())
            str.clear();
        return str;
    }
    default:
        return AttrItem::convert(value, role);
    }
}

// VariantItem

QVariant VariantItem::data(int role) const
{
    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
        return widget().getAttr(key()).toString();
    default:
        return AttrItem::data(role);
    }
}

QVariant VariantItem::convert(const QVariant& value, int role)
{
    switch (role) {
    case Qt::EditRole:
        return value;
    default:
        return AttrItem::convert(value, role);
    }
}
