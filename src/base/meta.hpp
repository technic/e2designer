#pragma once

#include <QMetaType>
#include <QMetaEnum>

/**
 * @brief Wrapper around QMetaType::registerConverter()
 */

struct ConverterRegistrator
{
    template<typename Function>
    ConverterRegistrator(Function f)
    {
        QMetaType::registerConverter(f);
    }
};

template<typename T>
T strToEnum(const QString& str)
{
    QMetaEnum meta = QMetaEnum::fromType<T>();
    bool ok;
    int value = meta.keyToValue(str.toLatin1().data(), &ok);
    if (ok) {
        return static_cast<T>(value);
    } else {
        return static_cast<T>(meta.value(0));
    }
}

template<typename T>
QString enumToStr(T value)
{
    return QMetaEnum::fromType<T>().valueToKey(static_cast<int>(value));
}

