#ifndef ASIOUTILS_STRING_ENCODING_H
#define ASIOUTILS_STRING_ENCODING_H
#include <codecvt>
#include <locale>
#include <string>
namespace AsioUtils {
    class Encoding {
    public:
        Encoding() = delete;
        Encoding(const Encoding&) = delete;
        Encoding(Encoding&&) = delete;
        ~Encoding() = delete;
        Encoding& operator=(const Encoding&) = delete;
        Encoding& operator=(Encoding&&) = delete;
        static std::string ToUTF8(std::wstring_view wstr) {
            std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
            return convert.to_bytes(wstr.data(), wstr.data() + wstr.size());
        };
        static std::wstring FromUTF8(std::string_view str) {
            std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
            return convert.from_bytes(str.data(), str.data() + str.size());
        };

    };
}
#endif