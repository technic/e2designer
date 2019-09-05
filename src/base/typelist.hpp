#pragma once

#include <limits>
#include <type_traits>

/**
 * @brief Keep list of types provided by variadic template arguments.
 * Provide constexpr function to get index of specific type in the list.
 */

template<typename... Ts>
class TypeList
{
private:
    static constexpr auto no_index = std::numeric_limits<size_t>::max();

    template<typename T>
    static constexpr size_t getIndexImpl()
    {
        auto seq = { same<T, Ts>()... };
        size_t i = 0u;
        for (auto s : seq) {
            if (s) {
                return i;
            }
            i++;
        }
        return no_index;
    }
    template<typename U, typename V>
    static constexpr bool same()
    {
        return std::is_same<U, V>::value;
    }

public:
    template<typename T>
    static constexpr size_t getIndex()
    {
        constexpr size_t index = getIndexImpl<T>();
        static_assert(index != no_index, "Type T not found in TypeList");
        return index;
    }
    template<typename U>
    static constexpr size_t getIndex(U object)
    {
        using type = std::remove_cv_t<std::remove_pointer_t<U>>;
        return getIndex<type>();
        Q_UNUSED(object);
    }
};

