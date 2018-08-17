#ifndef FONTITEM_H
#define FONTITEM_H

#include "attritem.hpp"
#include "skin/fontattr.hpp"

class FontItem : public AttrItem
{
public:
    explicit FontItem(WidgetData* widget, int key)
        : AttrItem(widget, key)
    {
    }
    QVariant data(int role) const override;
    bool setData(const QVariant& value, int role) override;

private:
    inline FontAttr attr() const;
    inline void setAttr(const FontAttr& attr);
};

#endif // FONTITEM_H
