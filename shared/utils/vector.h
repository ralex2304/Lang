#ifndef VECTOR_H_
#define VECTOR_H_

#include <assert.h>
#include <stdlib.h>

#include "macros.h"

struct Vector {
    public:
        inline bool ctor(size_t elem_size, size_t init_cap = 256) {
            assert(elem_size > 0);
            assert(init_cap > 0);
            data_ = calloc(init_cap, elem_size);
            if (data_ == nullptr)
                return false;

            elem_size_ = (ssize_t)elem_size;
            capacity_ = (ssize_t)init_cap;
            size_ = 0;
            return true;
        };

        inline void dtor() {
            FREE(data_);
            capacity_ = UNITIALISED_VAL_;
            size_ = UNITIALISED_VAL_;
            elem_size_ = UNITIALISED_VAL_;
        }

        inline bool push_back(void* new_elem) {
            assert(new_elem);
            if (size_ >= capacity_)
                if (!resize((size_t)capacity_ * 2)) return false;

            memcpy((char*)data_ + elem_size_ * (size_++), new_elem, (size_t)elem_size_);
            return true;
        }

        inline bool resize(size_t new_cap) {
            assert(new_cap > 0);

            void* tmp = data_;
            data_ = recalloc(data_, (size_t)capacity_ * (size_t)elem_size_, new_cap * (size_t)elem_size_);
            if (data_ == nullptr) {
                data_ = tmp;
                return false;
            }

            capacity_ = (ssize_t)new_cap;
            return true;
        }

        inline ssize_t size() {
            return size_;
        }

        inline void* operator[] (size_t i) {
            if ((ssize_t)i >= size_)
                return nullptr;

            return (char*)data_ + (size_t)elem_size_ * i;
        }

    private:
        void* data_ = nullptr;

        static const ssize_t UNITIALISED_VAL_ = -1;
        ssize_t size_ = UNITIALISED_VAL_;
        ssize_t capacity_ = UNITIALISED_VAL_;
        ssize_t elem_size_ = UNITIALISED_VAL_;
};

#endif //< #ifndef VECTOR_H_
