#ifndef NIBBLE_HXX
#define NIBBLE_HXX
#include <cstdint>

class Nibble {
public:
    Nibble() : data_(0) {}
    Nibble(uint8_t data) : data_(data & 0xF) {}
    Nibble operator&(const Nibble& other) const {
        return Nibble(data_ & other.data_);
    }
    Nibble operator|(const Nibble& other) const {
        return Nibble(data_ | other.data_);
    }
    Nibble operator^(const Nibble& other) const {
        return Nibble(data_ ^ other.data_);
    }
    Nibble operator~() const {
        return Nibble(~data_);
    }
    Nibble operator<<(const Nibble& other) const {
        return Nibble(data_ << other.data_);
    }
    Nibble operator>>(const Nibble& other) const {
        return Nibble(data_ >> other.data_);
    }
    Nibble operator+(const Nibble& other) const {
        return Nibble(data_ + other.data_);
    }
    Nibble operator-(const Nibble& other) const {
        return Nibble(data_ - other.data_);
    }
    Nibble& operator&=(const Nibble& other) {
        data_ &= other.data_;
        return *this;
    }
    Nibble& operator|=(const Nibble& other) {
        data_ |= other.data_;
        return *this;
    }
    Nibble& operator^=(const Nibble& other) {
        data_ ^= other.data_;
        return *this;
    }
    Nibble& operator<<=(const Nibble& other) {
        data_ <<= other.data_;
        data_ &= 0xF;
        return *this;
    }
    Nibble& operator>>=(const Nibble& other) {
        data_ >>= other.data_;
        data_ &= 0xF;
        return *this;
    }
    Nibble& operator+=(const Nibble& other) {
        data_ += other.data_;
        data_ &= 0xF;
        return *this;
    }
    Nibble& operator-=(const Nibble& other) {
        data_ -= other.data_;
        data_ &= 0xF;
        return *this;
    }
    bool operator==(const Nibble& other) const {
        return data_ == other.data_;
    }
    bool operator!=(const Nibble& other) const {
        return data_ != other.data_;
    }
    bool operator<(const Nibble& other) const {
        return data_ < other.data_;
    }
    bool operator>(const Nibble& other) const {
        return data_ > other.data_;
    }
    bool operator<=(const Nibble& other) const {
        return data_ <= other.data_;
    }
    bool operator>=(const Nibble& other) const {
        return data_ >= other.data_;
    }
    uint8_t Get() const {
        return data_;
    }
private:
    uint8_t data_;
};
#endif