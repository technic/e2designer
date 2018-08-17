#ifndef PIXMAPITEM_H
#define PIXMAPITEM_H

#include "attritem.hpp"

// TODO!

class PixmapItem : public AttrItem
{
public:
    bool setData(const QVariant& value, int role) override;
    QVariant data(int role) const override;

private:
};

#endif // PIXMAPITEM_H
