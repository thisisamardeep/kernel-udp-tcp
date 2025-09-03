

#ifndef ASIOUTILS_FLAGS_H
#define ASIOUTILS_FLAGS_H

#include <bitset>
#include <type_traits>

namespace AsioUtils {

template <typename TEnum>
struct IsEnumFlags : public std::false_type {};


#define ENUM_FLAGS(type)\
using AsioUtils::operator&;\
using AsioUtils::operator|;\
using AsioUtils::operator^;\
namespace AsioUtils {\
template <> struct IsEnumFlags<type> : std::true_type {};\
}


template <typename TEnum>
class Flags
{
    typedef typename std::make_unsigned<typename std::underlying_type<TEnum>::type>::type type;

public:
    Flags() noexcept : _value(0) {}
    Flags(type value) noexcept : _value(value) {}
    Flags(TEnum value) noexcept : _value((type)value) {}
    Flags(const Flags&) noexcept = default;
    Flags(Flags&&) noexcept = default;
    ~Flags() noexcept = default;

    Flags& operator=(type value) noexcept
    { _value = value; return *this; }
    Flags& operator=(TEnum value) noexcept
    { _value = (type)value; return *this; }
    Flags& operator=(const Flags&) noexcept = default;
    Flags& operator=(Flags&&) noexcept = default;

    explicit operator bool() const noexcept { return isset(); }

    bool operator!() const noexcept { return !isset(); }

    Flags operator~() const noexcept { return Flags(~_value); }

    Flags& operator&=(const Flags& flags) noexcept
    { _value &= flags._value; return *this; }
    Flags& operator|=(const Flags& flags) noexcept
    { _value |= flags._value; return *this; }
    Flags& operator^=(const Flags& flags) noexcept
    { _value ^= flags._value; return *this; }

    friend Flags operator&(const Flags& flags1, const Flags& flags2) noexcept
    { return Flags(flags1._value & flags2._value); }
    friend Flags operator|(const Flags& flags1, const Flags& flags2) noexcept
    { return Flags(flags1._value | flags2._value); }
    friend Flags operator^(const Flags& flags1, const Flags& flags2) noexcept
    { return Flags(flags1._value ^ flags2._value); }

    friend bool operator==(const Flags& flags1, const Flags& flags2) noexcept
    { return flags1._value == flags2._value; }
    friend bool operator!=(const Flags& flags1, const Flags& flags2) noexcept
    { return flags1._value != flags2._value; }

    operator TEnum() const noexcept { return (TEnum)_value; }

    bool isset() const noexcept { return (_value != 0); }
    bool isset(type value) const noexcept { return (_value & value) != 0; }
    bool isset(TEnum value) const noexcept { return (_value & (type)value) != 0; }

    TEnum value() const noexcept { return (TEnum)_value; }
    type underlying() const noexcept { return _value; }
    std::bitset<sizeof(type) * 8> bitset() const noexcept { return {_value}; }

    void swap(Flags& flags) noexcept { using std::swap; swap(_value, flags._value); }
    template <typename UEnum>
    friend void swap(Flags<UEnum>& flags1, Flags<UEnum>& flags2) noexcept;

private:
    type _value;
};


}


namespace AsioUtils {

    template <typename TEnum>
     void swap(Flags<TEnum>& flags1, Flags<TEnum>& flags2) noexcept
    {
        flags1.swap(flags2);
    }

    template <typename TEnum>
    constexpr auto operator&(TEnum value1, TEnum value2) noexcept -> typename std::enable_if<IsEnumFlags<TEnum>::value, Flags<TEnum>>::type
    {
        return Flags<TEnum>(value1) & value2;
    }

    template <typename TEnum>
    constexpr auto operator|(TEnum value1, TEnum value2) noexcept -> typename std::enable_if<IsEnumFlags<TEnum>::value, Flags<TEnum>>::type
    {
        return Flags<TEnum>(value1) | value2;
    }

    template <typename TEnum>
    constexpr auto operator^(TEnum value1, TEnum value2) noexcept -> typename std::enable_if<IsEnumFlags<TEnum>::value, Flags<TEnum>>::type
    {
        return Flags<TEnum>(value1) ^ value2;
    }

}

#endif
