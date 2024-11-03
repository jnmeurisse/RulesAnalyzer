/*
uint128_t.h
An unsigned 128 bit integer type for C++

Copyright (c) 2013 - 2017 Jason Lee @ calccrypto at gmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

With much help from Auston Sterling

Thanks to Stefan Deigmüller for finding
a bug in operator*.

Thanks to François Dessenne for convincing me
to do a general rewrite of this class.

Minor changes
   - fix compilation warnings
   - explicit type cast operators
   - add numeric_limits
*/

#ifndef __UINT128_T__
#define __UINT128_T__

#include "global.h"
#include <cstdint>
#include <ostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>
#include <limits>

#if defined(RA_BIG_ENDIAN) && defined(RA_LITTLE_ENDIAN)
# error "Both RA_BIG_ENDIAN and RA_LITTLE_ENDIAN are defined"
#elif !defined(RA_BIG_ENDIAN) && !defined(RA_LITTLE_ENDIAN)
# error "RA_BIG_ENDIAN or RA_LITTLE_ENDIAN is not defined"
#endif


#define UINT128_T_EXTERN
class UINT128_T_EXTERN uint128_t;

// Give uint128_t type traits
namespace std {  // This is probably not a good idea
    template <> struct is_arithmetic <uint128_t> : std::true_type {};
    template <> struct is_integral   <uint128_t> : std::true_type {};
    template <> struct is_unsigned   <uint128_t> : std::true_type {};
}

class uint128_t{
    private:
#ifdef RT_BIG_ENDIAN
        uint64_t UPPER, LOWER;
#endif
#ifdef RA_LITTLE_ENDIAN
        uint64_t LOWER, UPPER;
#endif

    public:
        // Constructors
        uint128_t() = default;
        uint128_t(const uint128_t & rhs) = default;
        uint128_t(uint128_t && rhs) = default;

        // do not use prefixes (0x, 0b, etc.)
        // if the input string is too long, only right most characters are read
        uint128_t(const std::string & s, uint8_t base);
        uint128_t(const char *s, std::size_t len, uint8_t base);

        uint128_t(const bool & b);

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        uint128_t(const T & rhs)
#ifdef RT_BIG_ENDIAN
            : UPPER(0), LOWER(rhs)
#endif
#ifdef RA_LITTLE_ENDIAN
            : LOWER(rhs), UPPER(0)
#endif
        {
            if (std::is_signed<T>::value) {
                if (rhs < 0) {
                    UPPER = -1;
                }
            }
        }

        template <typename S, typename T, typename = typename std::enable_if <std::is_integral<S>::value && std::is_integral<T>::value, void>::type>
        constexpr uint128_t(const S & upper_rhs, const T & lower_rhs)
#ifdef RT_BIG_ENDIAN
            : UPPER(upper_rhs), LOWER(lower_rhs)
#endif
#ifdef RA_LITTLE_ENDIAN
            : LOWER(lower_rhs), UPPER(upper_rhs)
#endif
        {}

        //  RHS input args only

        // Assignment Operator
        uint128_t & operator=(const uint128_t & rhs) = default;
        uint128_t & operator=(uint128_t && rhs) = default;

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        uint128_t & operator=(const T & rhs){
            UPPER = 0;

            if (std::is_signed<T>::value) {
                if (rhs < 0) {
                    UPPER = -1;
                }
            }

            LOWER = rhs;
            return *this;
        }

        uint128_t & operator=(const bool & rhs);

        // Typecast Operators
        explicit operator bool() const;
        explicit operator uint8_t() const;
        explicit operator uint16_t() const;
        explicit operator uint32_t() const;
        explicit operator uint64_t() const;

        // Bitwise Operators
        uint128_t operator&(const uint128_t & rhs) const;

        void export_bits(std::vector<uint8_t> & ret) const;

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        uint128_t operator&(const T & rhs) const{
            return uint128_t(0, LOWER & (uint64_t) rhs);
        }

        uint128_t & operator&=(const uint128_t & rhs);

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        uint128_t & operator&=(const T & rhs){
            UPPER = 0;
            LOWER &= rhs;
            return *this;
        }

        uint128_t operator|(const uint128_t & rhs) const;

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        uint128_t operator|(const T & rhs) const{
            return uint128_t(UPPER, LOWER | (uint64_t) rhs);
        }

        uint128_t & operator|=(const uint128_t & rhs);

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        uint128_t & operator|=(const T & rhs){
            LOWER |= (uint64_t) rhs;
            return *this;
        }

