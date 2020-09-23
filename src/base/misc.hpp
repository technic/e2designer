#pragma once

#include <memory>

/**
 * @brief Apply static_cast to pointer managed by std::unique_ptr.
 * The cast safety is same as of the original static_cast,
 * so you have to make sure that target type is correct before calling this function.
 *
 * @tparam U target type
 * @tparam T source type
 * @param uptr
 * @return std::unique_ptr<U>
 */
template<typename U, typename T>
std::unique_ptr<U> uptr_static_cast(std::unique_ptr<T>&& uptr)
{
    return std::unique_ptr<U>(static_cast<U*>(uptr.release()));
}
