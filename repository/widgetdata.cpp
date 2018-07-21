#include "widgetdata.hpp"
#include <QDebug>
#include <QTimer>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "attr/attrgroupitem.hpp"
#include "attr/coloritem.hpp"
#include "attr/enumitem.hpp"
#include "attr/fontitem.hpp"
#include "attr/integeritem.hpp"
#include "attr/pixmapitem.hpp"
#include "attr/positionitem.hpp"
#include "attr/sizeitem.hpp"
#include "attr/textitem.hpp"
#include "attr/variantitem.hpp"

#include "adapter/attritemfactory.hpp"
#include "model/colorsmodel.hpp"
#include "model/fontsmodel.hpp"
#include "model/screensmodel.hpp"
#include "repository/skinrepository.hpp"

#include <iostream>
#include <memory>
#include <type_traits>
#include <typeinfo>

WidgetData::WidgetData(bool empty)
    : MixinTreeNode<WidgetData>()
    , UniqueId()
    , mType(Widget)
    , mConnectedCount(0)
{
    /*
    typedef std::remove_pointer<decltype(this)>::type ThisType;
    static_assert(std::is_base_of< MixinTreeNode<ThisType>, ThisType >::value,
    "bad inheratance");
    */

    mRoot = new AttrItem(this, Property::invalid);

    if (empty) {
        return;
    }

    buildPropertiesTree();
}

WidgetData::~WidgetData()
{
    delete mRoot;
}
AttrItem* WidgetData::getAttrAdapterPtr(const int key) const
{
    auto it = mAdapters.find(key);
    if (it != mAdapters.end()) {
        return *it;
    } else {
        return nullptr;
    }
}

bool WidgetData::setType(int type)
{
    int typeCount = QMetaEnum::fromType<WidgetType>().keyCount();
    if (type >= 0 && type < typeCount) {
        mType = static_cast<WidgetType>(type);
        emit typeChanged(mType);
        return true;
    }
    return false;
}

void WidgetData::setType(WidgetType type)
{
    mType = type;
    emit typeChanged(mType);
}

QVariant WidgetData::getAttr(const int key, int role) const
{
    if (role == Roles::DataRole) {
        return getVariant(key);
    }

    auto it = mAdapters.find(key);
    if (it != mAdapters.end()) {
        return it.value()->data(role);
    } else {
        return QVariant();
    }
}

bool WidgetData::setAttr(const int key, const QVariant& value, int role)
{
    if (role == Roles::DataRole) {
        bool changed = setFromVariant(key, value);
        if (changed) {
            notifyAttrChange(key);
        }
        return changed;
    }

    auto it = mAdapters.find(key);
    if (it == mAdapters.end()) {
        return false;
    } else {
        bool changed = it.value()->setData(value, role);
        if (changed)
            notifyAttrChange(key);
        return changed;
    }
}

void WidgetData::connectNotify(const QMetaMethod& signal)
{
    if (signal == QMetaMethod::fromSignal(&WidgetData::attrChanged)) {
        if (mConnectedCount == 0) {
            connect(SkinRepository::colors(), &ColorsModel::valueChanged, this,
                    &WidgetData::onColorChanged);
            connect(SkinRepository::fonts(), &FontsModel::valueChanged, this,
                    &WidgetData::onFontChanged);
        }
        mConnectedCount++;
    }
}

void WidgetData::disconnectNotify(const QMetaMethod& signal)
{
    //    return;
    if (signal == QMetaMethod::fromSignal(&WidgetData::attrChanged)) {
        mConnectedCount--;
        if (mConnectedCount == 0) {
            // use singleShot workaround to avoid deadlocks
            QTimer::singleShot(0, [this]() {
                disconnect(SkinRepository::colors(), &ColorsModel::valueChanged, this,
                           &WidgetData::onColorChanged);
                disconnect(SkinRepository::fonts(), &FontsModel::valueChanged, this,
                           &WidgetData::onFontChanged);
            });
        }
    }
}

QString WidgetData::typeStr() const
{
    switch (mType) {
    case Screen:
        return "screen";
    case Widget:
        return "widget";
    case Label:
        return "eLabel";
    case Pixmap:
        return "ePixmap";
    }
}

WidgetData::WidgetType WidgetData::strToType(QStringRef str, bool& ok)
{
    ok = true;
    if (str == "screen") {
        return Screen;
    } else if (str == "widget") {
        return Widget;
    } else if (str == "eLabel") {
        return Label;
    } else if (str == "ePixmap") {
        return Pixmap;
    } else {
        ok = false;
        return Widget;
    }
}

QSize WidgetData::selfSize() const
{
    auto size = getAttr<SizeAttr>(Property::size);
    return size.getSize(*this);
}

QSize WidgetData::parentSize() const
{
    MixinTreeNode<WidgetData>* p = parent();
    if (p && p->isChild()) {
        return p->self()->selfSize();
    } else {
        return SkinRepository::instance().outputSize();
    }
}

