#ifndef SIZEITEM_H
#define SIZEITEM_H

#include "attritem.hpp"
#include "skin/sizeattr.hpp"

class SizeItem : public AttrItem
{
public:
    SizeItem(WidgetData* widget, int key)
        : AttrItem(widget, key)
    {
    }
    QVariant data(int role = Qt::UserRole + 1) const override;
    bool setData(const QVariant& value, int role) override;

private:
    inline SizeAttr attr() const;
    inline void setAttr(const SizeAttr& attr);
};

#endif // SIZEITEM_H
