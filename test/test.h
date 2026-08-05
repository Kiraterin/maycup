/**
 * @file test.h
 * @brief Test tool
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

#ifndef TEST_H
#define TEST_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

// helper macros
#define CONCAT_INNER(a, b) a##b
#define CONCAT(a, b) CONCAT_INNER(a, b)
#define STRINGIFY_INNER(a) #a
#define STRINGIFY(a) STRINGIFY_INNER(a)

// constants
#define MAX_TEST_SUITE_CNT 16
#define MAX_TEST_CASE_EACH_CNT 128

// structs
typedef enum { TEST_RESULT_PASS = 0, TEST_RESULT_FAIL = 1 } TestResult;
typedef TestResult (*TestFunc)();
typedef struct {
    const char *case_name;
    TestFunc case_func;
} TestCase;
typedef struct {
    const char *suite_name;
    TestCase cases[MAX_TEST_CASE_EACH_CNT];
    size_t case_cnt;
} TestSuite;

// test case macros
#define TEST_CASE_NAME(name) CONCAT(test_, name)
#define TEST_CASE(name) static TestResult TEST_CASE_NAME(name)(void)

// test suite macros
#define TEST_SUITE_NAME(name) CONCAT(test_suite_, name)
#define TEST_SUITE_DECLARE(name) void TEST_SUITE_NAME(name)(void)
#define TEST_SUITE_BEGIN(name)                                                 \
    TEST_SUITE_DECLARE(name) {                                                 \
        TEST_CTX.suites[TEST_CTX.suite_cnt].suite_name =                       \
            STRINGIFY(TEST_SUITE_NAME(name));
#define TEST_CASE_ADD(case)                                                    \
    do {                                                                       \
        TEST_CTX.suites[TEST_CTX.suite_cnt]                                    \
            .cases[TEST_CTX.suites[TEST_CTX.suite_cnt].case_cnt++] =           \
            (TestCase){.case_name = STRINGIFY(TEST_CASE_NAME(case)),           \
                       .case_func = TEST_CASE_NAME(case)};                     \
    } while (false)
#define TEST_SUITE_END                                                         \
    ++TEST_CTX.suite_cnt;                                                      \
    }
#define TEST_SUITE_REG(name)                                                   \
    do {                                                                       \
        TEST_SUITE_DECLARE(name);                                              \
        CONCAT(test_suite_, name)();                                           \
    } while (false)

// test run macro
#define TEST_RUN                                                               \
    do {                                                                       \
        void run_all_test_suites();                                            \
        run_all_test_suites();                                                 \
    } while (false)

// color macros
#define COLOR_LIGHT_RED "\033[91m"
#define COLOR_LIGHT_GREEN "\033[92m"
#define COLOR_LIGHT_WHITE "\033[97m"
#define COLOR_RESET "\033[0m"

// asserts
#define ASSERT_EQ(expr, val, fail_label)                                       \
    do {                                                                       \
        if ((expr) != (val)) {                                                 \
            printf("[FAIL] assertion failed("__FILE__                          \
                   ":" STRINGIFY(__LINE__) " (%s) == (%s)): \n",               \
                   STRINGIFY(expr), STRINGIFY(val));                           \
            printf("\tactual:   (%s) == %lld\n", STRINGIFY(expr),              \
                   (long long)(expr));                                         \
            printf("\texpected: (%s) == %lld\n", STRINGIFY(val),               \
                   (long long)(val));                                          \
            goto fail_label;                                                   \
        }                                                                      \
    } while (false)

#define ASSERT_NEQ(expr, val, fail_label)                                      \
    do {                                                                       \
        if ((expr) == (val)) {                                                 \
            printf("[FAIL] assertion failed("__FILE__                          \
                   ":" STRINGIFY(__LINE__) " (%s) != (%s)): \n",               \
                   STRINGIFY(expr), STRINGIFY(val));                           \
            goto fail_label;                                                   \
        }                                                                      \
    } while (false)

#define ASSERT_OK(expr, fail_label) ASSERT_EQ((expr), M2H_RESULT_OK, fail_label)

// mock functions
typedef struct {
    bool m_malloc : 1;
    bool m_realloc : 1;
} TestMockState;

void *malloc_mock(size_t p);
void *realloc_mock(void *pa, size_t pb);

// test context struct and macros
typedef struct {
    TestSuite suites[MAX_TEST_SUITE_CNT];
    size_t suite_cnt;
    TestMockState mock_state;
} TestContext;

#define TEST_CTX test_ctx
#define TEST_CTX_DEF                                                           \
    TestContext TEST_CTX = {                                                   \
        .mock_state = {.m_malloc = false, .m_realloc = false}}
extern TestContext TEST_CTX;

#endif // TEST_H