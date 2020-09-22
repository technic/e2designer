#include "widgetdata.hpp"
#include <QDebug>
#include <QTimer>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QBuffer>

#include "model/colorsmodel.hpp"
#include "model/windowstyle.hpp"
#include "model/fontsmodel.hpp"
#include "model/screensmodel.hpp"
#include "repository/skinrepository.hpp"
#include "model/propertytree.hpp"
#include "skin/enumattr.hpp"
#include "skin/attributes.hpp"

#include <iostream>
#include <memory>
#include <type_traits>
#include <typeinfo>

using Widget = WidgetData;

class AbstractReflection
{
public:
    virtual QVariant get(const Widget& w) = 0;
    virtual void set(Widget& w, const QVariant& v) = 0;
    virtual int type() const = 0;
    virtual QString getStr(const Widget& w) = 0;
    virtual void setFromStr(Widget& w, const QString& str) = 0;
    virtual ~AbstractReflection() = default;
};

template<typename V>
using Getter = V (Widget::*)() const;
template<typename V>
using Setter = void (Widget::*)(const V&);
template<typename V>
using SetterValue = void (Widget::*)(V);

template<typename V>
using GetterKey = V (Widget::*)(int) const;
template<typename V>
using SetterKey = void (Widget::*)(int, const V&);
template<typename V>
using SetterKeyValue = void (Widget::*)(int, V);

template<typename T, typename Getter, typename Setter>
class Reflection : public AbstractReflection
{
public:
    Reflection(Getter get_method, Setter set_method)
        : m_getter(get_method)
        , m_setter(set_method)
    {}

    QVariant get(const Widget& w) final
    {
        const T& t = (w.*m_getter)();
        return QVariant::fromValue(t);
    }
    void set(Widget& w, const QVariant& v) final
    {
        const T& t = v.value<T>();
        (w.*m_setter)(t);
    }
    QString getStr(const Widget& w) final
    {
        const T& t = (w.*m_getter)();
        return serialize(t);
    }
    void setFromStr(Widget& w, const QString& str) final
    {
        T t;
        deserialize(str, t);
        (w.*m_setter)(t);
    }
    int type() const final { return qMetaTypeId<T>(); }

private:
    Getter m_getter;
    Setter m_setter;
};

template<typename T, typename Getter, typename Setter>
Reflection<T, Getter, Setter>* makeReflection(Getter g, Setter s)
{
    return new Reflection<T, Getter, Setter>(g, s);
}

template<typename T, typename GetterKey, typename SetterKey>
class ReflectionKey : public AbstractReflection
{
public:
    ReflectionKey(int key, GetterKey get_method, SetterKey set_method)
        : m_key(key)
        , m_getter(get_method)
        , m_setter(set_method)
    {}

    QVariant get(const Widget& w) final
    {
        const T& t = (w.*m_getter)(m_key);
        return QVariant::fromValue(t);
    }
    void set(Widget& w, const QVariant& v) final
    {
        const T& t = v.value<T>();
        (w.*m_setter)(m_key, t);
    }
    QString getStr(const Widget& w) final
    {
        const T& t = (w.*m_getter)(m_key);
        return serialize(t);
    }
    void setFromStr(Widget& w, const QString& str) final
    {
        T t;
        deserialize(str, t);
        (w.*m_setter)(m_key, t);
    }
    int type() const final { return qMetaTypeId<T>(); }

private:
    int m_key;
    GetterKey m_getter;
    SetterKey m_setter;
};

template<typename T, typename GetterKey, typename SetterKey>
ReflectionKey<T, GetterKey, SetterKey>* makeReflectionKey(int k, GetterKey g, SetterKey s)
{
    return new ReflectionKey<T, GetterKey, SetterKey>(k, g, s);
}

class WidgetReflection
{
    Q_DISABLE_COPY(WidgetReflection)
public:
    WidgetReflection();

