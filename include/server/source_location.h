#ifndef ASIOUTILS_SYSTEM_SOURCE_LOCATION_H
#define ASIOUTILS_SYSTEM_SOURCE_LOCATION_H

#include "format.h"

#include <sstream>
#include <string>

namespace AsioUtils {


    class SourceLocation
    {
        friend class Exception;

    public:
        explicit SourceLocation(const char* filename, int line) noexcept : _filename(filename), _line(line) {}
        SourceLocation(const SourceLocation&) noexcept = default;
        SourceLocation(SourceLocation&&) noexcept = default;
        ~SourceLocation() noexcept = default;

        SourceLocation& operator=(const SourceLocation&) noexcept = default;
        SourceLocation& operator=(SourceLocation&&) noexcept = default;

        const char* filename() const noexcept { return _filename; }
        int line() const noexcept { return _line; }

        std::string string() const
        { std::stringstream ss; ss << *this; return ss.str(); }

        friend std::ostream& operator<<(std::ostream& os, const SourceLocation& source_location);

    private:
        const char* _filename;
        int _line;

        SourceLocation() noexcept : SourceLocation(nullptr, 0) {}
    };

}

namespace AsioUtils {

    inline std::ostream& operator<<(std::ostream& os, const SourceLocation& source_location)
    {
        if ((source_location.filename() == nullptr) || (source_location.line() == 0))
            return os;

        return os << source_location.filename() << ':' << source_location.line();
    }

}

template <> struct fmt::formatter<AsioUtils::SourceLocation> : ostream_formatter {};

#endif