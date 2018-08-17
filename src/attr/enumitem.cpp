#include "enumitem.hpp"
#include "repository/widgetdata.hpp"
#include <QDebug>


// EnumItem

template <class Enum>
QVariant EnumItem<Enum>::data(int role) const
{
    switch (role) {
    case Roles::GraphicsRole:
        return attr().value();
    case Roles::StringListRole:
        return attr().toStrList();
    case Qt::DisplayRole:
        return attr().toStr();
    case Qt::EditRole:
        return QVariant::fromValue(SkinEnumList(attr().toStr(), attr().toStrList()));
    case Roles::XmlRole:
        return attr().toStr(true);
    default:
        return AttrItem::data(role);
    }
}

template <class Enum>
bool EnumItem<Enum>::setData(const QVariant& value, int role)
{
    switch (role) {
    case Roles::DataRole:
    case Roles::GraphicsRole:
        setAttr(EnumAttr<Enum>(value.toInt()));
        break;
    case Roles::XmlRole:
        setAttr(EnumAttr<Enum>(value.toString()));
        break;
    case Qt::EditRole:
        setAttr(EnumAttr<Enum>(qvariant_cast<SkinEnumList>(value).first));
        break;
    default:
        return AttrItem::setData(value, role);
    }
    return true;
}

template <class Enum>
EnumAttr<Enum> EnumItem<Enum>::attr() const
{
    return pWidget->getAttr<EnumAttr<Enum>>(key());
}

template <class Enum>
void EnumItem<Enum>::setAttr(const EnumAttr<Enum>& attr)
{
    pWidget->setAttr<EnumAttr<Enum>>(key(), attr);
}

// instanciate templates
template class EnumItem<PropertyVAlign::Enum>;
template class EnumItem<PropertyHAlign::Enum>;
template class EnumItem<Property::Alphatest>;
template class EnumItem<Property::Orientation>;
template class EnumItem<Property::ScrollbarMode>;
template class EnumItem<Property::Render>;
template class EnumItem<Property::Flags>;

static AttrItemRegistrator<VAlignAttr, VAlignItem> rVAlign;
static AttrItemRegistrator<HAlignAttr, HAlignItem> rHAlign;
static AttrItemRegistrator<AlphatestAttr, AlphatestItem> rAlphatest;
static AttrItemRegistrator<OrientationAttr, OrientationItem> rOrientation;
static AttrItemRegistrator<ScrollbarModeAttr, ScrollbarModeItem> rScrollbar;
static AttrItemRegistrator<RenderAttr, RenderItem> rRender;
static AttrItemRegistrator<FlagsAttr, FlagsItem> rFlags;