    template<typename T>
    void add(int k, Getter<T> g, Setter<T> s)
    {
        Q_ASSERT(m_funcs.find(k) == m_funcs.end());
        m_funcs[k] = makeReflection<T>(g, s);
    }
    template<typename T>
    void add(int k, Getter<T> g, SetterValue<T> s)
    {
        Q_ASSERT(m_funcs.find(k) == m_funcs.end());
        m_funcs[k] = makeReflection<T>(g, s);
    }
    template<typename T>
    void add(int k, GetterKey<T> g, SetterKey<T> s)
    {
        Q_ASSERT(m_funcs.find(k) == m_funcs.end());
        m_funcs[k] = makeReflectionKey<T>(k, g, s);
    }
    template<typename T>
    void add(int k, GetterKey<T> g, SetterKeyValue<T> s)
    {
        Q_ASSERT(m_funcs.find(k) == m_funcs.end());
        m_funcs[k] = makeReflectionKey<T>(k, g, s);
    }
    ~WidgetReflection() { qDeleteAll(m_funcs); }
    using Hash = QHash<int, AbstractReflection*>;
    // Allow iteration
    inline Hash::const_iterator cbegin() const { return m_funcs.cbegin(); }
    inline Hash::const_iterator cend() const { return m_funcs.cend(); }
    inline Hash::const_iterator find(int key) const { return m_funcs.find(key); }
    // test:
    bool hasAllKeys();

private:
    QHash<int, AbstractReflection*> m_funcs;
};

WidgetReflection::WidgetReflection()
{
    using p = Property;
    using w = WidgetData;

    add(p::name, &w::name, &w::setName);
    add(p::position, &w::position, &w::setPosition);
    add(p::size, &w::size, &w::setSize);
    add(p::zPosition, &w::zPosition, &w::setZPosition);
    add(p::transparent, &w::transparent, &w::setTransparent);
    add(p::borderColor, &w::color, &w::setColor);
    add(p::borderWidth, &w::borderWidth, &w::setBorderWidth);
    add(p::pixmap, &w::pixmap, &w::setPixmap);
    add(p::alphatest, &w::alphatest, &w::setAlphatest);
    add(p::scale, &w::scale, &w::setScale);
    add(p::text, &w::text, &w::setText);
    add(p::font, &w::font, &w::setFont);
    add(p::valign, &w::valign, &w::setValign);
    add(p::halign, &w::halign, &w::setHalign);
    add(p::shadowColor, &w::color, &w::setColor);
    add(p::shadowOffset, &w::shadowOffset, &w::setShadowOffset);
    add(p::noWrap, &w::noWrap, &w::setNoWrap);
    add(p::title, &w::title, &w::setTitle);
    add(p::flags, &w::flags, &w::setFlags);
    add(p::itemHeight, &w::itemHeight, &w::setItemHeight);
    add(p::selectionPixmap, &w::pixmap, &w::setPixmap);
    add(p::selectionDisabled, &w::hasFlag, &w::setFlag);
    add(p::scrollbarMode, &w::scrollbarMode, &w::setScrollbarMode);
    add(p::enableWrapAround, &w::hasFlag, &w::setFlag);
    add(p::sliderPixmap, &w::pixmap, &w::setPixmap);
    add(p::backgroundPixmap, &w::pixmap, &w::setPixmap);
    add(p::orientation, &w::orientation, &w::setOrientation);
    add(p::backgroundColor, &w::color, &w::setColor);
    add(p::backgroundColorSelected, &w::color, &w::setColor);
    add(p::foregroundColor, &w::color, &w::setColor);
    add(p::foregroundColorSelected, &w::color, &w::setColor);
    add(p::pointer, &w::pixmap, &w::setPixmap);
    add(p::seek_pointer, &w::pixmap, &w::setPixmap);
    add(p::render, &w::render, &w::setRender);
    add(p::source, &w::source, &w::setSource);
    add(p::previewRender, &w::previewRender, &w::setPreviewRender);
    add(p::previewValue, &w::previewValue, &w::setPreviewValue);

    // Call it here because of Qt limitation with static objects
    Q_ASSERT(hasAllKeys());
}

