#ifndef PROPERTYITEM_H
#define PROPERTYITEM_H

#include "base/tree.hpp"
#include "skin/enums.hpp"

class WidgetData;

/**
 * @brief Provides interface to manipulate WidgetData attribute
 * Has tree structure for QAbstractItemModel
 */
class AttrItem : public MixinTreeNode<AttrItem>
{
public:
    AttrItem(WidgetData* widget, int key = Property::invalid)
        : MixinTreeNode<AttrItem>()
        , pWidget(widget)
        , mKey(key)
    {
    }
    virtual ~AttrItem() {}
    int key() const { return mKey; }
    // Iterface for QtModels
    virtual QVariant keyData(int role) const;
    virtual QVariant data(int role) const;
    virtual bool setData(const QVariant& value, int role);

protected:
    WidgetData* pWidget;

private:
    int mKey;
};

#endif // PROPERTYITEM_H
