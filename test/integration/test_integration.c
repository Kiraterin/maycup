/**
 * @file test_integration.c
 * @brief Integrated tests for maycup
 * @date 2026-08-18
 * @copyright GPLv3 License
 * @section LICENSE
 * maycup
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

#include "maycup.h"
#include "test.h"

#ifdef DETAIL
#define OUTFILE(name) STRINGIFY(name) ".html"
#else
#define OUTFILE(name) "itest.html"
#endif // DETAIL

#define ITEST_CASE(name)                                                       \
    TEST_CASE(name) {                                                          \
        maycup_convert_file(ITEST_FIXTURE_PATH_INPUT STRINGIFY(name) ".md",    \
                            ITEST_TMP_DIR OUTFILE(name));                      \
        ASSERT_FILE_EQ_FILE(                                                   \
            ITEST_TMP_DIR OUTFILE(name),                                       \
            ITEST_FIXTURE_PATH_EXPECTED STRINGIFY(name) ".html", fail);        \
        return TEST_RESULT_PASS;                                               \
    fail:                                                                      \
        return TEST_RESULT_FAIL;                                               \
    }

ITEST_CASE(title);
ITEST_CASE(title_attached);
ITEST_CASE(paragraph);
ITEST_CASE(paragraph_eof_attached);
ITEST_CASE(title_and_paragraph);

TEST_SUITE_BEGIN(integration)

TEST_CASE_ADD(title);
TEST_CASE_ADD(title_attached);
TEST_CASE_ADD(paragraph);
TEST_CASE_ADD(paragraph_eof_attached);
TEST_CASE_ADD(title_and_paragraph);

TEST_SUITE_END