bool WidgetReflection::hasAllKeys()
{
    auto meta = QMetaEnum::fromType<Property::PropertyEnum>();
    for (int i = 0; i < meta.keyCount(); ++i) {
        int key = meta.value(i);
        // Special values
        if (key == Property::invalid || key == Property::preview)
            continue;
        if (m_funcs.find(key) == m_funcs.end()) {
            qWarning() << "Missing key:" << meta.key(i);
            return false;
        }
    }
    return true;
}

static WidgetReflection reflection;

// WidgetData

WidgetData::WidgetAttributes::WidgetAttributes()
    : zValue(0)
    , transparent(false)
    , borderWidth(0)
    , alphatest(Property::Alphatest::off)
    , scale(0)
    , valign(PropertyVAlign::top)
    , halign(PropertyHAlign::left)
    , noWrap(false)
    , flags(Property::Flags::wfBorder)
    , itemHeight(0)
    , scrollbarMode(Property::ScrollbarMode::showNever)
    , orientation(Property::Orientation::orHorizontal)
    , render(Property::Render::Widget)
    , previewRender(Property::Render::Widget)
{}

WidgetData::WidgetData(WidgetType type)
    : MixinTreeNode<WidgetData>(crtp_guard())
    , m_type(type)
    , m_model(nullptr)
{}

WidgetData* WidgetData::createFromXml(QXmlStreamReader& xml)
{
    Q_ASSERT(xml.isStartElement());

    bool ok;
    auto type = strToType(xml.name(), ok);
    if (!ok) {
        xml.raiseError(QString("Unknown widget tag '%1'").arg(xml.name().toString()));
        return nullptr;
    }
    auto widget = new WidgetData(type);
    ok = widget->fromXml(xml);
    if (ok) {
        return widget;
    }
    delete widget;
    return nullptr;
}

bool WidgetData::insertChild(int position, WidgetData* child)
{
    bool inserted = Base::insertChild(position, child);
    if (inserted) {
        child->setModel(m_model);
    }
    return inserted;
}

bool WidgetData::insertChildren(int position, QVector<WidgetData*> list)
{
    bool inserted = Base::insertChildren(position, list);
    if (inserted) {
        for (int i = position; i < position + list.size(); ++i) {
            child(i)->setModel(m_model);
        }
    }
    return inserted;
}

QVector<WidgetData*> WidgetData::takeChildren(int position, int count)
{
    auto list = Base::takeChildren(position, count);
    for (auto* w : qAsConst(list)) {
        w->setModel(nullptr);
    }
    return list;
}

void WidgetData::setModel(ScreensModel* model)
{
    m_model = model;
    for (int i = 0; i < childCount(); ++i) {
        child(i)->setModel(m_model);
    }
}

QString WidgetData::typeStr() const
{
    switch (m_type) {
    case WidgetType::Screen:
        return "screen";
    case WidgetType::Widget:
        return "widget";
    case WidgetType::Label:
        return "eLabel";
    case WidgetType::Pixmap:
        return "ePixmap";
    case WidgetType::Applet:
        return "applet";
    case WidgetType::Panel:
        return "panel";
    case WidgetType::Include:
        return "include";
    }
}

WidgetData::WidgetType WidgetData::strToType(const QStringRef& str, bool& ok)
{
    ok = true;
    if (str == "screen") {
        return WidgetType::Screen;
    } else if (str == "widget") {
        return WidgetType::Widget;
    } else if (str == "eLabel") {
        return WidgetType::Label;
    } else if (str == "ePixmap") {
        return WidgetType::Pixmap;
    } else if (str == "applet") {
        return WidgetType::Applet;
    } else if (str == "panel") {
        return WidgetType::Panel;
    } else {
        ok = false;
        return WidgetType::Widget;
    }
}

