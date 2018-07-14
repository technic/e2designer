#ifndef VARIANTITEM_H
#define VARIANTITEM_H

#include "attritem.h"

typedef QVariant VariantAttr;

class VariantItem : public AttrItem
{
public:
    VariantItem(WidgetData *widget, int key)
        : AttrItem(widget, key) { }

    QVariant data(int role) const override;
    bool setData(const QVariant &value, int role) override;
};

#endif // VARIANTITEM_H
