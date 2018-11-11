#ifndef ENUMATTR_HPP
#define ENUMATTR_HPP

#include "skin/enums.hpp"
#include <QString>
#include <QStringList>
#include <type_traits>

/**
 * @brief Attribute represented by enum
 */
template <class Enum>
class EnumAttr
{
public:
    EnumAttr();
    EnumAttr(const QString& str);
    EnumAttr(const int value);

    inline Enum value() const { return Enum(mValue); }
    QString toStr(bool emptyDefault = true) const;
    QString toXml() { return toStr(true); }
    QStringList toStrList() const;

private:
    int mValue;
};

template<class T, typename = typename std::enable_if<std::is_enum<T>::value>::type>
inline QString serialize(T value) {
    return EnumAttr<T>(value).toStr();
}
template<class T, typename = typename std::enable_if<std::is_enum<T>::value>::type>
inline void deserialize(const QString &str, T &value) {
    value = EnumAttr<T>(str).value();
}


typedef EnumAttr<PropertyVAlign::Enum> VAlignAttr;
Q_DECLARE_METATYPE(VAlignAttr);

typedef EnumAttr<PropertyHAlign::Enum> HAlignAttr;
Q_DECLARE_METATYPE(HAlignAttr);

typedef EnumAttr<Property::Alphatest> AlphatestAttr;
Q_DECLARE_METATYPE(AlphatestAttr);

typedef EnumAttr<Property::Orientation> OrientationAttr;
Q_DECLARE_METATYPE(OrientationAttr);

typedef EnumAttr<Property::ScrollbarMode> ScrollbarModeAttr;
Q_DECLARE_METATYPE(ScrollbarModeAttr);

typedef EnumAttr<Property::Render> RenderAttr;
Q_DECLARE_METATYPE(RenderAttr);

typedef EnumAttr<Property::Flags> FlagsAttr;
Q_DECLARE_METATYPE(FlagsAttr);


#endif // ENUMATTR_HPP