void WidgetData::resize(const QSizeF& size)
{
    m_attrs.size.setSize(*this, size.toSize());
    sizeChanged();
}

void WidgetData::setSize(const SizeAttr& size)
{
    m_attrs.size = size;
    sizeChanged();
}

void WidgetData::move(const QPointF& pos)
{
    m_attrs.position.setPoint(*this, pos.toPoint());
    notifyAttrChange(Property::position);
}

void WidgetData::setPosition(const PositionAttr& pos)
{
    m_attrs.position = pos;
    notifyAttrChange(Property::position);
}

QPoint WidgetData::absolutePosition() const
{
    return m_attrs.position.toPoint(*this);
}

QSize WidgetData::selfSize() const
{
    return m_attrs.size.getSize(*this);
}

QSize WidgetData::parentSize() const
{
    MixinTreeNode<WidgetData>* p = parent();
    if (p && p->isChild()) {
        return p->self()->selfSize();
    } else {
        // TODO: should depend on output id
        return SkinRepository::instance().outputSize();
    }
}

void WidgetData::setZPosition(int z)
{
    m_attrs.zValue = z;
    notifyAttrChange(Property::zPosition);
}

void WidgetData::setTransparent(bool val)
{
    m_attrs.transparent = val;
    notifyAttrChange(Property::transparent);
}

ColorAttr WidgetData::color(int key) const
{
    switch (key) {
    case Property::foregroundColor:
    case Property::backgroundColor:
    default:
        return m_attrs.colors[key];
    }
}

void WidgetData::setColor(int key, const ColorAttr& color)
{
    m_attrs.colors[key] = color;
    if (m_model) {
        m_attrs.colors[key].reload(m_model->colors());
        notifyAttrChange(key);
    }
}

QColor WidgetData::getQColor(int key) const
{
    if (m_attrs.colors[key].isDefined()) {
        return m_attrs.colors[key].getQColor();
    } else if (m_model) {
        switch (key) {
        case Property::foregroundColor:
            return m_model->roles().getQColor(WindowStyleColor::ColorRole::LabelForeground);
        case Property::backgroundColor:
            return m_model->roles().getQColor(WindowStyleColor::ColorRole::Background);
        default:
            return QColor();
        }
    } else {
        return QColor();
    }
}

void WidgetData::setFont(const FontAttr& font)
{
    m_attrs.font = font;
    notifyAttrChange(Property::font);
}

void WidgetData::setBorderWidth(int px)
{
    m_attrs.borderWidth = px;
    notifyAttrChange(Property::borderWidth);
}

void WidgetData::setText(const QString& text)
{
    m_attrs.text = text;
    notifyAttrChange(Property::text);
}

void WidgetData::setHalign(PropertyHAlign::Enum align)
{
    m_attrs.halign = align;
    notifyAttrChange(Property::halign);
}

void WidgetData::setValign(PropertyVAlign::Enum align)
{
    m_attrs.valign = align;
    notifyAttrChange(Property::valign);
}

void WidgetData::setShadowOffset(const OffsetAttr& offset)
{
    m_attrs.shadowOffset = offset;
    notifyAttrChange(Property::shadowOffset);
}

void WidgetData::setNoWrap(bool value)
{
    m_attrs.noWrap = value;
    notifyAttrChange(Property::noWrap);
}

void WidgetData::setItemHeight(int px)
{
    m_attrs.itemHeight = px;
    notifyAttrChange(Property::itemHeight);
}

void WidgetData::setScrollbarMode(Property::ScrollbarMode mode)
{
    m_attrs.scrollbarMode = mode;
    notifyAttrChange(Property::scrollbarMode);
}

PixmapAttr WidgetData::pixmap(int key) const
{
    return m_attrs.pixmaps[key];
}

