#ifndef COLORITEM_H
#define COLORITEM_H

#include "attr/attritem.hpp"
#include "skin/colorattr.hpp"
#include <QColor>
#include <QRgb>


class ColorItem : public AttrItem
{
public:
    explicit ColorItem(WidgetData* widget, int key)
        : AttrItem(widget, key) {}
    QVariant data(int role) const override;
    bool setData(const QVariant& value, int role) override;

private:
    inline ColorAttr attr() const;
    inline void setAttr(const ColorAttr& attr);
};

#endif // COLORITEM_H
