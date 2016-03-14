#include "big_integer.h"

#include <string>
#include <iostream>
#include <assert.h>

const __uint128_t BASE = static_cast<__uint128_t>(std::numeric_limits<size_t>::max()) + static_cast<__uint128_t>(1);

big_integer::big_integer() : sign_(false), value_(0), isSingleNum_(true) { }

big_integer::big_integer(big_integer const &other) : big_integer() {
    *this = other;
}

big_integer::big_integer(int a) : sign_(a < 0), value_(abs(a)), isSingleNum_(true) { }

big_integer::big_integer(size_t a) : sign_(false), value_(a), isSingleNum_(true) { }

big_integer::big_integer(const std::string& str) : big_integer() {
    for (int i = (str[0] == '-' ? 1 : 0); i < str.length(); i++) {
        *this = *this * 10 + (str[i] - '0');
    }

    if (*this != 0) this->sign_ = str[0] == '-';
}

big_integer::~big_integer() {
    if (!this->isSingleNum_) this->nums_.~cow_vector();
}

big_integer &big_integer::operator=(big_integer const &other) {
    this->sign_ = other.sign_;

    if (this->isSingleNum_ && !other.isSingleNum_) {
        new(&nums_) cow_vector();
        this->isSingleNum_ = false;
    }
    else if (!this->isSingleNum_ && other.isSingleNum_) {
        this->nums_.~cow_vector();
        this->isSingleNum_ = true;
    }

    this->isSingleNum_ = other.isSingleNum_;

    if (this->isSingleNum_) this->value_ = other.value_;
    else this->nums_ = other.nums_;

    return *this;
}

big_integer &big_integer::operator+=(big_integer const &rhs) {
    if (this->sign_ != rhs.sign_) {
        if (rhs.sign_ == 1) *this -= -rhs;
        else *this = rhs - -(*this);
        return *this;
    }

    if (this->isSingleNum_ && rhs.isSingleNum_) {
        __uint128_t tmp = this->value_ + static_cast<__uint128_t>(rhs.value_);
        if (tmp < BASE) this->value_ = static_cast<size_t>(tmp);
        else {
            new(&nums_) cow_vector(2);
            this->isSingleNum_ = false;
            this->nums_[0] = static_cast<size_t>(tmp);
            this->nums_[1] = static_cast<size_t>(tmp / BASE);
        }
        return *this;
    }

    if (rhs.isSingleNum_) {
        size_t carryBit = 0;
        for (int i = 0; i < this->nums_.size() || carryBit; i++) {
            if (i == this->nums_.size()) this->nums_.push_back(0);

            __uint128_t tmp = static_cast<__uint128_t>(this->nums_[i]) + carryBit + (i == 0 ? rhs.value_ : 0);
            this->nums_[i] = static_cast<size_t>(tmp);
            carryBit = tmp / BASE;
        }

        return *this;
    }

    if (this->isSingleNum_) {
        size_t tmpVal = this->value_;
        new(&nums_) cow_vector(1, tmpVal);
        this->isSingleNum_ = false;
    }

    size_t carryBit = 0;
    for (int i = 0; i < std::max(this->nums_.size(), rhs.nums_.size()) || carryBit; i++) {
        if (i == this->nums_.size())
            this->nums_.push_back(0);

        __uint128_t tmp = static_cast<__uint128_t>(this->nums_[i]) +
                          static_cast<__uint128_t>(i < rhs.nums_.size() ? rhs.nums_[i] : 0) +
                          static_cast<__uint128_t>(carryBit);
        this->nums_[i] = static_cast<size_t>(tmp);
        carryBit = tmp / BASE;
    }

    return *this;
}