void WidgetData::setPixmap(int key, const PixmapAttr& p)
{
    m_attrs.pixmaps[key] = p;
    notifyAttrChange(key);
}

bool WidgetData::hasFlag(int key) const
{
    return m_attrs.switches[key];
}

void WidgetData::setFlag(int key, bool value)
{
    m_attrs.switches[key] = value;
    notifyAttrChange(key);
}

void WidgetData::setAlphatest(Property::Alphatest value)
{
    m_attrs.alphatest = value;
    notifyAttrChange(Property::alphatest);
}

void WidgetData::setScale(int scale)
{
    m_attrs.scale = scale;
    notifyAttrChange(Property::scale);
}

void WidgetData::setOrientation(Property::Orientation orientation)
{
    m_attrs.orientation = orientation;
    notifyAttrChange(Property::orientation);
}

void WidgetData::setName(const QString& name)
{
    m_attrs.name = name;
    notifyAttrChange(Property::name);
}

void WidgetData::setRender(Property::Render render)
{
    m_attrs.render = render;
    notifyAttrChange(Property::render);
}

void WidgetData::setSource(const QString& source)
{
    m_attrs.source = source;
    notifyAttrChange(Property::source);
}

void WidgetData::setPreviewRender(Property::Render render)
{
    m_attrs.previewRender = render;
    notifyAttrChange(Property::previewRender);
}

void WidgetData::setPreviewValue(const QVariant& value)
{
    m_attrs.previewValue = value;
    notifyAttrChange(Property::previewValue);
}

QVariant WidgetData::scenePreview() const
{
    Source* source = nullptr;
    PreviewSource previewSource{ previewValue() };

    if (!this->source().isNull()) {
        source = MockSourceFactory::instance().getReference(this->source());
    }
    if (!source) {
        source = &previewSource;
    }

    for (size_t i = 0; i < m_converters.size(); ++i) {
        if (i < 1) {
            m_converters[i]->attach(source);
        } else {
            m_converters[i]->attach(m_converters[i - 1].get());
        }
    }

    Source* finalSource = source;
    if (!m_converters.empty()) {
        finalSource = m_converters.back().get();
    }
    using R = Property::Render;
    switch (m_attrs.render) {
    case R::Label:
    case R::FixedLabel:
        return finalSource->getText();
    case R::Slider:
        return finalSource->getValue();
    case R::Listbox:
        return finalSource->getVariant(QString());
    default:
        return QVariant();
    }
}

void WidgetData::setTitle(const QString& text)
{
    m_attrs.title = text;
    notifyAttrChange(Property::title);
}

void WidgetData::setFlags(Property::Flags flags)
{
    m_attrs.flags = flags;
    notifyAttrChange(Property::flags);
}

/*!
 * \brief Parses widget data from xml stream
 * The \a xml reader should be at the StartElement token.
 * If widget can not be parsed the error is reported to \a xml reader
 * \param xml reader
 * \return parse status
 */
bool WidgetData::fromXml(QXmlStreamReader& xml)
{
    Q_ASSERT(xml.isStartElement());

    bool ok;
    auto type = strToType(xml.name(), ok);
    Q_ASSERT(ok && type == m_type);

    parseAttributes(xml.attributes());

    // FIXME: replace this if spaghetti with proper OOP solution
    if (m_type == WidgetType::Applet) {
        m_attrs.appletCode = xml.readElementText();
        return !xml.hasError();
    }

    while (xml.readNextStartElement()) {
        if (m_type == WidgetType::Screen) {
            auto widget = WidgetData::createFromXml(xml);
            if (widget) {
                appendChild(widget);
            } else {
                return false;
            }
        } else if (xml.name() == "convert") {
            appendConverterFromXml(xml);
        } else {
            qWarning() << "unknown element" << xml.name();
            xml.skipCurrentElement();
        }
    }

    return !xml.hasError();
}

