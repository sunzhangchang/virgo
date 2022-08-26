_Pragma("once")

#include "mem.hpp"
#include "util.hpp"

namespace virgo {

    template<class T>
    class vector {
    private:
        T * v;
        unsigned len;
        unsigned cap;

        void resize_cap(unsigned n) {
            v = (T *)((v == nullptr)
                ? halloc(n * sizeof(T))
                : hrealloc(this->v, n * sizeof(T)));
            this->cap = n;
        }

        using Self = vector;

        void _assign(const Self & other) {
            this->v = nullptr;
            this->len = this->cap = 0;
            this->resize(other.len);
            for (unsigned i = 0; i < other.len; ++ i) {
                this->at(i) = other[i];
            }
        }

    public:
        using iterator = T *;
        using const_iterator = T const *;

        vector(): v(nullptr), len(0), cap(0) {}
        vector(const Self & other) { this->_assign(other); }
        vector(Self && other) { this->_assign(other); }
        ~vector() { hfree(this->v); this->len = this->cap = 0; }

        iterator begin() { return this->v; }
        iterator end() { return this->v + this->len; }
        const_iterator begin() const { return this->v; }
        const_iterator end() const { return this->v + this->len; }
        const_iterator cbegin() { return this->v; }
        const_iterator cend() { return this->v + this->len; }

        void push_back(T x) {
            if (this->len >= this->cap) {
                this->resize_cap(this->cap ? (this->cap * 2) : 1);
            }
            this->v[this->len ++] = x;
        }

        bool erase(iterator _it) {
            for (auto it = this->begin(); it != this->end(); ++ it) {
                if (it == _it) {
                    for (auto jt = ++ _it; jt != this->end(); ++ jt) {
                        *it = *jt;
                        ++ it;
                    }
                    -- this->len;
                    return true;
                }
            }
            return false;
        }

        iterator find(iterator st, const T & val) {
            for (; st != this->end(); ++ st) {
                if (*st == val) {
                    return st;
                }
            }
            return st;
        }

        const_iterator find(const_iterator st, const T & val) const {
            return this->find(st, val);
        }

        iterator find(const T & val) { return this->find(this->begin(), val); }
        const_iterator find(const T & val) const { return this->find(this->begin(), val); }

        bool empty() const { return this->len == 0; }
        unsigned size() const { return this->len; }
        unsigned capacity() const { return this->cap; }

        T & at(unsigned i) & { assert(i < this->len); return this->v[i]; }
        const T & at(unsigned i) const & { assert(i < this->len); return this->v[i]; }

        T & operator[](unsigned i) & { return this->at(i); }
        const T & operator[](unsigned i) const & { return this->at(i); }

        Self & operator=(const Self & other) & {
            if (this->v != nullptr) {
                this->~vector();
            }
            this->_assign(other);
            return *this;
        }
        const T & operator=(const Self & other) const & {
            if (this->v != nullptr) {
                this->~vector();
            }
            this->_assign(other);
            return *this;
        }

        void resize(unsigned n) {
            if (n > this->len) {
                if (n > this->cap) {
                    unsigned new_cap = this->cap + (this->cap == 0);
                    while (n > new_cap) {
                        new_cap <<= 1;
                    }
                    this->resize_cap(new_cap);
                }
            }
            this->len = n;
        }

        void clear() { this->len = 0; }

        void shrink_to_fit() {
            if (this->capacity() > this->size()) {
                this->resize_cap(this->len);
            }
        }

        template<class F>
        Self filter(F && func) const {
            auto t = *this;
            t.resize(copy_if(t.begin(), t.end(), t.begin(), func) - t.begin());
            t.shrink_to_fit();
            return t;
        }

        bool has(const T & val) const {
            return this->find(val) != this->end();
        }
    };
}
