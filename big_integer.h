#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H

#include <iosfwd>
#include <vector>
#include "cow_vector/cow_vector.h"

struct big_integer {
    big_integer();

    big_integer(big_integer const &other);
    big_integer(int a);
    big_integer(size_t a);
    explicit big_integer(std::string const &str);
    ~big_integer();

    big_integer& operator=(big_integer const &other);
    big_integer& operator+=(big_integer const &rhs);
    big_integer& operator-=(big_integer const &rhs);
    big_integer& operator*=(big_integer const &rhs);
    big_integer& operator/=(big_integer const &rhs);

    big_integer& operator%=(big_integer const &rhs);
    big_integer& operator&=(big_integer const &rhs);
    big_integer& operator|=(big_integer const &rhs);
    big_integer& operator^=(big_integer const &rhs);
    big_integer& operator<<=(int rhs);
    big_integer& operator>>=(int rhs);

    big_integer operator+() const;
    big_integer operator-() const;
    big_integer operator~() const;
    big_integer& operator++();
    big_integer operator++(int);
    big_integer& operator--();
    big_integer operator--(int);

    friend bool operator==(big_integer const &a, big_integer const &b);
    friend bool operator!=(big_integer const &a, big_integer const &b);
    friend bool operator<(big_integer const &a, big_integer const &b);
    friend bool operator>(big_integer const &a, big_integer const &b);
    friend bool operator<=(big_integer const &a, big_integer const &b);
    friend bool operator>=(big_integer const &a, big_integer const &b);

    friend std::string to_string(big_integer const &a);

public:
    friend big_integer full_division(big_integer a, big_integer b, bool isR);
    friend void swap(big_integer& a, big_integer& b);
    void to_twos_complement();
    void to_direct_code();
    void delete_leading_zeros();

    bool sign_;
    bool isSingleNum_;
    union { size_t value_; cow_vector nums_; };
};

big_integer operator+(big_integer a, big_integer const &b);
big_integer operator-(big_integer a, big_integer const &b);
big_integer operator*(big_integer a, big_integer const &b);
big_integer operator/(big_integer a, big_integer const &b);
big_integer operator%(big_integer a, big_integer const &b);

big_integer operator&(big_integer a, big_integer const &b);
big_integer operator|(big_integer a, big_integer const &b);
big_integer operator^(big_integer a, big_integer const &b);
big_integer operator<<(big_integer a, int b);
big_integer operator>>(big_integer a, int b);

bool operator==(big_integer const &a, big_integer const &b);
bool operator!=(big_integer const &a, big_integer const &b);
bool operator<(big_integer const &a, big_integer const &b);
bool operator>(big_integer const &a, big_integer const &b);
bool operator<=(big_integer const &a, big_integer const &b);
bool operator>=(big_integer const &a, big_integer const &b);

std::string to_string(big_integer const &a);

std::ostream &operator<<(std::ostream &s, big_integer const &a);

#endif // BIG_INTEGER_H