        uint128_t operator^(const uint128_t & rhs) const;

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        uint128_t operator^(const T & rhs) const{
            return uint128_t(UPPER, LOWER ^ (uint64_t) rhs);
        }

        uint128_t & operator^=(const uint128_t & rhs);

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        uint128_t & operator^=(const T & rhs){
            LOWER ^= (uint64_t) rhs;
            return *this;
        }

        uint128_t operator~() const;

        // Bit Shift Operators
        uint128_t operator<<(const uint128_t & rhs) const;

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        uint128_t operator<<(const T & rhs) const{
            return *this << uint128_t(rhs);
        }

        uint128_t & operator<<=(const uint128_t & rhs);

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        uint128_t & operator<<=(const T & rhs){
            *this = *this << uint128_t(rhs);
            return *this;
        }

        uint128_t operator>>(const uint128_t & rhs) const;

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        uint128_t operator>>(const T & rhs) const{
            return *this >> uint128_t(rhs);
        }

        uint128_t & operator>>=(const uint128_t & rhs);

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        uint128_t & operator>>=(const T & rhs){
            *this = *this >> uint128_t(rhs);
            return *this;
        }

        // Logical Operators
        bool operator!() const;
        bool operator&&(const uint128_t & rhs) const;
        bool operator||(const uint128_t & rhs) const;

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        bool operator&&(const T & rhs) const{
            return static_cast <bool> (*this && rhs);
        }

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        bool operator||(const T & rhs) const{
            return static_cast <bool> (*this || rhs);
        }

        // Comparison Operators
        bool operator==(const uint128_t & rhs) const;

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        bool operator==(const T & rhs) const{
            return (!UPPER && (LOWER == (uint64_t) rhs));
        }

        bool operator!=(const uint128_t & rhs) const;

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        bool operator!=(const T & rhs) const{
            return (UPPER | (LOWER != (uint64_t) rhs));
        }

        bool operator>(const uint128_t & rhs) const;

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        bool operator>(const T & rhs) const{
            return (UPPER || (LOWER > (uint64_t) rhs));
        }

        bool operator<(const uint128_t & rhs) const;

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        bool operator<(const T & rhs) const{
            return (!UPPER)?(LOWER < (uint64_t) rhs):false;
        }

        bool operator>=(const uint128_t & rhs) const;

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        bool operator>=(const T & rhs) const{
            return ((*this > rhs) | (*this == rhs));
        }

        bool operator<=(const uint128_t & rhs) const;

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        bool operator<=(const T & rhs) const{
            return ((*this < rhs) | (*this == rhs));
        }

        // Arithmetic Operators
        uint128_t operator+(const uint128_t & rhs) const;

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        uint128_t operator+(const T & rhs) const{
            return uint128_t(UPPER + ((LOWER + (uint64_t) rhs) < LOWER), LOWER + (uint64_t) rhs);
        }

        uint128_t & operator+=(const uint128_t & rhs);

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        uint128_t & operator+=(const T & rhs){
            return *this += uint128_t(rhs);
        }

        uint128_t operator-(const uint128_t & rhs) const;

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        uint128_t operator-(const T & rhs) const{
            return uint128_t((uint64_t) (UPPER - ((LOWER - rhs) > LOWER)), (uint64_t) (LOWER - rhs));
        }

        uint128_t & operator-=(const uint128_t & rhs);

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        uint128_t & operator-=(const T & rhs){
            return *this = *this - uint128_t(rhs);
        }

        uint128_t operator*(const uint128_t & rhs) const;

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        uint128_t operator*(const T & rhs) const{
            return *this * uint128_t(rhs);
        }

        uint128_t & operator*=(const uint128_t & rhs);

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        uint128_t & operator*=(const T & rhs){
            return *this = *this * uint128_t(rhs);
        }

    private:
        std::pair <uint128_t, uint128_t> divmod(const uint128_t & lhs, const uint128_t & rhs) const;
        void ConvertToVector(std::vector<uint8_t> & current, const uint64_t & val) const;
        // do not use prefixes (0x, 0b, etc.)
        // if the input string is too long, only right most characters are read
        void init(const char * s, std::size_t len, uint8_t base);
        void _init_hex(const char *s, std::size_t len);
        void _init_dec(const char *s, std::size_t len);
        void _init_oct(const char *s, std::size_t len);
        void _init_bin(const char *s, std::size_t len);

