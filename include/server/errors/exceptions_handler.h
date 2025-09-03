#ifndef ASIOUTILS_ERRORS_EXCEPTIONS_HANDLER_H
#define ASIOUTILS_ERRORS_EXCEPTIONS_HANDLER_H

#include "singleton.h"
#include "stack_trace.h"
#include "filesystem/exceptions.h"

#include <cassert>
#include <functional>
#include <memory>

namespace AsioUtils {

    class ExceptionsHandler : public AsioUtils::Singleton<ExceptionsHandler>
    {
        friend Singleton<ExceptionsHandler>;

    public:
        ExceptionsHandler(const ExceptionsHandler&) = delete;
        ExceptionsHandler(ExceptionsHandler&&) = delete;
        ~ExceptionsHandler();

        ExceptionsHandler& operator=(const ExceptionsHandler&) = delete;
        ExceptionsHandler& operator=(ExceptionsHandler&&) = delete;

        static void SetupHandler(const std::function<void (const SystemException&, const StackTrace&)>& handler);
        static void SetupProcess();
        static void SetupThread();

    private:
        class Impl;

        Impl& impl() noexcept { return reinterpret_cast<Impl&>(_storage); }
        const Impl& impl() const noexcept { return reinterpret_cast<Impl const&>(_storage); }

        static const size_t StorageSize = 72;
        static const size_t StorageAlign = 16;

        alignas(StorageAlign) std::byte _storage[StorageSize];

        ExceptionsHandler();
    };

}

#endif