big_integer &big_integer::operator-=(big_integer const &rhs) {
    if (this->sign_ != rhs.sign_) {
        *this += -rhs;
        return *this;
    }

    if (this->sign_ == 1) {
        *this = -rhs - -*this;
        return *this;
    }

    if (*this < rhs) {
        *this = rhs - *this;
        this->sign_ = 1;
        return *this;
    }

    if (this->isSingleNum_ && rhs.isSingleNum_) {
        this->value_ -= rhs.value_;
        return *this;
    }

    if (rhs.isSingleNum_) {
        size_t borrowBit = this->nums_[0] < rhs.value_;
        if (borrowBit) this->nums_[0] = static_cast<size_t>(BASE + this->nums_[0] - rhs.value_);
        else this->nums_[0] -= rhs.value_;

        int i = 1;
        while (!this->nums_[i]) i++;
        this->nums_[i]--;

        return *this;
    }

    size_t borrowBit = 0;
    for (int i = 0; i < this->nums_.size(); i++) {
        __uint128_t firstOp = static_cast<__uint128_t>(this->nums_[i]);
        __uint128_t secondOp = static_cast<__uint128_t>(i < rhs.nums_.size() ? rhs.nums_[i] : 0) +
                               static_cast<__uint128_t>(borrowBit);
        if (secondOp > firstOp) {
            borrowBit = 1;
            firstOp += BASE;
        }
        else borrowBit = 0;

        firstOp -= secondOp;
        this->nums_[i] = static_cast<size_t>(firstOp);
    }

    delete_leading_zeros();

    return *this;
}

big_integer &big_integer::operator*=(big_integer const &rhs) {
    this->sign_ = this->sign_ != rhs.sign_;

    if (this->isSingleNum_ && rhs.isSingleNum_) {
        __uint128_t tmp = this->value_ * static_cast<__uint128_t>(rhs.value_);
        if (tmp < BASE) this->value_ = static_cast<size_t>(tmp);
        else {
            new(&nums_) cow_vector(2);
            this->isSingleNum_ = false;
            this->nums_[0] = static_cast<size_t>(tmp);
            this->nums_[1] = static_cast<size_t>(tmp / BASE);
        }
        return *this;
    }

    if (rhs.isSingleNum_) {
        size_t carryBit = 0;

        for (int i = 0; i < this->nums_.size() || carryBit; i++) {
            if (i == this->nums_.size()) this->nums_.push_back(0);

            __uint128_t tmp = static_cast<__uint128_t>(this->nums_[i]) * rhs.value_ + static_cast<__uint128_t>(carryBit);
            this->nums_[i] = static_cast<size_t>(tmp);
            carryBit = tmp / BASE;
        }

        delete_leading_zeros();

        return *this;
    }

    if (this->isSingleNum_) {
        size_t tmpVal = this->value_;
        new(&nums_) cow_vector(1, tmpVal);
        this->isSingleNum_ = false;
    }

    cow_vector ans(this->nums_.size() + rhs.nums_.size() - 1, 0);
    __uint128_t carryBits = 0;

    for (int j = 0; j < rhs.nums_.size(); j++) {
        for (int i = 0; i < this->nums_.size(); i++) {
            if (ans.size() <= i + j) ans.push_back(0);
            __uint128_t tmp = static_cast<__uint128_t>(ans[i + j]) +
                              static_cast<__uint128_t>(this->nums_[i]) * static_cast<__uint128_t>(rhs.nums_[j]) +
                              carryBits;

            ans[i + j] = tmp & (BASE - 1);
            carryBits = tmp >> sizeof(size_t) * 8;
        }

        //if (carryBits != 0) ans.push_back(carryBits);
        if (carryBits != 0) {
            if (ans.size() <= j + this->nums_.size()) ans.push_back(carryBits);
            else ans[j + this->nums_.size()] = carryBits;
        }
        carryBits = 0;
    }

    this->nums_ = ans;

    delete_leading_zeros();
    return *this;
}