    public:
        uint128_t operator/(const uint128_t & rhs) const;

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        uint128_t operator/(const T & rhs) const{
            return *this / uint128_t(rhs);
        }

        uint128_t & operator/=(const uint128_t & rhs);

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        uint128_t & operator/=(const T & rhs){
            return *this = *this / uint128_t(rhs);
        }

        uint128_t operator%(const uint128_t & rhs) const;

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        uint128_t operator%(const T & rhs) const{
            return *this % uint128_t(rhs);
        }

        uint128_t & operator%=(const uint128_t & rhs);

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        uint128_t & operator%=(const T & rhs){
            return *this = *this % uint128_t(rhs);
        }

        // Increment Operator
        uint128_t & operator++();
        uint128_t operator++(int);

        // Decrement Operator
        uint128_t & operator--();
        uint128_t operator--(int);

        // Nothing done since promotion doesn't work here
        uint128_t operator+() const;

        // two's complement
        uint128_t operator-() const;

        // Get private values
        const uint64_t & upper() const;
        const uint64_t & lower() const;

        // Get bitsize of value
        uint8_t bits() const;

        // Get string representation of value
        std::string str(uint8_t base = 10, const unsigned int & len = 0) const;
};

// useful values
UINT128_T_EXTERN extern const uint128_t uint128_0;
UINT128_T_EXTERN extern const uint128_t uint128_1;

// lhs type T as first arguemnt
// If the output is not a bool, casts to type T