void WidgetData::appendConverterFromXml(QXmlStreamReader& xml)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == "convert");
    auto name = xml.attributes().value("type");
    std::unique_ptr<Converter> converter;
    if (!name.isNull()) {
        converter = ConverterFactory::instance().createConverterByName(name.toString());
    }
    if (!converter) {
        // Fallback to default implementation
        converter = std::make_unique<Converter>();
    }
    converter->fromXml(xml);
    m_converters.push_back(std::move(converter));
}

void WidgetData::parseAttributes(QXmlStreamAttributes attrs)
{
    QMetaEnum meta = Property::propertyEnum();

    for (const auto& attr : attrs) {
        bool ok;
        m_propertiesOrder.append(attr.name().toString());
        int key = meta.keyToValue(attr.name().toLatin1().data(), &ok);
        if (ok) {
            setAttrFromXml(key, attr.value().toString());
        } else {
            qWarning() << "unknown attribute" << attr.name();
            m_attrs.otherAttributes[attr.name().toString()] = attr.value().toString();
        }
    }
}

/**
 * @brief Load preview value and render from model
 * according to widget and parent screen names.
 * If called on a screen apply operation to all child widgets
 */
void WidgetData::loadPreview()
{
    if (!m_model) {
        return;
    }
    if (m_type == WidgetType::Widget) {
        MixinTreeNode<WidgetData>* ptr = parent();
        if (ptr) {
            QString screen = ptr->self()->name();
            Preview p = m_model->getPreview(screen, previewName());
            m_attrs.previewRender = p.render;
            m_attrs.previewValue = p.value;
        }
    } else if (m_type == WidgetType::Screen) {
        for (int i = 0; i < childCount(); ++i) {
            child(i)->loadPreview();
        }
    }
}

QString WidgetData::previewName() const
{
    if (name().isEmpty()) {
        return source();
    }
    return name();
}

Property::Render WidgetData::sceneRender() const
{
    switch (type()) {
    case WidgetType::Screen:
        return Property::Screen;
    case WidgetType::Pixmap:
        return Property::Pixmap;
    case WidgetType::Label:
        return Property::Label;
    case WidgetType::Widget: {
        auto r = render();
        if (r == Property::Widget)
            r = previewRender();
        return r;
    }
    default:
        return Property::Widget;
    }
}

void WidgetData::toXml(XmlStreamWriter& xml) const
{
    xml.writeStartElement(typeStr());

    writeAttributes(xml);

    if (m_type == WidgetType::Applet) {
        xml.writeCharacters(m_attrs.appletCode);
    }

    for (int i = 0; i < childCount(); ++i) {
        child(i)->toXml(xml);
    }

    convertersToXml(xml);

    xml.writeEndElement();
}

void WidgetData::convertersToXml(XmlStreamWriter& xml) const
{
    for (auto& item : qAsConst(m_converters)) {
        item->toXml(xml);
    }
}

void WidgetData::writeAttributes(XmlStreamWriter& xml) const
{
    QMetaEnum meta = Property::propertyEnum();
    for (const QString& name : m_propertiesOrder) {
        bool ok;
        int key = meta.keyToValue(name.toLatin1().data(), &ok);
        QString value;
        if (ok) {
            auto it = reflection.find(key);
            value = it.value()->getStr(*this);
        } else {
            value = m_attrs.otherAttributes.value(name);
        }
        if (!value.isNull())
            xml.writeAttribute(name, value);
    }
    for (auto it = reflection.cbegin(); it != reflection.cend(); ++it) {
        const int key = it.key();
        if (m_type == WidgetType::Applet)
            continue; // This is ugly hack!
        if (key > Property::preview)
            continue;
        if (m_propertiesOrder.contains(meta.valueToKey(key)))
            continue;
        QString value = it.value()->getStr(*this);
        QString name = Property::propertyEnum().valueToKey(key);
        if (!value.isNull())
            xml.writeAttribute(name, value);
    }
    for (auto it = m_attrs.otherAttributes.cbegin(); it != m_attrs.otherAttributes.cend(); ++it) {
        if (!m_propertiesOrder.contains(it.key())) {
            if (!it.value().isNull())
                xml.writeAttribute(it.key(), it.value());
        }
    }
}

