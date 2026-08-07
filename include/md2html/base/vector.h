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
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "md2html/base/common.h"
#define M2H_DEFAULT_VEC_SIZE 2
#define M2H_MAX_VEC_CAP (SIZE_MAX / 2)

#if defined(M2H_VEC_T) && defined(M2H_VEC_DISPT)

// mock undef
#include "mock_funcs_undef.h"

#include "md2html/base/result.h"
#include <stdlib.h>

// mock def
#include "mock_funcs.h"

#define _CONCAT_INNER(a, b) a##b
#define _CONCAT(a, b) _CONCAT_INNER(a, b)

#define T M2H_VEC_T
#define DT M2H_VEC_DISPT
#define VECT _CONCAT(M2H_Vector, DT)
#define FUNC_PREF _CONCAT(_CONCAT(M2H_vector_, T), _)

typedef struct {
    T *ptr;
    size_t len;
    size_t cap;
} VECT;

/**
 * @brief Construct a vector
 * @param self Out, the vector to construct
 * @param cap In, the initial capacity of vector
 */
[[maybe_unused]] static M2H_Result _CONCAT(FUNC_PREF, ctor)(M2H_OUT VECT *self,
                                                            M2H_IN size_t cap) {
    if (self == NULL || cap == 0 || cap > M2H_MAX_VEC_CAP) {
        return M2H_RESULT_ILLEGAL_ARGUMENT;
    }
    self->len = 0;
    self->cap = cap;
    self->ptr = (T *)malloc(cap * sizeof(T));
    if (self->ptr == NULL) {
        return M2H_RESULT_MALLOC_FAIL;
    }
    return M2H_RESULT_OK;
}

/**
 * @brief Destruct a vector
 * @note After destruction, all of the member will be @c 0 or @c NULL
 * @param self Out, the vector to destruct
 */
[[maybe_unused]] static M2H_Result _CONCAT(FUNC_PREF,
                                           dtor)(M2H_OUT VECT *self) {
    if (self == NULL) {
        return M2H_RESULT_ILLEGAL_ARGUMENT;
    }
    free(self->ptr);
    self->ptr = NULL;
    self->len = 0;
    self->cap = 0;
    return M2H_RESULT_OK;
}

/**
 * @brief Reserve memory for vector
 * @note 1. The max capacity is @c M2H_MAX_VEC_CAP
 *       2. The function will do nothing when the new capacity is less than or
 *          equal to the current
 * @param self In & out, the vector being reserved
 * @param cap In, the capacity that the function reserves
 */
[[maybe_unused]] static M2H_Result
_CONCAT(FUNC_PREF, reserve)(M2H_INOUT VECT *self, M2H_IN size_t cap) {
    if (self == NULL || cap == 0 || cap > M2H_MAX_VEC_CAP ||
        self->ptr == NULL || cap <= self->cap) {
        return M2H_RESULT_ILLEGAL_ARGUMENT;
    }
    T *new_ptr = (T *)realloc(self->ptr, cap * sizeof(T));
    if (new_ptr == NULL) {
        return M2H_RESULT_MALLOC_FAIL;
    }
    self->ptr = new_ptr;
    self->cap = cap;
    return M2H_RESULT_OK;
}

/**
 * @brief Push an element to the back of a vector
 * @note The capacity of the vector will be doubled before operation when
 *       the vector is full
 * @param self In & out, the vector accepting the element
 * @param elem In, the element to be pushed back
 */
[[maybe_unused]] static M2H_Result
_CONCAT(FUNC_PREF, pushback)(M2H_INOUT VECT *self, M2H_IN T elem) {
    if (self == NULL || self->ptr == NULL) {
        return M2H_RESULT_ILLEGAL_ARGUMENT;
    }
    if (self->cap <= self->len) {
        // use "/ 2" rather than "* 2" to avoid overflow
        if (self->cap >= M2H_MAX_VEC_CAP / 2) {
            return M2H_RESULT_MAX_CAP_EXCEEDED;
        }
        M2H_RELAY(_CONCAT(FUNC_PREF, reserve)(self, self->cap * 2));
    }
    self->ptr[self->len] = elem;
    self->len++;
    return M2H_RESULT_OK;
}

/**
 * @brief Get the top element of a vector
 * @note The vector cannot be empty and value cannot be @c NULL
 * @param self In, the vector
 * @param value Out, the space where the result will be return
 */
[[maybe_unused]] static M2H_Result _CONCAT(FUNC_PREF, top)(M2H_IN VECT *self,
                                                           M2H_OUT T *value) {
    if (self == NULL || self->ptr == NULL || value == NULL) {
        return M2H_RESULT_ILLEGAL_ARGUMENT;
    }
    if (self->len == 0) {
        return M2H_RESULT_EMPTY_VECTOR;
    }
    *value = self->ptr[self->len - 1];
    return M2H_RESULT_OK;
}

/**
 * @brief Remove the last element of a vector
 * @note 1. Pop back an empty vector is not allowed
 *       2. The capacity will not be changed
 * @param self Out, the vector
 */
[[maybe_unused]] static M2H_Result _CONCAT(FUNC_PREF,
                                           popback)(M2H_OUT VECT *self) {
    if (self == NULL) {
        return M2H_RESULT_ILLEGAL_ARGUMENT;
    }
    if (self->len == 0) {
        return M2H_RESULT_EMPTY_VECTOR;
    }
    self->len--;
    return M2H_RESULT_OK;
}

#undef FUNC_PREF
#undef VECT
#undef DT
#undef T

#undef _CONCAT
#undef _CONCAT_INNER

// mock undef
#include "mock_funcs_undef.h"

#endif // M2H_VEC_T && M2H_VEC_DISPT