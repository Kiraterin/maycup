/**
 * @file test_reader.c
 * @brief Test for reader
 * @date 2026-08-05
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

#include "md2html/io/reader.h"
#include "test.h"
#include <errno.h>

TEST_CASE(filereader_ctor_normal) {
    M2H_FileReader fr;
    const char *filename = "./test/unit/fixture/input/reader-lipsum-1.txt";

    ASSERT_OK(M2H_filereader_ctor(&fr, filename), fail);
    ASSERT_NEQ(fr.base.get_char, NULL, fail);
    ASSERT_NEQ(fr.base.seek, NULL, fail);
    ASSERT_NEQ(fr.base.tell, NULL, fail);
    ASSERT_NEQ(fr.fp, NULL, fail);

    ASSERT_OK(M2H_filereader_dtor(&fr), fail);
    return TEST_RESULT_PASS;
fail:
    M2H_filereader_dtor(&fr);
    return TEST_RESULT_FAIL;
}

TEST_CASE(filereader_ctor_no_entry) {
    M2H_FileReader fr;
    const char *filename = "./test/unit/fixture/reader-lipsum-1-fake.txt";

    ASSERT_EQ(M2H_filereader_ctor(&fr, filename), M2H_RESULT_ERRNO, fail);
    ASSERT_EQ(errno, ENOENT, fail);

    return TEST_RESULT_PASS;
fail:
    M2H_filereader_dtor(&fr);
    return TEST_RESULT_FAIL;
}

TEST_CASE(filereader_ctor_illegal_arg) {
    M2H_FileReader fr;
    const char *filename = "./test/unit/fixture/input/reader-lipsum-1.txt";
    ASSERT_EQ(M2H_filereader_ctor(NULL, filename), M2H_RESULT_ILLEGAL_ARGUMENT,
              fail);
    ASSERT_EQ(M2H_filereader_ctor(&fr, NULL), M2H_RESULT_ILLEGAL_ARGUMENT,
              fail);

    return TEST_RESULT_PASS;
fail:
    return TEST_RESULT_FAIL;
}

TEST_CASE(filereader_dtor_normal) {
    M2H_FileReader fr;
    const char *filename = "./test/unit/fixture/input/reader-lipsum-1.txt";

    ASSERT_OK(M2H_filereader_ctor(&fr, filename), fail);
    ASSERT_OK(M2H_filereader_dtor(&fr), fail);
    ASSERT_EQ(fr.base.get_char, NULL, fail);
    ASSERT_EQ(fr.base.seek, NULL, fail);
    ASSERT_EQ(fr.base.tell, NULL, fail);
    ASSERT_EQ(fr.fp, NULL, fail);

    return TEST_RESULT_PASS;
fail:
    M2H_filereader_dtor(&fr);
    return TEST_RESULT_FAIL;
}

TEST_CASE(filereader_dtor_double) {
    M2H_FileReader fr;
    const char *filename = "./test/unit/fixture/input/reader-lipsum-1.txt";

    ASSERT_OK(M2H_filereader_ctor(&fr, filename), fail);
    ASSERT_OK(M2H_filereader_dtor(&fr), fail);
    ASSERT_EQ(M2H_filereader_dtor(&fr), M2H_RESULT_ILLEGAL_ARGUMENT, fail);

    return TEST_RESULT_PASS;
fail:
    M2H_filereader_dtor(&fr);
    return TEST_RESULT_FAIL;
}

TEST_CASE(filereader_dtor_illegal_arg) {
    ASSERT_EQ(M2H_filereader_dtor(NULL), M2H_RESULT_ILLEGAL_ARGUMENT, fail);
    return TEST_RESULT_PASS;
fail:
    return TEST_RESULT_FAIL;
}

TEST_CASE(filereader_get_char_normal) {
    M2H_FileReader fr;
    const char *filename_input =
        "./test/unit/fixture/input/reader-lipsum-1.txt";
    const char *filename_expected =
        "./test/unit/fixture/expected/reader-lipsum-1.txt";
    FILE *fp = fopen(filename_expected, "r");

    int actual;
    int expected;
    bool swc = false;

    ASSERT_OK(M2H_filereader_ctor(&fr, filename_input), fail);

    while (true) {
        expected = fgetc(fp);
        if (swc) {
            ASSERT_OK(fr.base.get_char((M2H_Reader *)&fr, &actual), fail);
            ASSERT_EQ(actual, expected, fail);
        } else {
            ASSERT_OK(fr.base.get_char((M2H_Reader *)&fr, NULL), fail);
        }
        if (expected == EOF) {
            if (!feof(fp)) {
                goto fail;
            } else {
                break;
            }
        }
        swc = !swc;
    }

    fclose(fp);
    ASSERT_OK(M2H_filereader_dtor(&fr), fail);
    return TEST_RESULT_PASS;
fail:
    fclose(fp);
    M2H_filereader_dtor(&fr);
    return TEST_RESULT_FAIL;
}

TEST_CASE(filereader_get_char_bulk) {
    M2H_FileReader fr;
    const char *filename_input =
        "./test/unit/fixture/input/reader-lipsum-2.txt";
    const char *filename_expected =
        "./test/unit/fixture/expected/reader-lipsum-2.txt";
    FILE *fp = fopen(filename_expected, "r");

    int actual;
    int expected;

    ASSERT_OK(M2H_filereader_ctor(&fr, filename_input), fail);

    while (true) {
        expected = fgetc(fp);
        ASSERT_OK(fr.base.get_char((M2H_Reader *)&fr, &actual), fail);
        ASSERT_EQ(actual, expected, fail);
        if (expected == EOF) {
            if (!feof(fp)) {
                goto fail;
            } else {
                break;
            }
        }
    }

    fclose(fp);
    ASSERT_OK(M2H_filereader_dtor(&fr), fail);
    return TEST_RESULT_PASS;
fail:
    fclose(fp);
    M2H_filereader_dtor(&fr);
    return TEST_RESULT_FAIL;
}

TEST_CASE(filereader_get_char_illegal_arg) {
    M2H_FileReader fr;
    const char *filename = "./test/unit/fixture/input/reader-lipsum-1.txt";

    ASSERT_OK(M2H_filereader_ctor(&fr, filename), fail);
    ASSERT_EQ(fr.base.get_char(NULL, NULL), M2H_RESULT_ILLEGAL_ARGUMENT, fail);

    ASSERT_OK(M2H_filereader_dtor(&fr), fail);
    return TEST_RESULT_PASS;
fail:
    M2H_filereader_dtor(&fr);
    return TEST_RESULT_FAIL;
}

TEST_SUITE_BEGIN(reader)

TEST_CASE_ADD(filereader_ctor_normal);
TEST_CASE_ADD(filereader_ctor_no_entry);
TEST_CASE_ADD(filereader_ctor_illegal_arg);

TEST_CASE_ADD(filereader_dtor_normal);
TEST_CASE_ADD(filereader_dtor_double);
TEST_CASE_ADD(filereader_dtor_illegal_arg);

TEST_CASE_ADD(filereader_get_char_normal);
TEST_CASE_ADD(filereader_get_char_bulk);
TEST_CASE_ADD(filereader_get_char_illegal_arg);

TEST_SUITE_END