QVariant WidgetData::getAttr(int key) const
{
    auto it = reflection.find(key);
    if (it != reflection.cend()) {
        return (*it)->get(*this);
    } else {
        return QVariant();
    }
}

bool WidgetData::setAttr(int key, const QVariant& value)
{
    auto it = reflection.find(key);
    if (it != reflection.cend()) {
        (*it)->set(*this, value);
        return value.canConvert((*it)->type());
    }
    return false;
}

QString WidgetData::getAttr(const QString& key) const
{
    auto it = m_attrs.otherAttributes.find(key);
    if (it != m_attrs.otherAttributes.cend()) {
        return *it;
    }
    return QString();
}

WidgetData* WidgetData::clone() const
{
    // Model of copied widget should be nullptr
    auto* copied = new WidgetData(m_type);
    copied->m_attrs = m_attrs;
    copied->m_propertiesOrder = m_propertiesOrder;

    // Ugly hack with xml serialization
    QBuffer buf;
    buf.open(QIODevice::WriteOnly);
    XmlStreamWriter writer(&buf);
    convertersToXml(writer);
    buf.close();

    buf.open(QIODevice::ReadOnly);
    QXmlStreamReader reader(&buf);
    while (reader.readNextStartElement()) {
        copied->appendConverterFromXml(reader);
    }

    // Copy children recursively
    for (int i = 0; i < childCount(); ++i) {
        WidgetData* childCopy = child(i)->clone();
        copied->appendChild(childCopy);
    }

    return copied;
}

void WidgetData::onColorChanged(const QString& name, QRgb value)
{
    for (auto it = m_attrs.colors.begin(); it != m_attrs.colors.end(); ++it) {
        auto& col = *it;
        if (col.name() == name) {
            col.updateValue(value);
            notifyAttrChange(it.key());
        }
    }
}

void WidgetData::onStyledColorChanged(WindowStyleColor::ColorRole role, QRgb value)
{
    using Role = WindowStyleColor::ColorRole;
    switch (role) {
    case Role::Background:
        return notifyAttrChange(Property::backgroundColor);
    case Role::LabelForeground:
        return notifyAttrChange(Property::foregroundColor);
    default:
        return;
    }
}

void WidgetData::onFontChanged(const QString& name, const Font& value)
{
    notifyAttrChange(Property::font);
    // TODO: update
}

void WidgetData::updateCache()
{
    // Widget must be attached to the model
    if (!m_model) {
        return;
    }
    for (auto& col : m_attrs.colors) {
        col.reload(m_model->colors());
    }
    // TODO: fonts?
}

void WidgetData::sizeChanged()
{
    notifyAttrChange(Property::size);
    for (int i = 0; i < childCount(); ++i) {
        child(i)->parentSizeChanged();
    }
}

void WidgetData::parentSizeChanged()
{
    if (m_attrs.position.isRelative()) {
        notifyAttrChange(Property::position);
    }
    if (m_attrs.size.isRelative()) {
        notifyAttrChange(Property::size);
        for (int i = 0; i < childCount(); ++i) {
            child(i)->parentSizeChanged();
        }
    }
}

void WidgetData::notifyAttrChange(int key)
{
    if (m_model) {
        m_model->widgetAttrHasChanged(this, key);
    }
}

void WidgetData::setAttrFromXml(int key, const QString& str)
{
    auto it = reflection.find(key);
    if (it != reflection.cend()) {
        (*it)->setFromStr(*this, str);
    }
}
