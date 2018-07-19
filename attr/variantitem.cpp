#include "variantitem.hpp"
#include "repository/widgetdata.hpp"
#include "adapter/attritemfactory.hpp"


QVariant VariantItem::data(int role) const
{
    switch (role) {
    case Roles::GraphicsRole:
        return pWidget->getAttr<QVariant>(Property::previewValue);
    case Qt::DisplayRole:
    case Qt::EditRole:
    case Roles::XmlRole:
        return pWidget->getAttr<QVariant>(Property::previewValue).toString();
    default:
        return AttrItem::data(role);
    }
}

bool VariantItem::setData(const QVariant &value, int role)
{
    switch (role) {
    case Roles::GraphicsRole:
    case Qt::DisplayRole:
    case Qt::EditRole:
    case Roles::XmlRole:
        return pWidget->setAttr(Property::previewValue, value);
    default:
        return AttrItem::setData(value, role);
    }
}

static AttrItemRegistrator<VariantAttr, VariantItem> registrator;
