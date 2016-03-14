#include <assert.h>
#include "cow_vector.h"

cow_vector::Data::Data() : count_(1), vector_() { }

cow_vector::Data::Data(int n)  : count_(1), vector_(n) { }

cow_vector::Data::Data(int n, size_t value)  : count_(1), vector_(n, value) { }

cow_vector::Data::Data(const Data& d) : count_(1), vector_(d.vector_) { }



cow_vector::cow_vector() : data_(new Data()) { }

cow_vector::cow_vector(int n) : data_(new Data(n)) { }

cow_vector::cow_vector(int n, size_t value) : data_(new Data(n, value)) { }

cow_vector::cow_vector(const cow_vector& other) : data_(other.data_) {
    ++data_->count_;
}

cow_vector& cow_vector::operator= (const cow_vector& other) {
    Data* const old = data_;

    data_ = other.data_;
    ++data_->count_;
    if (--old->count_ == 0) delete old;
    return *this;
}

cow_vector::~cow_vector() {
    if (--data_->count_ == 0) delete data_;
}

void cow_vector::prepare_to_changing() {
    if (data_->count_ > 1) {
        Data* d = new Data(*data_);
        --data_->count_;
        data_ = d;
    }

    assert(data_->count_ == 1);
}

std::vector<size_t>::const_iterator cow_vector::begin() const {
    return data_->vector_.begin();
}

std::vector<size_t>::const_iterator cow_vector::end() const {
    return data_->vector_.end();
}

size_t cow_vector::size() const {
    return data_->vector_.size();
}

void cow_vector::resize(size_t n) {
    prepare_to_changing();
    data_->vector_.resize(n);
}

size_t& cow_vector::operator[](size_t n) {
    prepare_to_changing();
    return data_->vector_[n];
}

const size_t& cow_vector::operator[](size_t n) const {
    return data_->vector_[n];
}

const size_t& cow_vector::back() const {
    return data_->vector_.back();
}

size_t& cow_vector::back() {
    prepare_to_changing();
    return data_->vector_.back();
}

void cow_vector::push_back(const size_t &val) {
    prepare_to_changing();
    data_->vector_.push_back(val);
}

void cow_vector::pop_back() {
    prepare_to_changing();
    return data_->vector_.pop_back();
}

void cow_vector::insert(std::vector<size_t>::const_iterator position, size_t n, const size_t& val) {
    prepare_to_changing();
    data_->vector_.insert(position, n, val);
}

void cow_vector::insert(std::vector<size_t>::const_iterator position, std::vector<size_t>::const_iterator first, std::vector<size_t>::const_iterator last) {
    prepare_to_changing();
    data_->vector_.insert(position, first, last);
}

void cow_vector::erase(std::vector<size_t>::const_iterator first, std::vector<size_t>::const_iterator last) {
    prepare_to_changing();
    data_->vector_.erase(first, last);
}
