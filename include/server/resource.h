

#ifndef ASIOUTILS_UTILITY_RESOURCE_H
#define ASIOUTILS_UTILITY_RESOURCE_H

#include <memory>

namespace AsioUtils {

    template <typename T, typename TCleaner>
    auto resource(T handle, TCleaner cleaner)
    {
        return std::unique_ptr<typename std::remove_pointer<T>::type, TCleaner>(handle, cleaner);
    }

    template <typename TCleaner>
    auto resource(void* handle, TCleaner cleaner)
    {
        return std::unique_ptr<void, TCleaner>(handle, cleaner);
    }


    template <typename TCleaner>
    auto resource(TCleaner cleaner)
    {
        return std::unique_ptr<void, TCleaner>(&cleaner, cleaner);
    }

}

#endif
