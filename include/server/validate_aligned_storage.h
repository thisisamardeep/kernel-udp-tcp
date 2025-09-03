

#ifndef ASIOUTILS_UTILITY_VALIDATE_ALIGNED_STORAGE_H
#define ASIOUTILS_UTILITY_VALIDATE_ALIGNED_STORAGE_H

namespace AsioUtils {

    template <size_t ImplSize, size_t ImplAlign, size_t StorageSize, size_t StorageAlign, class Enable = void>
    class ValidateAlignedStorage;

    template <size_t ImplSize, const size_t ImplAlign, size_t StorageSize, size_t StorageAlign>
    class ValidateAlignedStorage<ImplSize, ImplAlign, StorageSize, StorageAlign, typename std::enable_if<(StorageSize >= ImplSize) && ((StorageAlign % ImplAlign) == 0)>::type> {};

}

#endif
