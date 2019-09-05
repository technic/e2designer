#pragma once

#include <QtGlobal>

template<typename T>
class SingletonMixin
{
    Q_DISABLE_COPY(SingletonMixin)
    friend T;

public:
    static T& instance()
    {
        // This can catch some errors, but not all
        static_assert(std::is_base_of<SingletonMixin<T>, T>::value,
                      "You must derive T from SingletonMixin<T>");

        static T self;
        return self;
    }

private:
    // You should derive from this class to create an instance
    SingletonMixin() = default;
};
