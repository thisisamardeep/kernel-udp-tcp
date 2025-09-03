#ifndef ASIOUTILS_SYSTEM_UUID_H
#define ASIOUTILS_SYSTEM_UUID_H

#include "errors/exceptions.h"
#include <array>
#include <string>

namespace AsioUtils {
class UUID {
public:
    constexpr UUID() : _data() { _data.fill(0); }
    template<size_t N>
    explicit constexpr UUID(const char(&uuid)[N]) : UUID(uuid, N) {}
    explicit constexpr UUID(const char* uuid, size_t size);
    explicit UUID(const std::string& uuid) : UUID(uuid.data(), uuid.size()) {}
    explicit UUID(const std::array<uint8_t, 16>& data) : _data(data) {}
    UUID(const UUID&) = default;
    UUID(UUID&&) noexcept = default;
    ~UUID() = default;
    UUID& operator=(const std::string& uuid) { _data = UUID(uuid).data(); return *this; }
    UUID& operator=(const std::array<uint8_t, 16>& data) { _data = data; return *this; }
    UUID& operator=(const UUID&) = default;
    UUID& operator=(UUID&&) noexcept = default;
    friend bool operator==(const UUID& uuid1, const UUID& uuid2) { return uuid1._data == uuid2._data; }
    friend bool operator!=(const UUID& uuid1, const UUID& uuid2) { return uuid1._data != uuid2._data; }
    friend bool operator<(const UUID& uuid1, const UUID& uuid2) { return uuid1._data < uuid2._data; }
    friend bool operator>(const UUID& uuid1, const UUID& uuid2) { return uuid1._data > uuid2._data; }
    friend bool operator<=(const UUID& uuid1, const UUID& uuid2) { return uuid1._data <= uuid2._data; }
    friend bool operator>=(const UUID& uuid1, const UUID& uuid2) { return uuid1._data >= uuid2._data; }
    explicit operator bool() const noexcept { return *this != Nil(); }
    std::array<uint8_t, 16>& data() noexcept { return _data; }
    const std::array<uint8_t, 16>& data() const noexcept { return _data; }
    std::string string() const;
    static UUID Nil() { return UUID(); }
    static UUID Sequential();
    static UUID Random();
    friend std::ostream& operator<<(std::ostream& os, const UUID& uuid) { os << uuid.string(); return os; }
    void swap(UUID& uuid) noexcept;
    friend void swap(UUID& uuid1, UUID& uuid2) noexcept;
private:
    std::array<uint8_t, 16> _data;
};

constexpr AsioUtils::UUID operator ""_uuid(const char* uuid, size_t size) { return AsioUtils::UUID(uuid, size); }

namespace Internals {
inline constexpr uint8_t unhex(char ch) {
    if ((ch >= '0') && (ch <= '9')) return ch - '0';
    else if ((ch >= 'a') && (ch <= 'f')) return 10 + ch - 'a';
    else if ((ch >= 'A') && (ch <= 'F')) return 10 + ch - 'A';
    else throw AsioUtils::SourceLocation(__FILE__, __LINE__) + DomainException("Invalid UUID character");
}
}

inline constexpr UUID::UUID(const char* uuid, size_t size) : _data{} {
    char v1 = 0;
    char v2 = 0;
    bool pack = false;
    size_t index = 0;
    for (size_t i = 0; i < size; ++i) {
        char ch = uuid[i];
        if ((ch == '-') || (ch == '{') || (ch == '}')) continue;
        if (pack) {
            v2 = ch;
            pack = false;
            uint8_t ui1 = Internals::unhex(v1);
            uint8_t ui2 = Internals::unhex(v2);
            if ((ui1 > 15) || (ui2 > 15)) throw AsioUtils::SourceLocation(__FILE__, __LINE__) + ArgumentException("Invalid UUID literal");
            _data[index++] = ui1 * 16 + ui2;
            if (index >= 16) break;
        } else {
            v1 = ch;
            pack = true;
        }
    }
    for (; index < 16; ++index) _data[index++] = 0;
}

inline void UUID::swap(UUID& uuid) noexcept {
    using std::swap;
    swap(_data, uuid._data);
}

inline void swap(UUID& uuid1, UUID& uuid2) noexcept {
    uuid1.swap(uuid2);
}
}

template <>
struct fmt::formatter<AsioUtils::UUID> : formatter<std::string_view> {
    template <typename FormatContext>
    auto format(const AsioUtils::UUID& value, FormatContext& ctx) const {
        return formatter<string_view>::format(value.string(), ctx);
    }
};

template <>
struct std::hash<AsioUtils::UUID> {
    typedef AsioUtils::UUID argument_type;
    typedef size_t result_type;
    result_type operator() (const argument_type& value) const {
        result_type result = 17;
        std::hash<uint8_t> hasher;
        for (size_t i = 0; i < value.data().size(); ++i) result = result * 31 + hasher(value.data()[i]);
        return result;
    }
};

#endif