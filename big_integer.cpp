#include "big_integer.h"
#include <vector>
#include <string>
#include <climits>
#include <algorithm>

using std::vector;

typedef uint32_t uint;
typedef uint64_t ull;

const ull MAX = (ull)UINT32_MAX + 1;

big_integer::big_integer() {
    neg = false;
    set_zero();
}

big_integer::big_integer(int a) {
    neg = a < 0;
    digits = vector <uint> (1);
    digits[0] = (uint)a;
    normalize();
}

big_integer::big_integer(uint a) {
    neg = false;
    digits = vector <uint> (1);
    digits[0] = a;
    normalize();
}

big_integer::big_integer(bool negative, size_t l) {
    neg = negative;
    digits = vector<uint> (l, (uint)(neg ? (MAX - 1) : 0));
}

big_integer::big_integer(big_integer const &other) = default;

big_integer::big_integer(std::string const &str) {
    big_integer res = 0;
    bool negative = str[0] == '-';
    for (size_t i = negative ? 1 : 0; i < str.size(); ++i) {
        res = mul_uint_bigint(res, 10) + (str[i] - '0');
    }
    if (negative) {
        res = -res;
    }
    res.normalize();
    *this = res;
}

big_integer &big_integer::operator=(big_integer const &other) = default;

big_integer big_integer::operator-() const {
    big_integer res = *this;
    res.neg = !neg;
    bool carry = true;
    size_t i = 0;
    for (i = 0; i < len() && carry; ++i) {
        res[i] = -res[i];
        carry = res[i] == 0;
    }
    for (; i < len(); ++i) {
        res[i] = ~res[i];
    }
    if (carry) {
        if (!res.neg)
            res.digits.push_back(1);
        else
            res.neg = false;
    }
    res.normalize();
    return res;
}
big_integer big_integer::operator+() const {
    return *this;
}

big_integer big_integer::operator~() const {
    big_integer res = *this;
    for (size_t i = 0; i < len(); ++i) {
        res.digits[i] = ~digits[i];
    }
    res.neg = !neg;
    res.normalize();
    return res;
}

big_integer& big_integer::operator++() {
    bool carry = true;
    for (size_t i = 0; i < len() && carry; ++i) {
        digits[i]++;
        carry = digits[i] == 0;
    }
    if (carry) {
        if (!neg)
            digits.push_back(1);
        else
            neg = false;
    }
    normalize();
    return *this;
}

big_integer& big_integer::operator--(){
    return *this = *this - 1;
}

big_integer const big_integer::operator++(int) {
    big_integer res = *this;
    ++*this;
    return res;
}

big_integer const big_integer::operator--(int){
    big_integer res = *this;
    *this = *this - 1;
    return res;
}

big_integer abs(const big_integer &a) {
    return a.neg ? -a : a;
}

bool operator==(big_integer const &a, big_integer const &b) {
    if (a.neg != b.neg || a.len() != b.len()) {
        return false;
    }
    for (size_t i = 0; i < a.len(); ++i) {
        if (a[i] != b[i]) {
             return false;
        }
    }
    return true;
}

bool operator!=(big_integer const &a, big_integer const &b) {
    return !(a == b);
}

bool operator<(big_integer const &a, big_integer const &b) {
    if (a.neg != b.neg) {
        return a.neg;
    }
    if (a.len() != b.len()) {
        return (b.len() > a.len()) != a.neg;
    }

    for (size_t i = a.len() - 1; i != (size_t)-1; --i) {
        if (a[i] > b[i]) {
            return false;
        } else if (b[i] > a[i]) {
            return true;
        }
    }
    return false;
}

bool operator>(big_integer const &a, big_integer const &b) {
    return b < a;
}

bool operator<=(big_integer const &a, big_integer const &b) {
    return a < b || a == b;
}

bool operator>=(big_integer const &a, big_integer const &b) {
    return b < a || b == a;
}

big_integer operator+(big_integer first, big_integer const &second) {
    first.change_len(second.len());
    uint zero = (first.neg ? uint(MAX - 1) : 0);
    uint zero_oth = (second.neg ? uint(MAX - 1) : 0);
    uint carry = 0;
    for (size_t i = 0; i < second.len(); ++i) {
        ull cur = (ull)carry + first[i] + second[i];
        first[i] = (uint)cur;
        carry = uint(cur >> 32);
    }
    for (size_t i = second.len(); i < first.len(); ++i) {
        ull cur = (ull)carry + first[i] + zero_oth;
        first[i] = (uint)cur;
        carry = uint(cur >> 32);
    }
    first.neg = ((zero + zero_oth + carry) >> 31) > 0;
    if (zero + zero_oth + carry != (first.neg ? (MAX - 1) : 0)) {
        first.digits.push_back((uint)(zero + zero_oth + carry));
    }
    first.normalize();
    return first;
}

