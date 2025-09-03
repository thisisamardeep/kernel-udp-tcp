#ifndef ASIOWRAPPER_ASIO_MEMORY_H
#define ASIOWRAPPER_ASIO_MEMORY_H

#include <memory>

namespace AsioWrapper {
namespace Asio {

class HandlerStorage
{
public:
    HandlerStorage() noexcept : _in_use(false) {}
    HandlerStorage(const HandlerStorage&) = delete;
    HandlerStorage(HandlerStorage&&) = delete;
    ~HandlerStorage() noexcept = default;

    HandlerStorage& operator=(const HandlerStorage&) = delete;
    HandlerStorage& operator=(HandlerStorage&&) = delete;

    void* allocate(size_t size);
    void deallocate(void* ptr);

private:
    bool _in_use;
    std::byte _storage[1024];
};

template <typename T>
class HandlerAllocator
{
    template <typename>
    friend class HandlerAllocator;

public:
    typedef T value_type;
    typedef T* pointer;
    typedef T& reference;
    typedef const T* const_pointer;
    typedef const T& const_reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

    explicit HandlerAllocator(HandlerStorage& storage) noexcept : _storage(storage) {}
    template <typename U>
    HandlerAllocator(const HandlerAllocator<U>& alloc) noexcept : _storage(alloc._storage) {}
    HandlerAllocator(const HandlerAllocator& alloc) noexcept : _storage(alloc._storage) {}
    HandlerAllocator(HandlerAllocator&&) noexcept = default;
    ~HandlerAllocator() noexcept = default;

    template <typename U>
    HandlerAllocator& operator=(const HandlerAllocator<U>& alloc) noexcept
    { _storage = alloc._storage; return *this; }
    HandlerAllocator& operator=(const HandlerAllocator& alloc) noexcept
    { _storage = alloc._storage; return *this; }
    HandlerAllocator& operator=(HandlerAllocator&&) noexcept = default;

    pointer allocate(size_type num, const void* hint = 0) { return (pointer)_storage.allocate(num * sizeof(T)); }
    void deallocate(pointer ptr, size_type num) { return _storage.deallocate(ptr); }

private:
    HandlerStorage& _storage;
};

template <typename THandler>
class AllocateHandler
{
public:
    typedef HandlerAllocator<THandler> allocator_type;

    AllocateHandler(HandlerStorage& storage, THandler handler) noexcept : _storage(storage), _handler(handler) {}
    AllocateHandler(const AllocateHandler&) noexcept = default;
    AllocateHandler(AllocateHandler&&) noexcept = default;
    ~AllocateHandler() noexcept = default;

    AllocateHandler& operator=(const AllocateHandler&) noexcept = default;
    AllocateHandler& operator=(AllocateHandler&&) noexcept = default;

    allocator_type get_allocator() const noexcept { return allocator_type(_storage); }

    template <typename ...Args>
    void operator()(Args&&... args) { _handler(std::forward<Args>(args)...); }

private:
    HandlerStorage& _storage;
    THandler _handler;
};

template <typename THandler>
AllocateHandler<THandler> make_alloc_handler(HandlerStorage& storage, THandler handler);

}
}



namespace AsioWrapper {
    namespace Asio {

        inline void* HandlerStorage::allocate(size_t size)
        {
            if (!_in_use && (size < sizeof(_storage)))
            {
                _in_use = true;
                return &_storage;
            }

            return ::operator new(size);
        }

        inline void HandlerStorage::deallocate(void* ptr)
        {
            if (ptr == &_storage)
            {
                _in_use = false;
                return;
            }

            ::operator delete(ptr);
        }

        template <typename THandler>
        inline AllocateHandler<THandler> make_alloc_handler(HandlerStorage& storage, THandler handler)
        {
            return AllocateHandler<THandler>(storage, handler);
        }

    }
}

#endif