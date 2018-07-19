#ifndef ENUMITEM_H
#define ENUMITEM_H

#include "attritem.h"
#include "adapter/attritemfactory.h"

// FIXME should we use fancy QMetaType???

#include <QPair>
#include <QStringList>

typedef QPair<QString, QStringList> SkinEnumList;
Q_DECLARE_METATYPE(SkinEnumList)

/**
 * @brief Property represented by enum
 */
template<class Enum>
class EnumAttr
{
public:
    EnumAttr();
    EnumAttr(const QString &str);
    EnumAttr(const int value);

    inline int value() const { return mValue; }
    QString toStr(bool emptyDefault = false) const;
    QStringList toStrList() const;

private:
    int mValue;
};


template<class Enum>
class EnumItem : public AttrItem
{
public:
    EnumItem(WidgetData *widget, int key)
        : AttrItem(widget, key) { }

    QVariant data(int role) const override;
    bool setData(const QVariant &value, int role) override;

private:
    inline EnumAttr<Enum> attr() const;
    inline void setAttr(const EnumAttr<Enum> &attr);

//    static AttrItemRegistrator<EnumAttr<Enum>, EnumItem<Enum>> registrator;
};


// type declarations
// instanciate in cpp file

typedef EnumAttr<PropertyVAlign::Enum> VAlignAttr;
typedef EnumItem<PropertyVAlign::Enum> VAlignItem;
Q_DECLARE_METATYPE(VAlignAttr);

typedef EnumAttr<PropertyHAlign::Enum> HAlignAttr;
typedef EnumItem<PropertyHAlign::Enum> HAlignItem;
Q_DECLARE_METATYPE(HAlignAttr);

typedef EnumAttr<Property::Alphatest> AlphatestAttr;
typedef EnumItem<Property::Alphatest> AlphatestItem;
Q_DECLARE_METATYPE(AlphatestAttr);

typedef EnumAttr<Property::Orientation> OrientationAttr;
typedef EnumItem<Property::Orientation> OrientationItem;
Q_DECLARE_METATYPE(OrientationAttr);

typedef EnumAttr<Property::ScrollbarMode> ScrollbarModeAttr;
typedef EnumItem<Property::ScrollbarMode> ScrollbarModeItem;
Q_DECLARE_METATYPE(ScrollbarModeAttr);

typedef EnumAttr<Property::Render> RenderAttr;
typedef EnumItem<Property::Render> RenderItem;
Q_DECLARE_METATYPE(RenderAttr);

typedef EnumAttr<Property::Flags> FlagsAttr;
typedef EnumItem<Property::Flags> FlagsItem;
Q_DECLARE_METATYPE(FlagsAttr);



#endif // ENUMITEM_H