big_integer operator-(big_integer const &a, big_integer const &b) {
    return a + -b;
}

big_integer mul_uint_bigint(big_integer const &a, uint b) {
    if (b == 0) {
        return big_integer(0);
    }
    if (b == 1) {
        return a;
    }
    big_integer res(a.neg, a.len() + 1);
    ull carry = 0;

    for (size_t i = 0; i < a.len(); ++i) {
        res[i] = (uint) ((carry + (ull)a[i] * b) & 0xffffffff);
        carry = (carry + (ull)a[i] * b) >> 32;
    }
    res[res.len() - 1] = (uint)carry;
    res.normalize();
    return res;
}

big_integer operator*(big_integer const &a, big_integer const &b) {
    bool negative = a.neg ^ b.neg;
    big_integer first = abs(a), second = abs(b);
    big_integer res = 0;
    res.change_len(first.len() + second.len() + 2);

    for (size_t i = 0; i < first.len(); ++i) {
        uint carry = 0;
        for (size_t j = 0; j < second.len(); ++j) {
            ull cur = (ull)first[i] * second[j] + carry + res[i + j];
            res[i + j] = (uint)cur;
            carry = (uint)(cur >> 32);
        }
        for (size_t j = second.len(); i + j < res.len() && carry; ++j) {
            ull cur = (ull)carry + res[i + j];
            res[i + j] = (uint)cur;
            carry = (uint)(cur >> 32);
        }
    }

    res.normalize();
    return negative ? -res : res;
}

void shl_sub(big_integer &a, big_integer const &b, size_t sh) {
    ull cur;
    bool carry = false;
    a.change_len(b.len() + sh);
    for (size_t i = 0; i < b.len(); ++i) {
        cur = (ull)a[i + sh] - b[i] - carry;
        carry = (cur >> 63) > 0;
        a[i + sh] = (uint)cur;
    }
    for (size_t i = b.len() + sh; i < a.len() && carry; ++i) {
        carry = (a[i] == 0);
        a[i]--;
    }
    a.neg = carry;
}

big_integer operator/(big_integer const &first, big_integer const &second) {
    if (first < 0 || second < 0) {
        return (first < 0 ? -first : first) / (second < 0 ? -second : second) * (first.neg == second.neg ? 1 : -1);
    }
    big_integer a = first, b = second;
    if (a < b || a == 0) {
        return 0;
    }
    if (a == b) {
        return 1;
    }
    a = a << __builtin_clz(b.digits.back());
    b = b << __builtin_clz(b.digits.back());
    size_t n = b.len(), m = a.len() - b.len();
    big_integer q = 0;
    q.change_len(m + 1);
    if (a >= (b << (m * 32))) {
        q[m] = 1;
        shl_sub(a, b, m);
    }

    for (size_t i = m - 1; i != (size_t)-1; --i) {
        ull q1 = ((ull(a.safe_get(n + i)) << 32) + a.safe_get(n + i - 1)) / b[n - 1];
        q[i] = (uint)std::min(MAX - 1, q1);
        shl_sub(a, mul_uint_bigint(b, q[i]), i);
        while (a.neg) {
            q[i] -= 1;
            a += (b << (i * 32));
        }
    }
    q.normalize();
    return q;
}

big_integer operator%(big_integer const &a, big_integer const &b) {
    return a - (a / b) * b;
}

big_integer& big_integer::operator+=(big_integer const &other) {
    return *this = *this + other;
}
big_integer& big_integer::operator-=(big_integer const &other) {
    return *this = *this - other;
}
big_integer& big_integer::operator*=(big_integer const &other) {
    return *this = *this * other;
}
big_integer& big_integer::operator/=(big_integer const &other) {
    return *this = *this / other;
}
big_integer& big_integer::operator%=(big_integer const &other) {
    return *this = *this % other;
}

big_integer operator&(big_integer first, big_integer const &second) {
    first &= second;
    return first;
}

big_integer operator|(big_integer first, big_integer const &second) {
    first |= second;
    return first;
}

