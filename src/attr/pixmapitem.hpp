#ifndef PIXMAPITEM_H
#define PIXMAPITEM_H

#include "attritem.hpp"

// Do i need this???

class PixmapItem : public AttrItem
{
public:
    bool setData(const QVariant& value, int role) override;
    QVariant data(int role) const override;

private:
};

#endif // PIXMAPITEM_H