big_integer full_division(big_integer a, big_integer b, bool isR) {
    bool newSign = isR ? a.sign_ : (a.sign_ != b.sign_);

    a.sign_ = 0;
    b.sign_ = 0;

    if (a < b) {
        if (isR) {
            if (a != 0) a.sign_ = newSign;
            return a;
        }
        else return 0;
    }

    if (a.isSingleNum_ && b.isSingleNum_) {
        big_integer ans;
        if(isR) ans = a.value_ % b.value_;
        else ans = a.value_ / b.value_;
        if (ans != 0) ans.sign_ = newSign;

        return ans;
    }

    if (b.isSingleNum_) {
        size_t carryBits = 0;
        for (int i = a.nums_.size() - 1; i >= 0; i--) {
            __uint128_t current = a.nums_[i] + carryBits * BASE;
            a.nums_[i] = static_cast<size_t>(current / b.value_);
            carryBits = static_cast<size_t>(current % b.value_);
        }

        big_integer ans;

        if (isR) ans = carryBits;
        else ans = a;
        ans.delete_leading_zeros();
        ans.sign_ = (ans == 0) ? 0 : newSign;

        return ans;
        /*
       size_t tmpVal = b.value_;
       new(&b.nums_) cow_vector(1, tmpVal);
       b.isSingleNum_ = false;
       */
    }

    //normalize b
    int k = 0;
    size_t bn1 = b.nums_[b.nums_.size() - 1];
    while (bn1 < static_cast<size_t>(BASE >> 1)) {
        bn1 <<= 1;
        k++;
    }
    b <<= k;
    a <<= k;

    big_integer ans;
    int m = a.nums_.size() - b.nums_.size();
    new(&(ans.nums_)) cow_vector(m + 1);
    ans.isSingleNum_ = false;

    //tmp = ß^m * rhs
    big_integer B_m = 1;
    B_m <<= sizeof(size_t) * 8 * m;
    B_m *= b;

    int n = b.nums_.size();

    if (a >= B_m) {
        ans.nums_[m] = 1;
        a -= B_m;
    }
    else ans.nums_[m] = 0;

    for (int j = m - 1; j >= 0; j--) {
        __uint128_t q_tmp = 0; //= (a.nums_[n + j] * BASE + a.nums_[n + j - 1]) / b.nums_[n - 1];

        if (a.isSingleNum_) {
            if (n + j == 0) q_tmp += a.value_ * BASE;
            if (n + j - 1 == 0) q_tmp += a.value_;
        }
        else q_tmp = a.nums_[n + j] * BASE + a.nums_[n + j - 1];
        q_tmp /= static_cast<__uint128_t>(b.nums_[n - 1]);

        ans.nums_[j] = std::min(static_cast<size_t>(q_tmp), static_cast<size_t>(BASE - 1));

        int k = B_m.nums_.size();
        B_m = 1;
        B_m <<= sizeof(size_t) * 8 * j;
        big_integer tmp1 = ans.nums_[j] * B_m;
        big_integer tmp2 = tmp1 * b;
        a -= tmp2;

        big_integer base_mul_b = B_m * b;
        while (a < 0) {
            ans.nums_[j]--;
            a += base_mul_b;
        }
    }

    ans.delete_leading_zeros();

    if (isR) {
        a >>= k;
        ans = a;
    }
    ans.sign_ = (ans == 0) ? 0 : ans.sign_ = newSign;

    return ans;
}

big_integer &big_integer::operator/=(big_integer const &rhs) {
    *this = full_division(*this, rhs, false);
    return *this;
}

big_integer &big_integer::operator%=(big_integer const &rhs) {
    *this = full_division(*this, rhs, true);
    return *this;
}