big_integer operator^(big_integer first, big_integer const &second) {
    first ^= second;
    return first;
}

big_integer operator<<(big_integer const &a, uint b) {
    big_integer res = a;
    size_t bc = b >> 5, br = b & 31;
    res.change_len(a.len() + bc + 1);
    if (bc > 0) {
        for (size_t i = res.len() - 1; i >= bc; --i)
            res[i] = res[i - bc];
        for (size_t i = bc - 1; i != (size_t)-1; --i) {
            res[i] = 0;
        }
    } else {
        for (size_t i = res.len() - 1; i != (size_t)-1; --i)
            res[i] = res[i - bc];
    }
    if (br != 0) {
        for (size_t i = res.len() - 1; i > 0; --i) {
            res[i] <<= br;
            res[i] |= (res[i - 1] >> (32 - br));
        }
        res[0] <<= br;
    }
    res.normalize();
    return res;
}

big_integer operator>>(big_integer const &a, uint b) {
    big_integer res = a;
    uint zero = (a.neg ? (uint)(MAX - 1) : 0);
    size_t bc = b >> 5, br = b & 31;
    for (size_t i = 0; i < res.len(); ++i) {
        res[i] = (i + bc < res.len() ? res[i + bc] : zero);
    }
    if (br != 0) {
        for (size_t i = 0; i < res.len(); ++i) {
            res[i] >>= br;
            res[i] |= (i + 1 == res.len() ? zero : res[i + 1]) << (32 - br);
        }
    }
    res.normalize();
    return res;
}

big_integer& big_integer::operator&=(big_integer const &other) {
    change_len(other.len());
    uint zero = (other.neg ? uint(MAX - 1) : 0);
    for (size_t i = 0; i < other.len(); ++i) {
        digits[i] &= other[i];
    }
    for (size_t i = other.len(); i < len(); ++i) {
        digits[i] &= zero;
    }
    neg &= other.neg;
    return *this;
}
big_integer& big_integer::operator|=(big_integer const &other) {
    change_len(other.len());
    uint zero = (other.neg ? uint(MAX - 1) : 0);
    for (size_t i = 0; i < other.len(); ++i) {
        digits[i] |= other[i];
    }
    for (size_t i = other.len(); i < len(); ++i) {
        digits[i] |= zero;
    }
    neg |= other.neg;
    return *this;
}
big_integer& big_integer::operator^=(big_integer const &other) {
    change_len(other.len());
    uint zero = (other.neg ? uint(MAX - 1) : 0);
    for (size_t i = 0; i < other.len(); ++i) {
        digits[i] ^= other[i];
    }
    for (size_t i = other.len(); i < len(); ++i) {
        digits[i] ^= zero;
    }
    neg ^= other.neg;
    return *this;
}
big_integer& big_integer::operator<<=(uint other) {
    return *this = *this << other;
}
big_integer& big_integer::operator>>=(uint other) {
    return *this = *this >> other;
}

std::string to_string(big_integer const &a) {
    std::string res;
    big_integer cur = abs(a);
    if (cur == 0) {
        return "0";
    }
    while (cur > 0) {
        res += (char)((cur % 10)[0] + '0');
        cur /= 10;
    }
    res += (a.neg ? "-" : "");
    std::reverse(res.begin(), res.end());
    return res;
}

std::ostream& operator<<(std::ostream& s, big_integer const& a) {
   return s << to_string(a);
}

void swap(big_integer &a, big_integer &b) {
    std::swap(a.neg, b.neg);
    std::swap(a.digits, b.digits);
}

size_t big_integer::len() const {
    return digits.size();
}

void big_integer::normalize() {
    while (!digits.empty() && ((digits.back() == 0 && !neg) || (digits.back() == (MAX - 1) && neg))) {
        digits.pop_back();
    }
    if (digits.empty()) {
        set_zero();
    }
}

void big_integer::set_zero() {
    digits.resize(1);
    digits[0] = neg ? (uint)(MAX - 1) : 0;
}

void big_integer::change_len(size_t nlen) {
    if (nlen <= len()) return;
    digits.resize(nlen, (uint)(neg ? (MAX - 1) : 0));
}

uint big_integer::operator[](size_t position) const {
    return digits[position];
}

uint& big_integer::operator[](size_t position) {
    return digits[position];
}

uint big_integer::safe_get(size_t i) const {
    return i < len() ? digits[i] : (neg ? (uint)(MAX - 1) : 0);
}
