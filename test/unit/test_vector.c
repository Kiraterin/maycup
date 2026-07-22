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
    size_t cap = M2H_DEFAULT_VEC_SIZE;
    M2H_VectorInt vec;
    M2H_Result res = M2H_vector_int_ctor(&vec, cap);

    ASSERT_EQ(res, M2H_RESULT_OK, fail);
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
    M2H_Result res = M2H_vector_int_ctor(&vec, 0);

    ASSERT_EQ(res, M2H_RESULT_ILLEGAL_ARGUMENT, fail);

    M2H_vector_int_dtor(&vec);
    return TEST_RESULT_PASS;
fail:
    M2H_vector_int_dtor(&vec);
    return TEST_RESULT_FAIL;
}

TEST(vector_ctor_overflow) {
    M2H_VectorInt vec;
    M2H_Result res = M2H_vector_int_ctor(&vec, M2H_MAX_VEC_CAP + 1);

    ASSERT_EQ(res, M2H_RESULT_ILLEGAL_ARGUMENT, fail);

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
TEST_SUITE_END