// Bitwise Operators
template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
uint128_t operator&(const T & lhs, const uint128_t & rhs){
    return rhs & lhs;
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
T & operator&=(T & lhs, const uint128_t & rhs){
    return lhs = static_cast <T> (rhs & lhs);
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
uint128_t operator|(const T & lhs, const uint128_t & rhs){
    return rhs | lhs;
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
T & operator|=(T & lhs, const uint128_t & rhs){
    return lhs = static_cast <T> (rhs | lhs);
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
uint128_t operator^(const T & lhs, const uint128_t & rhs){
    return rhs ^ lhs;
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
T & operator^=(T & lhs, const uint128_t & rhs){
    return lhs = static_cast <T> (rhs ^ lhs);
}

// Bitshift operators
UINT128_T_EXTERN uint128_t operator<<(const bool     & lhs, const uint128_t & rhs);
UINT128_T_EXTERN uint128_t operator<<(const uint8_t  & lhs, const uint128_t & rhs);
UINT128_T_EXTERN uint128_t operator<<(const uint16_t & lhs, const uint128_t & rhs);
UINT128_T_EXTERN uint128_t operator<<(const uint32_t & lhs, const uint128_t & rhs);
UINT128_T_EXTERN uint128_t operator<<(const uint64_t & lhs, const uint128_t & rhs);
UINT128_T_EXTERN uint128_t operator<<(const int8_t   & lhs, const uint128_t & rhs);
UINT128_T_EXTERN uint128_t operator<<(const int16_t  & lhs, const uint128_t & rhs);
UINT128_T_EXTERN uint128_t operator<<(const int32_t  & lhs, const uint128_t & rhs);
UINT128_T_EXTERN uint128_t operator<<(const int64_t  & lhs, const uint128_t & rhs);

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
T & operator<<=(T & lhs, const uint128_t & rhs){
    return lhs = static_cast <T> (uint128_t(lhs) << rhs);
}

UINT128_T_EXTERN uint128_t operator>>(const bool     & lhs, const uint128_t & rhs);
UINT128_T_EXTERN uint128_t operator>>(const uint8_t  & lhs, const uint128_t & rhs);
UINT128_T_EXTERN uint128_t operator>>(const uint16_t & lhs, const uint128_t & rhs);
UINT128_T_EXTERN uint128_t operator>>(const uint32_t & lhs, const uint128_t & rhs);
UINT128_T_EXTERN uint128_t operator>>(const uint64_t & lhs, const uint128_t & rhs);
UINT128_T_EXTERN uint128_t operator>>(const int8_t   & lhs, const uint128_t & rhs);
UINT128_T_EXTERN uint128_t operator>>(const int16_t  & lhs, const uint128_t & rhs);
UINT128_T_EXTERN uint128_t operator>>(const int32_t  & lhs, const uint128_t & rhs);
UINT128_T_EXTERN uint128_t operator>>(const int64_t  & lhs, const uint128_t & rhs);

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
T & operator>>=(T & lhs, const uint128_t & rhs){
    return lhs = static_cast <T> (uint128_t(lhs) >> rhs);
}

// Comparison Operators
template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
bool operator==(const T & lhs, const uint128_t & rhs){
    return (!rhs.upper() && ((uint64_t) lhs == rhs.lower()));
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
bool operator!=(const T & lhs, const uint128_t & rhs){
    return (rhs.upper() | ((uint64_t) lhs != rhs.lower()));
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
bool operator>(const T & lhs, const uint128_t & rhs){
    return (!rhs.upper()) && ((uint64_t) lhs > rhs.lower());
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
bool operator<(const T & lhs, const uint128_t & rhs){
    if (rhs.upper()){
        return true;
    }
    return ((uint64_t) lhs < rhs.lower());
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
bool operator>=(const T & lhs, const uint128_t & rhs){
    if (rhs.upper()){
        return false;
    }
    return ((uint64_t) lhs >= rhs.lower());
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
bool operator<=(const T & lhs, const uint128_t & rhs){
    if (rhs.upper()){
        return true;
    }
    return ((uint64_t) lhs <= rhs.lower());
}

// Arithmetic Operators
template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
uint128_t operator+(const T & lhs, const uint128_t & rhs){
    return rhs + lhs;
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
T & operator+=(T & lhs, const uint128_t & rhs){
    return lhs = static_cast <T> (rhs + lhs);
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
uint128_t operator-(const T & lhs, const uint128_t & rhs){
    return -(rhs - lhs);
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
T & operator-=(T & lhs, const uint128_t & rhs){
    return lhs = static_cast <T> (-(rhs - lhs));
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
uint128_t operator*(const T & lhs, const uint128_t & rhs){
    return rhs * lhs;
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
T & operator*=(T & lhs, const uint128_t & rhs){
    return lhs = static_cast <T> (rhs * lhs);
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
uint128_t operator/(const T & lhs, const uint128_t & rhs){
    return uint128_t(lhs) / rhs;
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
T & operator/=(T & lhs, const uint128_t & rhs){
    return lhs = static_cast <T> (uint128_t(lhs) / rhs);
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
uint128_t operator%(const T & lhs, const uint128_t & rhs){
    return uint128_t(lhs) % rhs;
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
T & operator%=(T & lhs, const uint128_t & rhs){
    return lhs = static_cast <T> (uint128_t(lhs) % rhs);
}

// IO Operator
UINT128_T_EXTERN std::ostream & operator<<(std::ostream & stream, const uint128_t & rhs);

namespace std {
    // numeric limits for uint128 type
    template<>
    class numeric_limits<uint128_t>
    {
    public:
        static constexpr bool is_specialized = true;
        static constexpr bool is_signed = false;
        static constexpr bool is_integer = true;
        static constexpr bool is_exact = true;
        static constexpr int  digits = 128;
        static constexpr int  digits10 = 38;
        static constexpr int  max_digits10 = digits10;
        static constexpr int  radix = 2;
        static constexpr int  min_exponent = 0;
        static constexpr int  min_exponent10 = 0;
        static constexpr int  max_exponent = 0;
        static constexpr int  max_exponent10 = 0;

        static constexpr bool has_infinity = false;
        static constexpr bool has_quiet_NaN = false;
        static constexpr bool has_signaling_NaN = false;
        static constexpr float_denorm_style has_denorm = denorm_absent;
        static constexpr bool has_denorm_loss = false;

        static constexpr bool is_iec559 = false;
        static constexpr bool is_bounded = true;
        static constexpr bool is_modulo = false;

        static  constexpr bool traps = numeric_limits<uint64_t>::traps;
        static  constexpr bool tinyness_before = false;
        static  constexpr float_round_style round_style = round_toward_zero;


        static constexpr uint128_t(min)() noexcept          { return uint128_t(0, 0); }
        static constexpr uint128_t(max)() noexcept          { return uint128_t(UINT64_MAX, UINT64_MAX); }
        static constexpr uint128_t lowest() noexcept        { return uint128_t(0, 0); }
        static constexpr uint128_t epsilon() noexcept       { return uint128_t(0, 0); }
        static constexpr uint128_t round_error() noexcept   { return uint128_t(0, 0); }
        static constexpr uint128_t denorm_min() noexcept    { return uint128_t(0, 0); }
        static constexpr uint128_t infinity() noexcept      { return uint128_t(0, 0); }
        static constexpr uint128_t quiet_NaN() noexcept     { return uint128_t(0, 0); }
        static constexpr uint128_t signaling_NaN() noexcept { return uint128_t(0, 0); }
    };
}

#endif