big_integer &big_integer::operator&=(big_integer const &rhs) {
    to_twos_complement();
    big_integer tmp = rhs;
    tmp.to_twos_complement();

    this->sign_ = (this->sign_ & tmp.sign_);

    if (this->isSingleNum_ && tmp.isSingleNum_) {
        this->value_ &= tmp.value_;
    }
    else if (this->isSingleNum_ || tmp.isSingleNum_) {
        size_t newVal = this->isSingleNum_ ? this->value_ & tmp.nums_[0] : this->nums_[0] & tmp.value_;
        new(&nums_) cow_vector(std::max(this->nums_.size(), tmp.nums_.size()), 0);
        this->isSingleNum_ = false;
        this->nums_[0] = newVal;
    }
    else {
        this->nums_.resize(std::max(this->nums_.size(), tmp.nums_.size()));
        for (int i = 0; i < std::max(this->nums_.size(), tmp.nums_.size()); i++) {
            this->nums_[i] = i < tmp.nums_.size() ? this->nums_[i] & tmp.nums_[i] : 0;
        }
    }

    to_direct_code();
    delete_leading_zeros();

    return *this;
}

big_integer &big_integer::operator|=(big_integer const &rhs) {
    to_twos_complement();

    big_integer tmp = rhs;
    tmp.to_twos_complement();

    this->sign_ = (this->sign_ | tmp.sign_);

    if (this->isSingleNum_ && tmp.isSingleNum_) this->value_ |= tmp.value_;
    else if (tmp.isSingleNum_) this->nums_[0] |= tmp.value_;
    else {
        if (this->isSingleNum_) {
            size_t tmpVal = this->value_;
            new(&nums_) cow_vector(1, tmpVal);
            this->isSingleNum_ = false;
        }

        for (int i = 0; i < std::min(this->nums_.size(), tmp.nums_.size()); i++)
            this->nums_[i] |= tmp.nums_[i];

        if (tmp.nums_.size() > this->nums_.size()) {
            this->nums_.insert(this->nums_.end(), tmp.nums_.begin() + this->nums_.size(), tmp.nums_.end());
        }
    }

    to_direct_code();
    delete_leading_zeros();

    return *this;
}

big_integer &big_integer::operator^=(big_integer const &rhs) {
    to_twos_complement();

    big_integer tmp = rhs;
    tmp.to_twos_complement();

    this->sign_ = (this->sign_ ^ tmp.sign_);

    if (this->isSingleNum_ && tmp.isSingleNum_) this->value_ ^= tmp.value_;
    else if (tmp.isSingleNum_) this->nums_[0] ^= tmp.value_;
    else {
        if (this->isSingleNum_) {
            size_t tmpVal = this->value_;
            new(&nums_) cow_vector(1, tmpVal);
            this->isSingleNum_ = false;
        }

        for (int i = 0; i < std::min(this->nums_.size(), tmp.nums_.size()); i++)
            this->nums_[i] ^= tmp.nums_[i];

        if (tmp.nums_.size() > this->nums_.size()) {
            this->nums_.insert(this->nums_.end(), tmp.nums_.begin() + this->nums_.size(), tmp.nums_.end());
        }
    }

    to_direct_code();
    delete_leading_zeros();

    return *this;
}

big_integer &big_integer::operator<<=(int rhs) {
    int digits = std::numeric_limits<size_t>::digits;  //how many bits in our size_t
    int numbersToInsert = rhs / digits;                //how many zeros we must insert
    int shlBits = rhs % digits;                        //how many bits we will shift besides those zeros we've inserted

    if (shlBits > 0) {
        if (this->isSingleNum_) { //if our number fit one num
            if (this->value_ >> (digits - shlBits) == 0) { //if it fit single num after "<<" just shift it
                this->value_ <<= shlBits;
            }
            else { //else convert it to vector and process later as a vector
                size_t tmpVal = this->value_;
                new(&nums_) cow_vector(1, tmpVal);
                this->isSingleNum_ = false;
            }
        }

        if (!this->isSingleNum_) { //if our number is presented by vector
            bool wasFirstNumHandled = false;
            if (this->nums_.back() >> (digits - shlBits) != 0) {
                wasFirstNumHandled = true;

                //000...0### where ### = last bits of nums_.back()
                nums_.push_back(nums_.back() >> (digits - shlBits));
            }

            for (int i = this->nums_.size() - 1 - wasFirstNumHandled; i >= 0; i--) {
                this->nums_[i] <<= shlBits;
                if (i != 0) this->nums_[i] |= this->nums_[i - 1] >> (digits - shlBits);
            }
        }
    }

    //insert zeros after all operations to reduce calculations
    if (numbersToInsert > 0) {
        if (this->isSingleNum_) {
            size_t tmpVal = this->value_;
            new(&nums_) cow_vector(numbersToInsert + 1, 0);
            this->isSingleNum_ = false;
            this->nums_.back() = tmpVal;
        }
        else this->nums_.insert(this->nums_.begin(), numbersToInsert, 0);
    }

    return *this;
}

