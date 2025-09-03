

#ifndef ASIOUTILS_MEMORY_MEMORY_H
#define ASIOUTILS_MEMORY_MEMORY_H


#include <cstddef>
#include <cstdint>

#include <memory>

namespace AsioUtils {

class Memory
{
public:
    Memory() = delete;
    Memory(const Memory&) = delete;
    Memory(Memory&&) = delete;
    ~Memory() = delete;

    Memory& operator=(const Memory&) = delete;
    Memory& operator=(Memory&&) = delete;



    static bool IsValidAlignment(size_t alignment) noexcept;

    template <typename T>
    static bool IsAligned(const T* address, size_t alignment = alignof(T)) noexcept;


    template <typename T>
    static T* Align(const T* address, size_t alignment = alignof(T), bool upwards = true) noexcept;




};


}




namespace AsioUtils {

    inline bool Memory::IsValidAlignment(size_t alignment) noexcept
    {
        return ((alignment > 0) && ((alignment & (alignment - 1)) == 0));
    }

    template <typename T>
     bool Memory::IsAligned(const T* address, size_t alignment) noexcept
    {

        uintptr_t ptr = (uintptr_t)address;
        return (ptr & (alignment - 1)) == 0;
    }

    template <typename T>
     T* Memory::Align(const T* address, size_t alignment, bool upwards) noexcept
    {

        uintptr_t ptr = (uintptr_t)address;

        if (upwards)
            return (T*)((ptr + (alignment - 1)) & -((int)alignment));
        else
            return (T*)(ptr & -((int)alignment));
    }

}

#endif
