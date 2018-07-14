#ifndef TEXTITEM_H
#define TEXTITEM_H

#include "attritem.h"

using TextAttr = QString;

class TextItem : public AttrItem
{
public:
    TextItem(WidgetData *widget, int key) : AttrItem(widget, key) { }
    QVariant data(int role = Qt::UserRole + 1) const override;
    bool setData(const QVariant &value, int role = Qt::UserRole + 1) override;
};


#endif // TEXTITEM_H