big_integer &big_integer::operator>>=(int rhs) {
    int digits = std::numeric_limits<size_t>::digits;
    int numbersToDelele = rhs / digits;
    int shrBits = rhs % digits;

    to_twos_complement();

    //delete numbers before all operations to reduce calculations
    if (numbersToDelele > 0) {
        if (numbersToDelele >= this->nums_.size() || (numbersToDelele && this->isSingleNum_)) {
            *this = this->sign_ ? -1 : 0;
            return *this;
        }
        else {
            this->nums_.erase(this->nums_.begin(), this->nums_.begin() + numbersToDelele);
            if (this->nums_.size() == 1) {
                size_t tmpVal = nums_[0];
                this->nums_.~cow_vector();
                this->value_ = tmpVal;
                this->isSingleNum_ = true;
            }
        }
    }

    if (this->isSingleNum_) {
        this->value_ >>= shrBits;
        if (this->sign_) this->value_ |= ~((1 << (digits - shrBits)) - 1);
    }

    if (!this->isSingleNum_) {
        for (int i = 0; i < this->nums_.size() - 1; i++) {
            this->nums_[i] >>= shrBits;
            this->nums_[i] |= this->nums_[i + 1] << (digits - shrBits);
        }

        this->nums_.back() >>= shrBits;
        if (this->sign_) this->nums_.back() |= ~((1 << (digits - shrBits)) - 1);
    }

    to_direct_code();
    delete_leading_zeros();

    return *this;
}

big_integer big_integer::operator+() const {
    return *this;
}

big_integer big_integer::operator-() const {
    big_integer r = *this;
    if (r != 0) r.sign_ = !r.sign_;
    return r;
}

big_integer big_integer::operator~() const {
    big_integer r = *this;
    r.to_twos_complement();

    if (this->isSingleNum_) r.value_ = ~r.value_;
    else {
        for (int i = 0; i < r.nums_.size(); i++)
            r.nums_[i] = ~r.nums_[i];
    }

    r.sign_ = !r.sign_;
    r.to_direct_code();
    return r;
}

big_integer &big_integer::operator++() {
    *this += big_integer(1);
    return *this;
}

big_integer big_integer::operator++(int) {
    big_integer r = *this;
    ++*this;
    return r;
}

big_integer &big_integer::operator--() {
    *this -= big_integer(1);
    return *this;
}

big_integer big_integer::operator--(int) {
    big_integer r = *this;
    --*this;
    return r;
}

big_integer operator+(big_integer a, big_integer const &b) {
    return a += b;
}

big_integer operator-(big_integer a, big_integer const &b) {
    return a -= b;
}

big_integer operator*(big_integer a, big_integer const &b) {
    size_t  asdasd = (a.isSingleNum_ ? a.value_ : 111);
    return a *= b;
}

big_integer operator/(big_integer a, big_integer const &b) {
    return a /= b;
}

big_integer operator%(big_integer a, big_integer const &b) {
    return a %= b;
}

big_integer operator&(big_integer a, big_integer const &b) {
    return a &= b;
}

big_integer operator|(big_integer a, big_integer const &b) {
    return a |= b;
}

big_integer operator^(big_integer a, big_integer const &b) {
    return a ^= b;
}

