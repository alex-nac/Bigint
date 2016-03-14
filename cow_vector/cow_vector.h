#ifndef COW_VECTOR_H
#define COW_VECTOR_H

#include <stddef.h>
#include <vector>

struct cow_vector {
    cow_vector();
    cow_vector(int n);
    cow_vector(int n, size_t value);
    cow_vector(const cow_vector& other);
    cow_vector& operator= (const cow_vector& other);
    ~cow_vector();

    std::vector<size_t>::const_iterator begin() const;
    //std::vector<size_t>::iterator begin();
    std::vector<size_t>::const_iterator end() const;
    //std::vector<size_t>::iterator end();

    size_t size() const;
    void resize(size_t n);

    size_t& operator[] (size_t n);
    const size_t& operator[] (size_t n) const;
    const size_t& back() const;
    size_t& back();

    void push_back(const size_t& val);
    void pop_back();
    void insert(std::vector<size_t>::const_iterator position, std::vector<size_t>::const_iterator first, std::vector<size_t>::const_iterator last);
    void insert(std::vector<size_t>::const_iterator position, size_t n, const size_t& val);
    void erase(std::vector<size_t>::const_iterator first, std::vector<size_t>::const_iterator last);

private:
    void prepare_to_changing();

private:
    class Data {
    friend class cow_vector;

    public:
        Data();
        Data(int n);
        Data(int n, size_t value);
        Data(const Data& d);
    private:
        size_t count_;
        std::vector<size_t> vector_;
    };

    Data* data_;
};

#endif //COW_VECTOR_H
