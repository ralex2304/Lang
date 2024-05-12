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

        inline bool is_initialised() { return capacity_ != UNITIALISED_VAL_; };

        inline bool push_back(const void* new_elem) {
            assert(new_elem);
            if (!prepare_space_for_elems(1))
                return false;

            memcpy((char*)data_ + elem_size_ * size_++, new_elem, (size_t)elem_size_);
            return true;
        }

        inline bool push_back_several(const void* new_elems, const size_t num) {
            assert(new_elems);
            if (!prepare_space_for_elems(num))
                return false;

            memcpy((char*)data_ + elem_size_ * size_, new_elems, (size_t)elem_size_ * num);
            size_ += num;
            return true;
        }

        inline bool push_zero_elems(size_t num) {
            bool res = prepare_space_for_elems(num);
            size_ += num;
            return res;
        }

        inline bool prepare_space_for_elems(size_t num) {
            if (size_ + (ssize_t)num < capacity_)
                return true;

            size_t new_cap = (size_t)capacity_;

            while ((size_t)size_ + num >= new_cap) new_cap *= 2;

            return resize(new_cap);
        }

        inline bool align(size_t elems_alignment) {
            if ((size_t)size() % elems_alignment == 0) return true;

            return push_zero_elems(elems_alignment - ((size_t)size() % elems_alignment));
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

        inline void fixup(size_t index, const void* elem) {
            assert(elem);
            fixup_several(index, elem, 1);
        }

        inline void fixup_several(size_t index, const void* elems, size_t num) {
            assert(elems);
            memcpy((char*)data_ + index * (size_t)elem_size_, elems, num * (size_t)elem_size_);
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