big_integer operator<<(big_integer a, int b) {
    return a <<= b;
}

big_integer operator>>(big_integer a, int b) {
    return a >>= b;
}

bool operator==(big_integer const &a, big_integer const &b) {
    if (a.sign_ != b.sign_) return false;
    if (a.isSingleNum_ != b.isSingleNum_) return false;
    if (!a.isSingleNum_ && a.nums_.size() != b.nums_.size()) return false;

    if (a.isSingleNum_) return a.value_ == b.value_;

    for (int i = 0; i < a.nums_.size(); i++) {
        if (a.nums_[i] != b.nums_[i]) return false;
    }

    return true;
}

bool operator!=(big_integer const &a, big_integer const &b) {
    return !(a == b);
}

bool operator<(big_integer const &a, big_integer const &b) {
    if (a.sign_ == 1 && b.sign_ == 0) return true;
    if (a.sign_ == 0 && b.sign_ == 1) return false;

    if (a.isSingleNum_ != b.isSingleNum_) {
        if (a.isSingleNum_) return a.sign_ == 0;
        else return a.sign_ != 0;
    }

    if (a.isSingleNum_) {
        if (a.value_ > b.value_) return a.sign_ != 0;
        if (a.value_ < b.value_) return a.sign_ == 0;
        return false;
    }

    if (a.nums_.size() == b.nums_.size()) {
        for (int i = a.nums_.size() - 1; i >= 0; i--) {
            if (a.nums_[i] > b.nums_[i]) return a.sign_ != 0;
            if (a.nums_[i] < b.nums_[i]) return a.sign_ == 0;
        }

        return false;
    }
    else return a.sign_ == 0 ? a.nums_.size() < b.nums_.size() : a.nums_.size() > b.nums_.size();
}

bool operator>(big_integer const &a, big_integer const &b) {
    return a != b && !(a < b);
}

bool operator<=(big_integer const &a, big_integer const &b) {
    return !(a > b);
}

bool operator>=(big_integer const &a, big_integer const &b) {
    return !(a < b);
}

std::string to_string(big_integer const &a) {
    std::string res = "";
    if (a.isSingleNum_) {
        if (a.value_ == 0) return "0";
        if (a.sign_) res += "-";
        res += std::to_string(a.value_);
        return res;
    }

    big_integer a_copy = a;
    while (a_copy != 0) {
        res += static_cast<char>((a_copy % 10).value_ + '0');
        a_copy /= big_integer(10);
    }

    if (a.sign_) res += "-";
    reverse(res.begin(), res.end());

    return res;
}

std::ostream &operator<<(std::ostream &s, big_integer const &a) {
    return s << to_string(a);
}

void big_integer::to_twos_complement() {
    if (this->sign_) {
        if (this->isSingleNum_) this->value_ = ~this->value_;
        else {
            for (int i = 0; i < this->nums_.size(); i++)
                this->nums_[i] = ~this->nums_[i];
        }
        (*this)--;
    }
}

void big_integer::to_direct_code() {
    if (this->sign_) {
        (*this)++;
        if (this->isSingleNum_) this->value_ = ~this->value_;
        else {
            for (int i = 0; i < this->nums_.size(); i++)
                this->nums_[i] = ~this->nums_[i];
        }
    }
}

void big_integer::delete_leading_zeros() {
    if (this->isSingleNum_) return;

    for (int i = this->nums_.size() - 1; i >= 0 && this->nums_.back() == 0; i--) {
        this->nums_.pop_back();
    }

    if (this->nums_.size() == 1) {
        size_t tmpVal = this->nums_.back();
        this->nums_.~cow_vector();
        this->isSingleNum_ = true;
        this->value_ = tmpVal;
    }
    else if (this->nums_.size() == 0) {
        this->sign_ = false;
        this->nums_.~cow_vector();
        this->isSingleNum_ = true;
        this->value_ = 0;
    }

}