/**
 * @file test_vector.c
 * @brief Test for vector
 * @date 2026-07-22
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

#include "test.h"

#define M2H_VEC_T int
#define M2H_VEC_DISPT Int
#include "md2html/base/vector.h"
#undef M2H_VEC_DISPT
#undef M2H_VEC_T

TEST(vector_ctor_normal) {
    const size_t cap = M2H_DEFAULT_VEC_SIZE;
    M2H_VectorInt vec;

    ASSERT_OK(M2H_vector_int_ctor(&vec, cap), fail);
    ASSERT_EQ(vec.len, 0, fail);
    ASSERT_EQ(vec.cap, cap, fail);

    M2H_vector_int_dtor(&vec);
    return TEST_RESULT_PASS;
fail:
    M2H_vector_int_dtor(&vec);
    return TEST_RESULT_FAIL;
}

TEST(vector_ctor_zero) {
    M2H_VectorInt vec;

    ASSERT_EQ(M2H_vector_int_ctor(&vec, 0), M2H_RESULT_ILLEGAL_ARGUMENT, fail);

    M2H_vector_int_dtor(&vec);
    return TEST_RESULT_PASS;
fail:
    M2H_vector_int_dtor(&vec);
    return TEST_RESULT_FAIL;
}

TEST(vector_ctor_overflow) {
    M2H_VectorInt vec;

    ASSERT_EQ(M2H_vector_int_ctor(&vec, M2H_MAX_VEC_CAP + 1),
              M2H_RESULT_ILLEGAL_ARGUMENT, fail);

    M2H_vector_int_dtor(&vec);
    return TEST_RESULT_PASS;
fail:
    M2H_vector_int_dtor(&vec);
    return TEST_RESULT_FAIL;
}

TEST(vector_dtor_normal) {
    M2H_VectorInt vec;

    ASSERT_OK(M2H_vector_int_ctor(&vec, M2H_DEFAULT_VEC_SIZE), fail);
    M2H_vector_int_dtor(&vec);
    ASSERT_EQ(vec.ptr, NULL, fail);
    ASSERT_EQ(vec.len, 0, fail);
    ASSERT_EQ(vec.cap, 0, fail);

    return TEST_RESULT_PASS;
fail:
    return TEST_RESULT_FAIL;
}

TEST(vector_dtor_double) {
    M2H_VectorInt vec;

    ASSERT_OK(M2H_vector_int_ctor(&vec, M2H_DEFAULT_VEC_SIZE), fail);
    M2H_vector_int_dtor(&vec);

    // double free
    M2H_vector_int_dtor(&vec);

    return TEST_RESULT_PASS;
fail:
    return TEST_RESULT_FAIL;
}

TEST(vector_reserve_normal) {
    M2H_VectorInt vec;
    const size_t reserve_cap = 1024;

    ASSERT_OK(M2H_vector_int_ctor(&vec, M2H_DEFAULT_VEC_SIZE), fail);
    ASSERT_OK(M2H_vector_int_reserve(&vec, reserve_cap), fail);
    ASSERT_EQ(vec.cap, reserve_cap, fail);
    M2H_vector_int_dtor(&vec);
    return TEST_RESULT_PASS;
fail:
    return TEST_RESULT_FAIL;
}

TEST(vector_reserve_rw) {
    M2H_VectorInt vec;
    const size_t reserve_cap = 1 << 24;

    ASSERT_OK(M2H_vector_int_ctor(&vec, M2H_DEFAULT_VEC_SIZE), fail);
    ASSERT_OK(M2H_vector_int_reserve(&vec, reserve_cap), fail);
    for (size_t i = 0; i < reserve_cap; ++i) {
        vec.ptr[i] = (int)i;
    }
    for (size_t i = 0; i < reserve_cap; ++i) {
        ASSERT_EQ(vec.ptr[i], (int)i, fail);
    }

    M2H_vector_int_dtor(&vec);
    return TEST_RESULT_PASS;
fail:
    return TEST_RESULT_FAIL;
}

TEST(vector_pushback_little) {
    const size_t cap = M2H_DEFAULT_VEC_SIZE;
    M2H_VectorInt vec;
    ASSERT_OK(M2H_vector_int_ctor(&vec, cap), fail);

    for (size_t i = 0; i < cap; ++i) {
        ASSERT_OK(M2H_vector_int_pushback(&vec, (int)i), fail);
    }
    ASSERT_EQ(vec.len, cap, fail);
    for (size_t i = 0; i < cap; ++i) {
        ASSERT_EQ(vec.ptr[i], (int)i, fail);
    }

    M2H_vector_int_dtor(&vec);
    return TEST_RESULT_PASS;
fail:
    M2H_vector_int_dtor(&vec);
    return TEST_RESULT_FAIL;
}

TEST(vector_pushback_bulk) {
    const size_t cap = M2H_DEFAULT_VEC_SIZE;
    const size_t ratio = 1 << 16;
    M2H_VectorInt vec;
    ASSERT_OK(M2H_vector_int_ctor(&vec, cap), fail);

    for (size_t i = 0; i < cap * ratio; ++i) {
        ASSERT_OK(M2H_vector_int_pushback(&vec, (int)i), fail);
        ASSERT_EQ(vec.len, i + 1, fail);
    }
    for (size_t i = 0; i < cap * ratio; ++i) {
        ASSERT_EQ(vec.ptr[i], (int)i, fail);
    }

    M2H_vector_int_dtor(&vec);
    return TEST_RESULT_PASS;
fail:
    M2H_vector_int_dtor(&vec);
    return TEST_RESULT_FAIL;
}

TEST(vector_top_normal) {
    const size_t cap = M2H_DEFAULT_VEC_SIZE;
    const size_t ratio = 1 << 8;
    M2H_VectorInt vec;
    ASSERT_OK(M2H_vector_int_ctor(&vec, cap), fail);

    for (size_t i = 0; i < cap * ratio; ++i) {
        ASSERT_OK(M2H_vector_int_pushback(&vec, (int)i), fail);
        int top;
        ASSERT_OK(M2H_vector_int_top(&vec, &top), fail);
        ASSERT_EQ(top, (int)i, fail);
    }

    M2H_vector_int_dtor(&vec);
    return TEST_RESULT_PASS;
fail:
    M2H_vector_int_dtor(&vec);
    return TEST_RESULT_FAIL;
}

TEST(vector_top_empty) {
    const size_t cap = M2H_DEFAULT_VEC_SIZE;
    M2H_VectorInt vec;
    ASSERT_OK(M2H_vector_int_ctor(&vec, cap), fail);

    ASSERT_EQ(M2H_vector_int_top(&vec, NULL), M2H_RESULT_EMPTY_VECTOR, fail);

    M2H_vector_int_dtor(&vec);
    return TEST_RESULT_PASS;
fail:
    M2H_vector_int_dtor(&vec);
    return TEST_RESULT_FAIL;
}

TEST(vector_popback_normal) {
    const size_t cap = M2H_DEFAULT_VEC_SIZE;
    const size_t ratio = 1 << 8;
    M2H_VectorInt vec;
    ASSERT_OK(M2H_vector_int_ctor(&vec, cap), fail);

    for (size_t i = 0; i < cap * ratio; ++i) {
        ASSERT_OK(M2H_vector_int_pushback(&vec, (int)i), fail);
    }
    for (size_t i = vec.len; i > 0; --i) {
        ASSERT_EQ(vec.len, i, fail);
        ASSERT_OK(M2H_vector_int_popback(&vec), fail);
    }
    ASSERT_EQ(vec.len, 0, fail);

    M2H_vector_int_dtor(&vec);
    return TEST_RESULT_PASS;
fail:
    M2H_vector_int_dtor(&vec);
    return TEST_RESULT_FAIL;
}

TEST(vector_popback_empty) {
    const size_t cap = M2H_DEFAULT_VEC_SIZE;
    M2H_VectorInt vec;
    ASSERT_OK(M2H_vector_int_ctor(&vec, cap), fail);

    ASSERT_EQ(M2H_vector_int_popback(&vec), M2H_RESULT_EMPTY_VECTOR, fail);

    M2H_vector_int_dtor(&vec);
    return TEST_RESULT_PASS;
fail:
    M2H_vector_int_dtor(&vec);
    return TEST_RESULT_FAIL;
}

TEST(vector_module_common) {
    M2H_VectorInt vec;

    ASSERT_OK(M2H_vector_int_ctor(&vec, M2H_DEFAULT_VEC_SIZE), fail);
    ASSERT_EQ(vec.cap, M2H_DEFAULT_VEC_SIZE, fail);
    ASSERT_EQ(vec.len, 0, fail);
    ASSERT_EQ(M2H_vector_int_top(&vec, NULL), M2H_RESULT_EMPTY_VECTOR, fail);
    ASSERT_EQ(M2H_vector_int_popback(&vec), M2H_RESULT_EMPTY_VECTOR, fail);
    ASSERT_OK(M2H_vector_int_pushback(&vec, 3), fail);
    int val;
    ASSERT_OK(M2H_vector_int_top(&vec, &val), fail);
    ASSERT_EQ(val, 3, fail);
    ASSERT_EQ(vec.len, 1, fail);
    vec.ptr[vec.len - 1] = 2;
    ASSERT_OK(M2H_vector_int_top(&vec, &val), fail);
    ASSERT_EQ(val, 2, fail);
    ASSERT_OK(M2H_vector_int_popback(&vec), fail);
    ASSERT_EQ(vec.len, 0, fail);
    ASSERT_OK(M2H_vector_int_pushback(&vec, 3), fail);
    ASSERT_OK(M2H_vector_int_pushback(&vec, 37), fail);
    ASSERT_OK(M2H_vector_int_pushback(&vec, 490), fail);
    ASSERT_OK(M2H_vector_int_popback(&vec), fail);
    ASSERT_OK(M2H_vector_int_top(&vec, &val), fail);
    ASSERT_EQ(val, 37, fail);
    ASSERT_OK(M2H_vector_int_pushback(&vec, 490), fail);
    ASSERT_OK(M2H_vector_int_top(&vec, &val), fail);
    ASSERT_EQ(val, 490, fail);

    M2H_vector_int_dtor(&vec);
    return TEST_RESULT_PASS;
fail:
    M2H_vector_int_dtor(&vec);
    return TEST_RESULT_FAIL;
}

TEST(vector_module_pressure) {
    M2H_VectorInt vec;
    const size_t n = 2000000 * 2;

    ASSERT_OK(M2H_vector_int_ctor(&vec, M2H_DEFAULT_VEC_SIZE), fail);

    for (size_t i = 0; i < n; ++i) {
        ASSERT_OK(M2H_vector_int_pushback(&vec, (int)(i * 48271u + 12345u)),
                  fail);
    }
    for (size_t i = 0; i < n; ++i) {
        ASSERT_EQ(vec.ptr[i], (int)(i * 48271u + 12345u), fail);
    }
    for (size_t i = 1; i <= n / 2; ++i) {
        ASSERT_OK(M2H_vector_int_popback(&vec), fail);
    }
    for (size_t i = n / 2; i < n; ++i) {
        ASSERT_OK(M2H_vector_int_pushback(&vec, (int)(i * 23371u + 54321u)),
                  fail);
    }
    for (size_t i = 0; i < n; ++i) {
        if (i < n / 2) {
            ASSERT_EQ(vec.ptr[i], (int)(i * 48271u + 12345u), fail);
        } else {
            ASSERT_EQ(vec.ptr[i], (int)(i * 23371u + 54321u), fail);
        }
    }
    for (size_t i = 1; i <= n; ++i) {
        ASSERT_OK(M2H_vector_int_popback(&vec), fail);
    }
    ASSERT_EQ(vec.len, 0, fail);

    M2H_vector_int_dtor(&vec);
    return TEST_RESULT_PASS;
fail:
    M2H_vector_int_dtor(&vec);
    return TEST_RESULT_FAIL;
}

TEST_SUITE_BEGIN(vector)

TEST_ADD(vector_ctor_normal);
TEST_ADD(vector_ctor_zero);
TEST_ADD(vector_ctor_overflow);

TEST_ADD(vector_dtor_normal);
TEST_ADD(vector_dtor_double);

TEST_ADD(vector_reserve_normal);
TEST_ADD(vector_reserve_rw);

TEST_ADD(vector_pushback_little);
TEST_ADD(vector_pushback_bulk);

TEST_ADD(vector_top_normal);
TEST_ADD(vector_top_empty);

TEST_ADD(vector_popback_normal);
TEST_ADD(vector_popback_empty);

TEST_ADD(vector_module_common);
TEST_ADD(vector_module_pressure);

TEST_SUITE_END