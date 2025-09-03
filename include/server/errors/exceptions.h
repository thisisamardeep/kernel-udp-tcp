#ifndef ASIOUTILS_ERRORS_EXCEPTIONS_H
#define ASIOUTILS_ERRORS_EXCEPTIONS_H

#include "source_location.h"
#include "errors/system_error.h"

#include <exception>
#include <string>
#include <utility>


namespace AsioUtils {

class Exception : public std::exception
{
public:
    explicit Exception(const std::string& message = "") : _message(message), _location() {}
    Exception(const Exception&) = default;
    Exception(Exception&&) = default;
    virtual ~Exception() = default;

    Exception& operator=(const Exception&) = default;
    Exception& operator=(Exception&&) = default;

    const std::string& message() const noexcept { return _message; }
    const SourceLocation& location() const noexcept { return _location; }

    const char* what() const noexcept override;

    virtual std::string string() const;

    friend std::ostream& operator<<(std::ostream& os, const Exception& ex)
    { os << ex.string(); return os; }

    template<class T>
    friend T&& operator+(const SourceLocation& location, T&& instance)
    { instance._location = location; return std::forward<T>(instance); }

protected:
    mutable std::string _cache;
    std::string _message;
    SourceLocation _location;
};

class ArgumentException : public Exception
{
public:
    using Exception::Exception;
};

class DomainException : public Exception
{
public:
    using Exception::Exception;
};

class RuntimeException : public Exception
{
public:
    using Exception::Exception;
};

class SecurityException : public Exception
{
public:
    using Exception::Exception;
};

class SystemException : public Exception
{
public:
    SystemException()
        : SystemException(SystemError::GetLast())
    {}
    explicit SystemException(int error)
        : SystemException(SystemError::Description(error), error)
    {}
    explicit SystemException(const std::string& message)
        : SystemException(message, SystemError::GetLast())
    {}
    explicit SystemException(const std::string& message, int error)
        : Exception(message),
          _system_error(error),
          _system_message(SystemError::Description(error))
    {}

    int system_error() const noexcept { return _system_error; }
    const std::string& system_message() const noexcept { return _system_message; }

    std::string string() const override;

protected:
    int _system_error;
    std::string _system_message;
};

}

template <>
struct fmt::formatter<AsioUtils::Exception> : formatter<std::string_view>
{
    template <typename FormatContext>
    auto format(const AsioUtils::Exception& value, FormatContext& ctx) const
    {
        return formatter<string_view>::format(value.string(), ctx);
    }
};

#endif