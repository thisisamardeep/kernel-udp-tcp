#ifndef ASIOUTILS_SYSTEM_STACK_TRACE_H
#define ASIOUTILS_SYSTEM_STACK_TRACE_H

#include "format.h"

#include <sstream>
#include <string>
#include <vector>


namespace AsioUtils {

    class StackTrace
    {
    public:
        struct Frame
        {
            void* address;
            std::string module;
            std::string function;
            std::string filename;
            int line;

            std::string string() const
            { std::stringstream ss; ss << *this; return ss.str(); }

            friend std::ostream& operator<<(std::ostream& os, const Frame& frame);
        };

    public:
        explicit StackTrace(int skip = 0);
        StackTrace(const StackTrace&) = default;
        StackTrace(StackTrace&&) noexcept = default;
        ~StackTrace() = default;

        StackTrace& operator=(const StackTrace&) = default;
        StackTrace& operator=(StackTrace&&) noexcept = default;

        const std::vector<Frame>& frames() const noexcept { return _frames; }

        std::string string() const
        { std::stringstream ss; ss << *this; return ss.str(); }

        friend std::ostream& operator<<(std::ostream& os, const StackTrace& stack_trace);

    private:
        std::vector<Frame> _frames;
    };

}

template <> struct fmt::formatter<AsioUtils::StackTrace> : ostream_formatter {};

#endif