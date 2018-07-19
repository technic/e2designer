#include "attritemfactory.hpp"
#include "repository/widgetdata.hpp"
#include <QApplication>
#include <QPalette>

// AttrAdapterFactory

AttrItemFactory::~AttrItemFactory()
{
    qDeleteAll(mCreatorsMap.values());
    mCreatorsMap.clear();
}

void AttrItemFactory::doRegisterAdapter(int type, AttrItemCreatorBase *creator)
{
    auto it = mCreatorsMap.find(type);
    if (it != mCreatorsMap.end()) {
        delete it.value();
        mCreatorsMap.erase(it);
    }
    mCreatorsMap[type] = creator;
}

AttrItem* AttrItemFactory::doCreateAdapter(WidgetData *widget, int attrType, int attrKey)
{
    auto it = mCreatorsMap.find(attrType);
    if (it != mCreatorsMap.end()) {
        return it.value()->createAdapter(widget, attrKey);
    } else {
        return new AttrItem(widget, attrKey);
    }
}