void WidgetData::fromXml(QXmlStreamReader& xml)
{
    Q_ASSERT(xml.isStartElement());

    bool ok;
    mType = strToType(xml.name(), ok);
    Q_ASSERT(ok);

    QXmlStreamAttributes attrs = xml.attributes();
    for (auto it = attrs.cbegin(); it != attrs.cend(); ++it) {
        bool ok;
        QMetaEnum meta = Property::propertyEnum();
        int keyint = meta.keyToValue(it->name().toLatin1().data(), &ok);
        if (ok) {
            mPropertiesOrder.append(keyint);
            setAttr(keyint, it->value().toString(), XmlRole);
        } else {
            qWarning() << "unknown attribute" << it->name();
        }
    }

    while (nextXmlChild(xml)) {
        qDebug() << "child" << xml.tokenString() << xml.name() << xml.text();
        if (xml.isStartElement()) {
            if (mType == Screen) {
                bool ok;
                strToType(xml.name(), ok);
                if (ok) {
                    WidgetData* widget = new WidgetData();
                    appendChild(widget);
                    widget->fromXml(xml);
                } else {
                    xml.skipCurrentElement();
                }
            } else if (xml.name() == "convert") {
                ConverterItem c;
                c.fromXml(xml);
                mConverters.append(c);
            } else {
                xml.skipCurrentElement();
            }
        }
    }

    if (mType == Widget) {
        MixinTreeNode<WidgetData>* ptr = parent();
        if (ptr) {
            QString screen = ptr->self()->getAttr(Property::name, Roles::DataRole).toString();
            QString widget = getAttr(Property::name, Roles::DataRole).toString();
            Preview p = SkinRepository::screens()->getPreview(screen, widget);
            setAttr(Property::previewRender, p.render, Roles::GraphicsRole);
            setAttr(Property::previewValue, p.value, Roles::XmlRole);
        }
    }
}

void WidgetData::toXml(QXmlStreamWriter& xml) const
{
    xml.writeStartElement(typeStr());

    for (auto it = mPropertiesOrder.begin(); it != mPropertiesOrder.end(); ++it) {
        const int key = *it;
        if (mAdapters.contains(key)) {
            QString value = getAttr(key, XmlRole).toString();
            QString name = Property::propertyEnum().valueToKey(key);
            if (!value.isNull() && !value.isEmpty())
                xml.writeAttribute(name, value);
        }
    }
    for (auto it = mAdapters.begin(); it != mAdapters.end(); ++it) {
        const int key = it.key();
        if (key > Property::preview)
            continue;
        if (mPropertiesOrder.contains(key))
            continue;
        QString value = getAttr(key, XmlRole).toString();
        QString name = Property::propertyEnum().valueToKey(key);
        if (!value.isNull())
            xml.writeAttribute(name, value);
    }

    for (int i = 0; i < childCount(); ++i) {
        child(i)->toXml(xml);
    }
    for (const ConverterItem& item : mConverters) {
        item.toXml(xml);
    }

    xml.writeEndElement();
}

void WidgetData::onColorChanged(const QString& name, QRgb value)
{
    for (auto it = mAttrs.begin(); it != mAttrs.end(); ++it) {
        if (it.value()->typeId() == qMetaTypeId<ColorAttr>()) {
            ColorAttr old = it.value()->getValue<ColorAttr>();
            if (old.getName() == name) {
                // TODO: use given rgba value
                it.value()->setValue(ColorAttr(old.toStr()));
                notifyAttrChange(it.key());
            }
        }
    }
}

void WidgetData::onFontChanged(const QString& name, const Font& value)
{
    for (auto it = mAttrs.begin(); it != mAttrs.end(); ++it) {
        if (it.value()->typeId() == qMetaTypeId<FontAttr>()) {
            if (it.value()->getValue<FontAttr>().name() == name) {
                notifyAttrChange(it.key());
            }
        }
    }
}

