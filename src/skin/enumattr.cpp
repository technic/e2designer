#include "enumattr.hpp"
#include "enums.hpp"
#include <QDebug>

// instantiate templates
template class EnumAttr<PropertyVAlign::Enum>;
template class EnumAttr<PropertyHAlign::Enum>;
template class EnumAttr<Property::Alphatest>;
template class EnumAttr<Property::Orientation>;
template class EnumAttr<Property::ScrollbarMode>;
template class EnumAttr<Property::Render>;
template class EnumAttr<Property::Flags>;
