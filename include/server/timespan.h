#ifndef ASIOUTILS_TIME_TIMESPAN_H
#define ASIOUTILS_TIME_TIMESPAN_H

#include <chrono>
#include "errors/exceptions.h"

namespace AsioUtils {

class Timespan
{
public:
    Timespan() noexcept : _duration(0) {}
    explicit Timespan(int64_t duration) noexcept : _duration(duration) {}
    template <class Rep, class Period>
    explicit Timespan(const std::chrono::duration<Rep, Period>& duration) noexcept : _duration(std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count()) {}
    Timespan(const Timespan&) noexcept = default;
    Timespan(Timespan&&) noexcept = default;
    ~Timespan() noexcept = default;

    int64_t seconds() const noexcept
    { return _duration / 1000000000; }
    int64_t nanoseconds() const noexcept
    { return _duration; }

    int64_t total() const noexcept
    { return _duration; }
    static Timespan milliseconds(int64_t milliseconds) noexcept
    { return Timespan(milliseconds * 1000000); }
    std::chrono::system_clock::duration chrono() const noexcept
    { return std::chrono::duration_cast<std::chrono::system_clock::duration>(std::chrono::nanoseconds(_duration)); }
    friend bool operator<(const Timespan& timespan, int64_t offset) noexcept
    { return timespan.total() < offset; }
    friend bool operator<(int64_t offset, const Timespan& timespan) noexcept
    { return offset < timespan.total(); }
    friend bool operator<(const Timespan& timespan1, const Timespan& timespan2) noexcept
    { return timespan1.total() < timespan2.total(); }
    friend bool operator==(const Timespan& timespan, int64_t offset) noexcept
    { return timespan.total() == offset; }
    friend bool operator==(int64_t offset, const Timespan& timespan) noexcept
    { return offset == timespan.total(); }
    friend bool operator==(const Timespan& timespan1, const Timespan& timespan2) noexcept
    { return timespan1.total() == timespan2.total(); }
private:
    int64_t _duration;
};

}

#endif