void WidgetData::buildPropertiesTree()
{
    typedef TextAttr PixmapAttr; // TODO: fix this quick and dirty hack

    auto global = new AttrGroupItem("Global");
    qDebug() << global->data(Qt::DisplayRole);
    mRoot->appendChild(global);
    addProperty<TextAttr>(Property::name, global);
    addProperty<PositionAttr>(Property::position, global);
    addProperty<SizeAttr>(Property::size, global);
    addProperty<IntegerAttr>(Property::zPosition, global);
    addProperty<ColorAttr>(Property::foregroundColor, global);
    addProperty<ColorAttr>(Property::backgroundColor, global);
    addProperty<IntegerAttr>(Property::transparent, global);
    addProperty<ColorAttr>(Property::borderColor, global);
    addProperty<IntegerAttr>(Property::borderWidth, global);

    auto text = new AttrGroupItem("Label");
    mRoot->appendChild(text);
    addProperty<TextAttr>(Property::text, text);
    addProperty<FontAttr>(Property::font, text);
    addProperty<VAlignAttr>(Property::valign, text);
    addProperty<HAlignAttr>(Property::halign, text);
    addProperty<ColorAttr>(Property::shadowColor, text);
    addProperty<TextAttr>(Property::shadowOffset, text);

    auto pixmap = new AttrGroupItem("Pixmap");
    mRoot->appendChild(pixmap);
    addProperty<TextAttr>(Property::pixmap, pixmap);
    addProperty<AlphatestAttr>(Property::alphatest, pixmap);
    addProperty<IntegerAttr>(Property::scale, pixmap);

    auto screen = new AttrGroupItem("Screen");
    mRoot->appendChild(screen);
    addProperty<TextAttr>(Property::title, screen);
    addProperty<FlagsAttr>(Property::flags, screen);

    auto listbox = new AttrGroupItem("Listbox");
    mRoot->appendChild(listbox);
    addProperty<IntegerAttr>(Property::itemHeight, listbox);
    addProperty<PixmapAttr>(Property::selectionPixmap, listbox);
    addProperty<ScrollbarModeAttr>(Property::scrollbarMode, listbox);
    addProperty<IntegerAttr>(Property::enableWrapAround, listbox);
    addProperty<ColorAttr>(Property::foregroundColorSelected, listbox);
    addProperty<ColorAttr>(Property::backgroundColorSelected, listbox);

    auto slider = new AttrGroupItem("Slider");
    mRoot->appendChild(slider);
    addProperty<PixmapAttr>(Property::sliderPixmap, slider);
    addProperty<PixmapAttr>(Property::backgroundPixmap, slider);
    addProperty<OrientationAttr>(Property::orientation, slider);

    auto gauge = new AttrGroupItem("PositionGauge");
    mRoot->appendChild(gauge);
    addProperty<PixmapAttr>(Property::pointer, gauge);
    addProperty<PixmapAttr>(Property::seek_pointer, gauge);

    auto widget = new AttrGroupItem("Widget");
    mRoot->appendChild(widget);
    addProperty<RenderAttr>(Property::render, widget);
    addProperty<TextAttr>(Property::source, widget);
    addProperty<RenderAttr>(Property::previewRender, widget);
    addProperty<VariantAttr>(Property::previewValue, widget);

    //	mPosition = static_cast<PositionItem*>(getAttrPtr(Property::position));
    //	mSize = static_cast<SizeItem*>(getAttrPtr(Property::size));
}

void WidgetData::parentSizeChanged()
{
    auto pos = getAttr<PositionAttr>(Property::position);
    if (pos.isRelative()) {
        notifyAttrChange(Property::position);
    }
    auto size = getAttr<SizeAttr>(Property::size);
    if (size.isRelative()) {
        notifyAttrChange(Property::size);
        for (int i = 0; i < childCount(); ++i) {
            child(i)->parentSizeChanged();
        }
    }
}

void WidgetData::notifyAttrChange(int key)
{
    emit attrChanged(key);
    SkinRepository::screens()->widgetAttrHasChanged(this, key);
}

/*
template<> PositionAttr WidgetData::get(int k)
{
    if (k == Property::position) {
        return mPosition;
    } else {
        return PositionAttr();
    }
}

template<> bool WidgetData::set(int k, PositionAttr &value)
{
    if (k == Property::position) {
        mPosition = value;
        emit attrChanged(k);
        return true;
    } else {
        return false;
    }
}

template<> SizeAttr WidgetData::get(int k)
{
    if (k == Property::size) {
        return mSize;
    } else {
        return SizeAttr();
    }
}

template<> bool WidgetData::set(int k, SizeAttr &value)
{
    if (k == Property::size) {
        mSize = value;
        emit attrChanged(k);
        return true;
    } else {
        return false;
    }
}

template<> ColorAttr WidgetData::get(int k) {
    return getHashMember(mColors, k);
}
template<> bool WidgetData::set(int k, ColorAttr value) {
    return setHashMember(mColors, k, value);
}

template<typename T>
WidgetData::ContainerBase*WidgetData::Container<T>::copy() const {
    return new Container<T>(mData);
}
*/

// ConverterItem

void ConverterItem::fromXml(QXmlStreamReader& xml)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == "convert");

    mText.clear();
    mType = xml.attributes().value("type").toString();
    xml.readNext();
    if (xml.isCharacters()) {
        mText = xml.text().toString();
    }
    if (!xml.isEndElement()) {
        xml.skipCurrentElement();
    }
}

void ConverterItem::toXml(QXmlStreamWriter& xml) const
{
    xml.writeStartElement("convert");
    xml.writeAttribute("type", mType);
    xml.writeCharacters(mText);
    xml.writeEndElement();
}
