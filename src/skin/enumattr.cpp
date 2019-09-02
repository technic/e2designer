#include "enumattr.hpp"
#include "enums.hpp"
#include <QDebug>

// EnumAttr

template<class Enum>
EnumAttr<Enum>::EnumAttr()
{
    QMetaEnum meta = QMetaEnum::fromType<Enum>();
    if (meta.keyCount() > 0)
        m_value = meta.value(0);
    else
        m_value = 0;
}

template<class Enum>
EnumAttr<Enum>::EnumAttr(const QString& str)
{
    QMetaEnum meta = QMetaEnum::fromType<Enum>();
    bool ok;
    m_value = meta.keyToValue(str.toLatin1().data(), &ok);
    if (!ok) {
        qWarning() << "bad enum " << str;
        m_value = meta.value(0);
    }
}

template<class Enum>
EnumAttr<Enum>::EnumAttr(const int value)
{
    // TODO: bounds check?
    m_value = value;
}

template<class Enum>
QString EnumAttr<Enum>::toStr(bool emptyDefault) const
{
    QMetaEnum meta = QMetaEnum::fromType<Enum>();
    if (meta.keyCount() > 0) {
        if (emptyDefault && m_value == meta.value(0))
            return QString();
        else
            return meta.valueToKey(m_value);
    }
    return QString();
}

template<class Enum>
QStringList EnumAttr<Enum>::toStrList() const
{
    QMetaEnum meta = QMetaEnum::fromType<Enum>();
    QStringList list;
    for (int i = 0; i < meta.keyCount(); ++i) {
        list.append(QString(meta.key(i)));
    }
    return list;
}

// instanciate templates
template class EnumAttr<PropertyVAlign::Enum>;
template class EnumAttr<PropertyHAlign::Enum>;
template class EnumAttr<Property::Alphatest>;
template class EnumAttr<Property::Orientation>;
template class EnumAttr<Property::ScrollbarMode>;
template class EnumAttr<Property::Render>;
template class EnumAttr<Property::Flags>;
