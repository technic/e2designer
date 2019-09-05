#pragma once

#include "skin/enums.hpp"
#include <QString>
#include <QStringList>
#include <type_traits>

/**
 * @brief Attribute represented by enum
 */
template<class Enum>
class EnumAttr
{
public:
    EnumAttr();
    EnumAttr(const QString& str);
    EnumAttr(const int value);

    inline Enum value() const { return Enum(m_value); }
    QString toStr(bool emptyDefault = true) const;
    QString toXml() { return toStr(true); }
    QStringList toStrList() const;

private:
    int m_value;
};

template<class T, typename = typename std::enable_if<std::is_enum<T>::value>::type>
inline QString serialize(T value)
{
    return EnumAttr<T>(value).toStr();
}
template<class T, typename = typename std::enable_if<std::is_enum<T>::value>::type>
inline void deserialize(const QString& str, T& value)
{
    value = EnumAttr<T>(str).value();
}

using VAlignAttr = EnumAttr<PropertyVAlign::Enum>;
Q_DECLARE_METATYPE(VAlignAttr);

using HAlignAttr = EnumAttr<PropertyHAlign::Enum>;
Q_DECLARE_METATYPE(HAlignAttr);

using AlphatestAttr = EnumAttr<Property::Alphatest>;
Q_DECLARE_METATYPE(AlphatestAttr);

using OrientationAttr = EnumAttr<Property::Orientation>;
Q_DECLARE_METATYPE(OrientationAttr);

using ScrollbarModeAttr = EnumAttr<Property::ScrollbarMode>;
Q_DECLARE_METATYPE(ScrollbarModeAttr);

using RenderAttr = EnumAttr<Property::Render>;
Q_DECLARE_METATYPE(RenderAttr);

using FlagsAttr = EnumAttr<Property::Flags>;
Q_DECLARE_METATYPE(FlagsAttr);

