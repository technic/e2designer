#ifndef TYPELIST_HPP
#define TYPELIST_HPP

#include <limits>
#include <type_traits>

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
        constexpr bool found = getIndexImpl<T>() != no_index;
        static_assert(found, "Type T not found in TypeList");
        return getIndexImpl<T>();
    }
};

#endif // TYPELIST_HPP
