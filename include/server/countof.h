

#ifndef ASIOUTILS_UTILITY_COUNTOF_H
#define ASIOUTILS_UTILITY_COUNTOF_H

namespace AsioUtils {

    template <typename T, size_t N>
    constexpr size_t countof(const T (&)[N]) noexcept { return N; }

    template <typename T>
    size_t countof(const T& container) noexcept { return container.size(); }

}

#endif
