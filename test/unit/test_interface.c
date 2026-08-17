/**
 * @file test_app_maycup.c
 * @brief Test for interfaces
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
#include <string.h>

TEST_CASE(convert_file) {
    ASSERT_OK(maycup_convert_file(UTEST_FIXTURE_PATH_INPUT "interface_1.md",
                                  UTEST_TMP_DIR "itf_test.html"),
              fail);
    ASSERT_FILE_EQ_FILE(UTEST_TMP_DIR "itf_test.html",
                        UTEST_FIXTURE_PATH_EXPECTED "interface_1.html", fail);
    return TEST_RESULT_PASS;
fail:
    return TEST_RESULT_FAIL;
}

#include "fixture/expected/interface_str_1.h"
#include "fixture/input/interface_str_1.h"

TEST_CASE(convert_string) {
    char buf[8192];
    ASSERT_OK(maycup_convert_string(interface_str_1_input,
                                    strlen(interface_str_1_input), buf,
                                    sizeof(buf)),
              fail);
    ASSERT_EQ(strcmp(buf, interface_str_1_expected), 0, fail);
    return TEST_RESULT_PASS;
fail:
    return TEST_RESULT_FAIL;
}

TEST_SUITE_BEGIN(interface)

TEST_CASE_ADD(convert_file);
TEST_CASE_ADD(convert_string);

TEST_SUITE_END