
#ifndef ASIOUTILS_STRING_FORMAT_H
#define ASIOUTILS_STRING_FORMAT_H


#include <fmt/format.h>
#include <fmt/ostream.h>
#include <fmt/xchar.h>

namespace AsioUtils {


template <typename... T>
std::string format(fmt::format_string<T...> pattern, T&&... args);


template <typename... T>
std::wstring format(fmt::wformat_string<T...> pattern, T&&... args);


template <typename... T>
void print(fmt::format_string<T...> pattern, T&&... args);

template <typename... T>
void print(fmt::wformat_string<T...> pattern, T&&... args);


template <typename TOutputStream, typename... T>
void print(TOutputStream& stream, fmt::format_string<T...> pattern, T&&... args);


template <typename TOutputStream, typename... T>
void print(TOutputStream& stream, fmt::wformat_string<T...> pattern, T&&... args);


}



namespace AsioUtils {

    template <typename... T>
     std::string format(fmt::format_string<T...> pattern, T&&... args)
    {
        return fmt::vformat(pattern, fmt::make_format_args(args...));
    }

    template <typename... T>
     std::wstring format(fmt::wformat_string<T...> pattern, T&&... args)
    {
        return fmt::vformat(pattern, fmt::make_format_args<fmt::wformat_context>(args...));
    }

    template <typename... T>
     void print(fmt::format_string<T...> pattern, T&&... args)
    {
        return fmt::vprint(pattern, fmt::make_format_args(args...));
    }

    template <typename... T>
     void print(fmt::wformat_string<T...> pattern, T&&... args)
    {
        return fmt::vprint(pattern, fmt::make_format_args<fmt::wformat_context>(args...));
    }

    template <typename TOutputStream, typename... T>
     void print(TOutputStream& stream, fmt::format_string<T...> pattern, T&&... args)
    {
        return fmt::vprint(stream, pattern, fmt::make_format_args(args...));
    }

    template <typename TOutputStream, typename... T>
     void print(TOutputStream& stream, fmt::wformat_string<T...> pattern, T&&... args)
    {
        return fmt::vprint(stream, pattern, fmt::make_format_args<fmt::wformat_context>(args...));
    }

}


using namespace fmt::literals;


#endif
