#ifndef INTEGERITEM_H
#define INTEGERITEM_H

#include "attritem.h"

using IntegerAttr = int;

class IntegerItem : public AttrItem
{
public:
    IntegerItem(WidgetData *widget, int key)
        : AttrItem(widget, key) { }
    QVariant data(int role) const override;
    bool setData(const QVariant &value, int role) override;
};

#endif // INTEGERITEM_H
