/**
 * @file vector.h
 * @brief Generic vector data structure
 * @date 2026-07-09
 * @copyright GPLv3 License
 * @section LICENSE
 * md2html
 * Copyright (C) 2026 Kiraterin
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https: //www.gnu.org/licenses/>.
 */

#ifndef VECTOR_H
#define VECTOR_H

#include <common.h>
#define M2H_DEFAULT_VEC_SIZE 2
#define M2H_MAX_VEC_CAP (SIZE_MAX / 2)

// #define M2H_VEC_T char
// #define M2H_VEC_DISPT Char

#if defined(M2H_VEC_T) && defined(M2H_VEC_DISPT)

#include "lib_marker.h"
#include "result.h"
#include <stdlib.h>

#define CONCAT_INNER(a, b) a##b
#define CONCAT(a, b) CONCAT_INNER(a, b)

#define T M2H_VEC_T
#define DT M2H_VEC_DISPT
#define VECT CONCAT(M2H_Vector, DT)
#define FUNC_PREF CONCAT(CONCAT(M2H_vector_, T), _)

typedef struct {
    T *ptr;
    size_t len;
    size_t cap;
} VECT;

[[maybe_unused]] static M2H_Result CONCAT(FUNC_PREF, ctor)(M2H_OUT VECT *self,
                                                           M2H_IN size_t cap) {
    if (cap == 0 || cap > M2H_MAX_VEC_CAP) {
        return M2H_RESULT_ILLEGAL_ARGUMENT;
    }
    self->len = 0;
    self->cap = cap;
    self->ptr = (T *)malloc(cap * sizeof(T));
    if (self->ptr == NULL) {
        return M2H_RESULT_NOMEM;
    }
    return M2H_RESULT_OK;
}

[[maybe_unused]] static M2H_Result CONCAT(FUNC_PREF, dtor)(M2H_OUT VECT *self) {
    if (self->ptr == NULL) {
        return M2H_RESULT_NULL_DESTROY;
    }
    free(self->ptr);
    self->len = 0;
    self->cap = 0;
    return M2H_RESULT_OK;
}

[[maybe_unused]] static M2H_Result
CONCAT(FUNC_PREF, reserve)(M2H_INOUT VECT *self, M2H_IN size_t cap) {
    if (cap == 0 || cap > M2H_MAX_VEC_CAP) {
        return M2H_RESULT_ILLEGAL_ARGUMENT;
    }
    if (cap <= self->cap) {
        return M2H_RESULT_OK;
    }
    T *new_ptr = (T *)realloc(self->ptr, cap * sizeof(T));
    if (new_ptr == NULL) {
        return M2H_RESULT_NOMEM;
    }
    self->ptr = new_ptr;
    self->cap = cap;
    return M2H_RESULT_OK;
}

[[maybe_unused]] static M2H_Result
CONCAT(FUNC_PREF, pushback)(M2H_INOUT VECT *self, M2H_IN T elem) {
    if (self->cap == self->len) {
        M2H_RELAY(CONCAT(FUNC_PREF, reserve)(self, self->cap * 2));
    }
    self->ptr[self->len] = elem;
    self->len++;
    return M2H_RESULT_OK;
}

[[maybe_unused]] static M2H_Result CONCAT(FUNC_PREF, top)(M2H_IN VECT *self,
                                                          M2H_OUT T *value) {
    if (self->len == 0) {
        return M2H_RESULT_EMPTY_VECTOR;
    }
    *value = self->ptr[self->len - 1];
    return M2H_RESULT_OK;
}

[[maybe_unused]] static M2H_Result CONCAT(FUNC_PREF,
                                          popback)(M2H_OUT VECT *self) {
    if (self->len == 0) {
        return M2H_RESULT_EMPTY_VECTOR;
    }
    self->ptr--;
    self->len--;
    return M2H_RESULT_OK;
}

#undef FUNC_PREF
#undef VECT
#undef DT
#undef T

#undef CONCAT
#undef CONCAT_INNER

#endif // M2H_VEC_T && M2H_VEC_DISPT

#endif // VECTOR_H