#ifndef META_HPP
#define META_HPP

#include <QMetaType>

/**
 * @brief Wrapper around QMetaType::registerConverter()
 */

struct ConverterRegistrator
{
    template<typename Function>
    ConverterRegistrator(Function f) {
        QMetaType::registerConverter(f);
    }
};

#endif // META_HPP
