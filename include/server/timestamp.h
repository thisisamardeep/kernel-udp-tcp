#ifndef ASIOUTILS_TIME_TIMESTAMP_H
#define ASIOUTILS_TIME_TIMESTAMP_H

#include "timespan.h"

namespace AsioUtils {

class Timestamp
{
public:
    Timestamp() noexcept : _timestamp(epoch()) {}
    explicit Timestamp(uint64_t timestamp) noexcept : _timestamp(timestamp) {}
    template <class Clock, class Duration>
    explicit Timestamp(const std::chrono::time_point<Clock, Duration>& time_point) noexcept : _timestamp(std::chrono::duration_cast<std::chrono::nanoseconds>(time_point.time_since_epoch()).count()) {}
    Timestamp(const Timestamp&) noexcept = default;
    Timestamp(Timestamp&&) noexcept = default;
    ~Timestamp() noexcept = default;

    static uint64_t epoch() noexcept { return 0; }
    uint64_t total() const noexcept { return _timestamp; }


    friend Timestamp operator-(const Timestamp& timestamp, int64_t offset) noexcept
    { return Timestamp(timestamp.total() - offset); }
    friend Timestamp operator-(int64_t offset, const Timestamp& timestamp) noexcept
    { return Timestamp(offset - timestamp.total()); }
    friend Timestamp operator-(const Timestamp& timestamp, const Timespan& offset) noexcept
    { return Timestamp(timestamp.total() - offset.total()); }
    friend Timestamp operator-(const Timespan& offset, const Timestamp& timestamp) noexcept
    { return Timestamp(offset.total() - timestamp.total()); }

    friend Timespan operator-(const Timestamp& timestamp1, const Timestamp& timestamp2) noexcept
    { return Timespan(timestamp1.total() - timestamp2.total()); }


protected:
    uint64_t _timestamp;
};



class UtcTimestamp : public Timestamp
{
public:
    using Timestamp::Timestamp;
    UtcTimestamp(const Timestamp& timestamp) : Timestamp(timestamp) {}
};

class LocalTimestamp : public Timestamp
{
public:
    using Timestamp::Timestamp;
    LocalTimestamp(const Timestamp& timestamp) : Timestamp(timestamp) {}
};

class NanoTimestamp : public Timestamp
{
public:
    using Timestamp::Timestamp;
    NanoTimestamp(const Timestamp& timestamp) : Timestamp(timestamp) {}
};

class RdtsTimestamp : public Timestamp
{
public:
    using Timestamp::Timestamp;
    RdtsTimestamp(const Timestamp& timestamp) : Timestamp(timestamp) {}
};

}

namespace AsioUtils {




}

template <>
struct std::hash<AsioUtils::Timestamp>
{
    typedef AsioUtils::Timestamp argument_type;
    typedef size_t result_type;

    result_type operator() (const argument_type& value) const
    {
        result_type result = 17;
        result = result * 31 + std::hash<int64_t>()(value.total());
        return result;
    }
};

#endif