#ifndef ASIOUTILS_THREADS_THREAD_H
#define ASIOUTILS_THREADS_THREAD_H

#include "errors/exceptions_handler.h"
#include "timestamp.h"

#include <bitset>
#include <thread>


namespace AsioUtils {

enum class ThreadPriority : uint8_t
{
    IDLE     = 0x00,
    LOWEST   = 0x1F,
    LOW      = 0x3F,
    NORMAL   = 0x7F,
    HIGH     = 0x9F,
    HIGHEST  = 0xBF,
    REALTIME = 0xFF
};

template <class TOutputStream>
TOutputStream& operator<<(TOutputStream& stream, ThreadPriority priority);

class Thread
{
public:
    Thread() = delete;
    Thread(const Thread&) = delete;
    Thread(Thread&&) = delete;
    ~Thread() = delete;

    Thread& operator=(const Thread&) = delete;
    Thread& operator=(Thread&&) = delete;

    static uint64_t CurrentThreadId() noexcept;
    static uint32_t CurrentThreadAffinity() noexcept;

    template <class Fn, class... Args>
    static std::thread Start(Fn&& fn, Args&&... args);

    static void Sleep(int64_t milliseconds) noexcept
    { SleepFor(Timespan::milliseconds(milliseconds)); }
    static void SleepFor(const Timespan& timespan) noexcept;
    static void SleepUntil(const UtcTimestamp& timestamp) noexcept
    { SleepFor(timestamp - UtcTimestamp()); }

    static void Yield() noexcept;

    static std::bitset<64> GetAffinity();
    static std::bitset<64> GetAffinity(std::thread& thread);

    static void SetAffinity(const std::bitset<64>& affinity);
    static void SetAffinity(std::thread& thread, const std::bitset<64>& affinity);

    static ThreadPriority GetPriority();
    static ThreadPriority GetPriority(std::thread& thread);

    static void SetPriority(ThreadPriority priority);
    static void SetPriority(std::thread& thread, ThreadPriority priority);
};

}

namespace AsioUtils {

    template <class TOutputStream>
     TOutputStream& operator<<(TOutputStream& stream, ThreadPriority priority)
    {
        switch (priority)
        {
            case ThreadPriority::IDLE:
                stream << "IDLE";
                break;
            case ThreadPriority::LOWEST:
                stream << "LOWEST";
                break;
            case ThreadPriority::LOW:
                stream << "LOW";
                break;
            case ThreadPriority::NORMAL:
                stream << "NORMAL";
                break;
            case ThreadPriority::HIGH:
                stream << "HIGH";
                break;
            case ThreadPriority::HIGHEST:
                stream << "HIGHEST";
                break;
            case ThreadPriority::REALTIME:
                stream << "REALTIME";
                break;
            default:
                stream << "<unknown>";
                break;
        }
        return stream;
    }

    template <class Fn, class... Args>
     std::thread Thread::Start(Fn&& fn, Args&&... args)
    {
        return std::thread([fn = fn, args...]()
        {
            ExceptionsHandler::SetupThread();
            fn(std::move(args)...);
        });
    }

}

#endif