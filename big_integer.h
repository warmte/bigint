#ifndef BIGINT_BIG_INTEGER_H
#define BIGINT_BIG_INTEGER_H

#include <vector>
#include <string>

using std::vector;

struct big_integer {
    typedef unsigned int uint;
    typedef unsigned long long ull;

    big_integer();
    big_integer(int a);
    big_integer(uint a);
    big_integer (bool negative, size_t l);
    big_integer(big_integer const& other);
    explicit big_integer(std::string const& str);

    big_integer& operator=(big_integer const& other);
    big_integer operator-() const;
    big_integer operator+() const;
    big_integer operator~() const;

    big_integer& operator++();
    big_integer const operator++(int);

    big_integer& operator--();
    big_integer const operator--(int);
    friend big_integer abs(const big_integer &a);

    friend bool operator==(big_integer const& a, big_integer const& b);
    friend bool operator!=(big_integer const& a, big_integer const& b);
    friend bool operator<(big_integer const& a, big_integer const& b);
    friend bool operator>(big_integer const& a, big_integer const& b);
    friend bool operator<=(big_integer const& a, big_integer const& b);
    friend bool operator>=(big_integer const& a, big_integer const& b);

    friend big_integer operator+(big_integer a, big_integer const& b);
    friend big_integer operator-(big_integer const &a, big_integer const& b);
    friend big_integer operator*(big_integer const &a, big_integer const& b);
    friend big_integer operator/(big_integer a, big_integer b);
    friend big_integer operator%(big_integer const &a, big_integer const& b);

    big_integer& operator+=(big_integer const &other);
    big_integer& operator-=(big_integer const &other);
    big_integer& operator*=(big_integer const &other);
    big_integer& operator/=(big_integer const &other);
    big_integer& operator%=(big_integer const &other);

    friend big_integer operator&(big_integer a, big_integer const& b);
    friend big_integer operator|(big_integer a, big_integer const& b);
    friend big_integer operator^(big_integer a, big_integer const& b);
    friend big_integer operator<<(big_integer const &a, uint b);
    friend big_integer operator>>(big_integer const &a, uint b);

    big_integer& operator&=(big_integer const &other);
    big_integer& operator|=(big_integer const &other);
    big_integer& operator^=(big_integer const &other);
    big_integer& operator<<=(uint other);
    big_integer& operator>>=(uint other);

    friend std::string to_string(big_integer const& a);
    friend void swap(big_integer &a, big_integer &b);
private:
    bool neg;
    vector<uint> digits;

    size_t len() const;
    void normalize();
    void change_len(size_t nlen);
    uint operator[](size_t position) const;
    uint& operator[](size_t position);
    void set_zero();
    friend void shl_sub(big_integer &a, big_integer const &b, size_t sh);
    friend bool shl_more(big_integer const &a, big_integer const &b, size_t sh);

    friend big_integer mul_uint_bigint(big_integer const &num, uint b);
    uint safe_get(size_t i, uint zero) const;
};

std::ostream& operator<<(std::ostream& s, big_integer const& a);



#endif //BIGINT_BIG_INTEGER_H
