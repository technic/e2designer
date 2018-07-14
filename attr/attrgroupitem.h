#ifndef ATTRGROUPITEM_H
#define ATTRGROUPITEM_H

#include <QString>
#include "attritem.h"

class AttrGroupItem : public AttrItem
{
public:
    AttrGroupItem() :
        AttrItem(nullptr) { }
    AttrGroupItem(const QString &title) :
        AttrGroupItem() { mTitle = title; }

    QVariant keyData(int role) const override;
    QVariant data(int role = Qt::UserRole + 1) const override;

private:
    QString mTitle;
};

#endif // ATTRGROUPITEM_H
