/**
 * @file test_reader.c
 * @brief Test for reader
 * @date 2026-08-05
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

#include "maycup/reader.h"
#include "test.h"
#include <string.h>

#include "fixture/input/reader_str_lipsum_1.h"
#include "fixture/input/reader_str_lipsum_2.h"

TEST_CASE(filereader_ctor_normal) {
    MAYCUP_FileReader fr;
    const char *filename = UTEST_FIXTURE_PATH_INPUT "reader_lipsum_1.txt";

    ASSERT_OK(maycup_filereader_ctor(&fr, filename), fail);
    ASSERT_NEQ(fr.base.get_char, NULL, fail);
    ASSERT_NEQ(fr.base.tell, NULL, fail);
    ASSERT_NEQ(fr.base.seek, NULL, fail);
    ASSERT_NEQ(fr.fp, NULL, fail);

    ASSERT_OK(maycup_filereader_dtor(&fr), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_filereader_dtor(&fr);
    return TEST_RESULT_FAIL;
}

TEST_CASE(filereader_ctor_fopen_fail) {
    MAYCUP_FileReader fr;
    const char *filename = UTEST_FIXTURE_PATH_INPUT "reader_lipsum_1.txt";

    TEST_CTX.mock_state.m_fopen = true;

    ASSERT_EQ(maycup_filereader_ctor(&fr, filename), MAYCUP_RESULT_ERRNO, fail);

    TEST_CTX.mock_state.m_fopen = false;
    return TEST_RESULT_PASS;
fail:
    TEST_CTX.mock_state.m_fopen = false;
    maycup_filereader_dtor(&fr);
    return TEST_RESULT_FAIL;
}

TEST_CASE(filereader_ctor_illegal_arg) {
    MAYCUP_FileReader fr;
    const char *filename = UTEST_FIXTURE_PATH_INPUT "reader_lipsum_1.txt";
    ASSERT_EQ(maycup_filereader_ctor(NULL, filename),
              MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);
    ASSERT_EQ(maycup_filereader_ctor(&fr, NULL), MAYCUP_RESULT_ILLEGAL_ARGUMENT,
              fail);

    return TEST_RESULT_PASS;
fail:
    return TEST_RESULT_FAIL;
}

TEST_CASE(filereader_dtor_normal) {
    MAYCUP_FileReader fr;
    const char *filename = UTEST_FIXTURE_PATH_INPUT "reader_lipsum_1.txt";

    ASSERT_OK(maycup_filereader_ctor(&fr, filename), fail);
    ASSERT_OK(maycup_filereader_dtor(&fr), fail);
    ASSERT_EQ(fr.base.get_char, NULL, fail);
    ASSERT_EQ(fr.base.tell, NULL, fail);
    ASSERT_EQ(fr.base.seek, NULL, fail);
    ASSERT_EQ(fr.fp, NULL, fail);

    return TEST_RESULT_PASS;
fail:
    maycup_filereader_dtor(&fr);
    return TEST_RESULT_FAIL;
}

TEST_CASE(filereader_dtor_double) {
    MAYCUP_FileReader fr;
    const char *filename = UTEST_FIXTURE_PATH_INPUT "reader_lipsum_1.txt";

    ASSERT_OK(maycup_filereader_ctor(&fr, filename), fail);
    ASSERT_OK(maycup_filereader_dtor(&fr), fail);
    ASSERT_EQ(maycup_filereader_dtor(&fr), MAYCUP_RESULT_ILLEGAL_ARGUMENT,
              fail);

    return TEST_RESULT_PASS;
fail:
    maycup_filereader_dtor(&fr);
    return TEST_RESULT_FAIL;
}

TEST_CASE(filereader_dtor_fclose_fail) {
    MAYCUP_FileReader fr;
    const char *filename = UTEST_FIXTURE_PATH_INPUT "reader_lipsum_1.txt";

    MOCK_ON(fclose);

    ASSERT_OK(maycup_filereader_ctor(&fr, filename), fail);
    ASSERT_EQ(maycup_filereader_dtor(&fr), MAYCUP_RESULT_ERRNO, fail);

    MOCK_OFF(fclose);
    ASSERT_OK(maycup_filereader_dtor(&fr), fail);
    return TEST_RESULT_PASS;
fail:
    MOCK_OFF(fclose);
    maycup_filereader_dtor(&fr);
    return TEST_RESULT_FAIL;
}

TEST_CASE(filereader_dtor_illegal_arg) {
    ASSERT_EQ(maycup_filereader_dtor(NULL), MAYCUP_RESULT_ILLEGAL_ARGUMENT,
              fail);
    return TEST_RESULT_PASS;
fail:
    return TEST_RESULT_FAIL;
}

TEST_CASE(filereader_get_char_normal) {
    MAYCUP_FileReader fr;
    const char *filename_input = UTEST_FIXTURE_PATH_INPUT "reader_lipsum_1.txt";
    const char *filename_expected =
        UTEST_FIXTURE_PATH_EXPECTED "reader_lipsum_1.txt";
    FILE *fp = fopen(filename_expected, "r");

    int actual;
    int expected;
    bool swc = false;

    ASSERT_OK(maycup_filereader_ctor(&fr, filename_input), fail);

    while (true) {
        expected = fgetc(fp);
        if (swc) {
            ASSERT_OK(maycup_reader_get_char(&fr, &actual), fail);
            ASSERT_EQ(actual, expected, fail);
        } else {
            ASSERT_OK(maycup_reader_get_char(&fr, NULL), fail);
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
    ASSERT_OK(maycup_filereader_dtor(&fr), fail);
    return TEST_RESULT_PASS;
fail:
    fclose(fp);
    maycup_filereader_dtor(&fr);
    return TEST_RESULT_FAIL;
}

TEST_CASE(filereader_get_char_bulk) {
    MAYCUP_FileReader fr;
    const char *filename_input = UTEST_FIXTURE_PATH_INPUT "reader_lipsum_2.txt";
    const char *filename_expected =
        UTEST_FIXTURE_PATH_EXPECTED "reader_lipsum_2.txt";
    FILE *fp = fopen(filename_expected, "r");

    int actual;
    int expected;

    ASSERT_OK(maycup_filereader_ctor(&fr, filename_input), fail);

    while (true) {
        expected = fgetc(fp);
        ASSERT_OK(maycup_reader_get_char(&fr, &actual), fail);
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
    ASSERT_OK(maycup_filereader_dtor(&fr), fail);
    return TEST_RESULT_PASS;
fail:
    fclose(fp);
    maycup_filereader_dtor(&fr);
    return TEST_RESULT_FAIL;
}

TEST_CASE(filereader_get_char_feof_fail) {
    MAYCUP_FileReader fr;
    const char *filename = UTEST_FIXTURE_PATH_INPUT "reader_lipsum_1.txt";

    MOCK_ON(feof);

    ASSERT_OK(maycup_filereader_ctor(&fr, filename), fail);
    for (size_t i = 1; i <= 1610; ++i) {
        ASSERT_OK(maycup_reader_get_char(&fr, NULL), fail);
    }

    ASSERT_EQ(maycup_reader_get_char(&fr, NULL), MAYCUP_RESULT_ERRNO, fail);

    MOCK_OFF(feof);
    ASSERT_OK(maycup_filereader_dtor(&fr), fail);
    return TEST_RESULT_PASS;
fail:
    MOCK_OFF(feof);
    maycup_filereader_dtor(&fr);
    return TEST_RESULT_FAIL;
}

TEST_CASE(filereader_get_char_illegal_arg) {
    MAYCUP_FileReader fr;
    const char *filename = UTEST_FIXTURE_PATH_INPUT "reader_lipsum_1.txt";

    ASSERT_OK(maycup_filereader_ctor(&fr, filename), fail);
    ASSERT_EQ(maycup_reader_get_char(NULL, NULL),
              MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);
    ASSERT_EQ(fr.base.get_char(NULL, NULL), MAYCUP_RESULT_ILLEGAL_ARGUMENT,
              fail);

    ASSERT_OK(maycup_filereader_dtor(&fr), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_filereader_dtor(&fr);
    return TEST_RESULT_FAIL;
}

TEST_CASE(filereader_tell_normal) {
    MAYCUP_FileReader fr;
    const char *filename = UTEST_FIXTURE_PATH_INPUT "reader_lipsum_1.txt";
    long cur;

    ASSERT_OK(maycup_filereader_ctor(&fr, filename), fail);
    ASSERT_OK(maycup_reader_tell(&fr, &cur), fail);
    ASSERT_EQ(cur, 0, fail);
    ASSERT_OK(maycup_reader_get_char(&fr, NULL), fail);
    ASSERT_OK(maycup_reader_tell(&fr, &cur), fail);
    ASSERT_EQ(cur, 1, fail);
    for (size_t i = 1; i <= 10; ++i) {
        ASSERT_OK(maycup_reader_get_char(&fr, NULL), fail);
    }
    ASSERT_OK(maycup_reader_tell(&fr, &cur), fail);
    ASSERT_EQ(cur, 11, fail);

    ASSERT_OK(maycup_filereader_dtor(&fr), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_filereader_dtor(&fr);
    return TEST_RESULT_FAIL;
}

TEST_CASE(filereader_tell_ftell_fail) {
    MAYCUP_FileReader fr;
    const char *filename = UTEST_FIXTURE_PATH_INPUT "reader_lipsum_1.txt";
    long cur;

    MOCK_ON(ftell);

    ASSERT_OK(maycup_filereader_ctor(&fr, filename), fail);
    ASSERT_EQ(maycup_reader_tell(&fr, &cur), MAYCUP_RESULT_ERRNO, fail);

    MOCK_OFF(ftell);
    ASSERT_OK(maycup_filereader_dtor(&fr), fail);
    return TEST_RESULT_PASS;
fail:
    MOCK_OFF(ftell);
    maycup_filereader_dtor(&fr);
    return TEST_RESULT_FAIL;
}

TEST_CASE(filereader_tell_illegal_arg) {
    MAYCUP_FileReader fr;
    const char *filename = UTEST_FIXTURE_PATH_INPUT "reader_lipsum_1.txt";
    long cur;

    ASSERT_OK(maycup_filereader_ctor(&fr, filename), fail);
    ASSERT_EQ(maycup_reader_tell(NULL, &cur), MAYCUP_RESULT_ILLEGAL_ARGUMENT,
              fail);
    ASSERT_EQ(maycup_reader_tell(&fr, NULL), MAYCUP_RESULT_ILLEGAL_ARGUMENT,
              fail);
    ASSERT_EQ(fr.base.tell(NULL, &cur), MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);
    ASSERT_EQ(fr.base.tell((MAYCUP_Reader *)&fr, NULL),
              MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);

    ASSERT_OK(maycup_filereader_dtor(&fr), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_filereader_dtor(&fr);
    return TEST_RESULT_FAIL;
}

TEST_CASE(filereader_seek_normal) {
    MAYCUP_FileReader fr;
    const char *filename = UTEST_FIXTURE_PATH_INPUT "reader_lipsum_1.txt";
    int cur;

    ASSERT_OK(maycup_filereader_ctor(&fr, filename), fail);
    ASSERT_OK(maycup_reader_seek(&fr, 384), fail);
    ASSERT_OK(maycup_reader_get_char(&fr, &cur), fail);
    ASSERT_EQ(cur, 'u', fail);
    ASSERT_OK(maycup_reader_seek(&fr, 0), fail);
    ASSERT_OK(maycup_reader_get_char(&fr, &cur), fail);
    ASSERT_EQ(cur, 'L', fail);

    ASSERT_OK(maycup_filereader_dtor(&fr), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_filereader_dtor(&fr);
    return TEST_RESULT_FAIL;
}

TEST_CASE(filereader_seek_fseek_fail) {
    MAYCUP_FileReader fr;
    const char *filename = UTEST_FIXTURE_PATH_INPUT "reader_lipsum_1.txt";

    MOCK_ON(fseek);

    ASSERT_OK(maycup_filereader_ctor(&fr, filename), fail);
    ASSERT_EQ(maycup_reader_seek(&fr, 384), MAYCUP_RESULT_ERRNO, fail);

    MOCK_OFF(fseek);
    ASSERT_OK(maycup_filereader_dtor(&fr), fail);
    return TEST_RESULT_PASS;
fail:
    MOCK_OFF(fseek);
    maycup_filereader_dtor(&fr);
    return TEST_RESULT_FAIL;
}

TEST_CASE(filereader_seek_illegal_arg) {
    MAYCUP_FileReader fr;
    const char *filename = UTEST_FIXTURE_PATH_INPUT "reader_lipsum_1.txt";

    ASSERT_OK(maycup_filereader_ctor(&fr, filename), fail);
    ASSERT_EQ(maycup_reader_seek(NULL, 0), MAYCUP_RESULT_ILLEGAL_ARGUMENT,
              fail);
    ASSERT_EQ(fr.base.seek(NULL, 0), MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);

    ASSERT_OK(maycup_filereader_dtor(&fr), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_filereader_dtor(&fr);
    return TEST_RESULT_FAIL;
}

TEST_CASE(stringreader_ctor_normal) {
    MAYCUP_StringReader sr;
    const char *stringname = reader_str_lipsum_1;
    const size_t stringlen = reader_str_lipsum_1_len;

    ASSERT_OK(maycup_stringreader_ctor(&sr, stringname, stringlen), fail);
    ASSERT_NEQ(sr.base.get_char, NULL, fail);
    ASSERT_NEQ(sr.base.tell, NULL, fail);
    ASSERT_NEQ(sr.base.seek, NULL, fail);
    ASSERT_NEQ(sr.begin, NULL, fail);
    ASSERT_NEQ(sr.cur, NULL, fail);
    ASSERT_NEQ(sr.end, NULL, fail);

    ASSERT_OK(maycup_stringreader_dtor(&sr), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_stringreader_dtor(&sr);
    return TEST_RESULT_FAIL;
}

TEST_CASE(stringreader_ctor_illegal_arg) {
    MAYCUP_StringReader sr;
    const char *stringname = reader_str_lipsum_1;
    const size_t stringlen = reader_str_lipsum_1_len;

    ASSERT_EQ(maycup_stringreader_ctor(NULL, stringname, stringlen),
              MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);
    ASSERT_EQ(maycup_stringreader_ctor(&sr, NULL, stringlen),
              MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);

    return TEST_RESULT_PASS;
fail:
    return TEST_RESULT_FAIL;
}

TEST_CASE(stringreader_dtor_normal) {
    MAYCUP_StringReader sr;
    const char *stringname = reader_str_lipsum_1;
    const size_t stringlen = reader_str_lipsum_1_len;

    ASSERT_OK(maycup_stringreader_ctor(&sr, stringname, stringlen), fail);
    ASSERT_OK(maycup_stringreader_dtor(&sr), fail);
    ASSERT_EQ(sr.base.get_char, NULL, fail);
    ASSERT_EQ(sr.base.tell, NULL, fail);
    ASSERT_EQ(sr.base.seek, NULL, fail);
    ASSERT_EQ(sr.begin, NULL, fail);
    ASSERT_EQ(sr.cur, NULL, fail);
    ASSERT_EQ(sr.end, NULL, fail);

    return TEST_RESULT_PASS;
fail:
    maycup_stringreader_dtor(&sr);
    return TEST_RESULT_FAIL;
}

TEST_CASE(stringreader_dtor_double) {
    MAYCUP_StringReader sr;
    const char *stringname = reader_str_lipsum_1;
    const size_t stringlen = reader_str_lipsum_1_len;

    ASSERT_OK(maycup_stringreader_ctor(&sr, stringname, stringlen), fail);
    ASSERT_OK(maycup_stringreader_dtor(&sr), fail);
    ASSERT_EQ(maycup_stringreader_dtor(&sr), MAYCUP_RESULT_ILLEGAL_ARGUMENT,
              fail);

    return TEST_RESULT_PASS;
fail:
    maycup_stringreader_dtor(&sr);
    return TEST_RESULT_FAIL;
}

TEST_CASE(stringreader_dtor_illegal_arg) {
    ASSERT_EQ(maycup_stringreader_dtor(NULL), MAYCUP_RESULT_ILLEGAL_ARGUMENT,
              fail);
    return TEST_RESULT_PASS;
fail:
    return TEST_RESULT_FAIL;
}

TEST_CASE(stringreader_get_char_normal) {
    MAYCUP_StringReader sr;
    const char *stringname_input = reader_str_lipsum_1;
    const char *stringname_expected = reader_str_lipsum_1;
    const size_t stringlen = reader_str_lipsum_1_len;

    int actual;
    int expected;
    bool swc = false;
    const char *ptr = stringname_expected;

    ASSERT_OK(maycup_stringreader_ctor(&sr, stringname_input, stringlen), fail);

    while (true) {
        expected = *ptr;
        ++ptr;
        if (swc) {
            ASSERT_OK(maycup_reader_get_char(&sr, &actual), fail);
            ASSERT_EQ(actual, expected, fail);
        } else {
            ASSERT_OK(maycup_reader_get_char(&sr, NULL), fail);
        }
        if (expected == '\0') {
            break;
        }
        swc = !swc;
    }

    ASSERT_OK(maycup_stringreader_dtor(&sr), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_stringreader_dtor(&sr);
    return TEST_RESULT_FAIL;
}

TEST_CASE(stringreader_get_char_bulk) {
    MAYCUP_StringReader sr;
    const char *stringname_input = reader_str_lipsum_2;
    const char *stringname_expected = reader_str_lipsum_2;
    const size_t stringlen = reader_str_lipsum_2_len;

    int actual;
    int expected;
    bool swc = false;
    const char *ptr = stringname_expected;

    ASSERT_OK(maycup_stringreader_ctor(&sr, stringname_input, stringlen), fail);

    while (true) {
        expected = *ptr;
        ++ptr;
        if (swc) {
            ASSERT_OK(maycup_reader_get_char(&sr, &actual), fail);
            ASSERT_EQ(actual, expected, fail);
        } else {
            ASSERT_OK(maycup_reader_get_char(&sr, NULL), fail);
        }
        if (expected == '\0') {
            break;
        }
        swc = !swc;
    }

    ASSERT_OK(maycup_stringreader_dtor(&sr), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_stringreader_dtor(&sr);
    return TEST_RESULT_FAIL;
}

TEST_CASE(stringreader_get_char_eof) {
    MAYCUP_StringReader sr;
    const char *str = "123";
    int res;

    ASSERT_OK(maycup_stringreader_ctor(&sr, str, 3), fail);
    ASSERT_OK(maycup_reader_get_char(&sr, &res), fail);
    ASSERT_NEQ(res, EOF, fail);
    ASSERT_OK(maycup_reader_get_char(&sr, &res), fail);
    ASSERT_NEQ(res, EOF, fail);
    ASSERT_OK(maycup_reader_get_char(&sr, &res), fail);
    ASSERT_NEQ(res, EOF, fail);
    for (int i = 1; i <= 10; ++i) {
        ASSERT_OK(maycup_reader_get_char(&sr, &res), fail);
        ASSERT_EQ(res, EOF, fail);
    }

    ASSERT_OK(maycup_stringreader_dtor(&sr), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_stringreader_dtor(&sr);
    return TEST_RESULT_FAIL;
}

TEST_CASE(stringreader_get_char_illegal_arg) {
    MAYCUP_StringReader sr;
    const char *stringname = reader_str_lipsum_1;
    const size_t stringlen = reader_str_lipsum_1_len;

    ASSERT_OK(maycup_stringreader_ctor(&sr, stringname, stringlen), fail);
    ASSERT_EQ(maycup_reader_get_char(NULL, NULL),
              MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);
    ASSERT_EQ(sr.base.get_char(NULL, NULL), MAYCUP_RESULT_ILLEGAL_ARGUMENT,
              fail);

    ASSERT_OK(maycup_stringreader_dtor(&sr), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_stringreader_dtor(&sr);
    return TEST_RESULT_FAIL;
}

TEST_CASE(stringreader_tell_normal) {
    MAYCUP_StringReader sr;
    const char *stringname = reader_str_lipsum_1;
    const size_t stringlen = reader_str_lipsum_1_len;
    long cur;

    ASSERT_OK(maycup_stringreader_ctor(&sr, stringname, stringlen), fail);
    ASSERT_OK(maycup_reader_tell(&sr, &cur), fail);
    ASSERT_EQ(cur, 0, fail);
    ASSERT_OK(maycup_reader_get_char(&sr, NULL), fail);
    ASSERT_OK(maycup_reader_tell(&sr, &cur), fail);
    ASSERT_EQ(cur, 1, fail);
    for (size_t i = 1; i <= 10; ++i) {
        ASSERT_OK(maycup_reader_get_char(&sr, NULL), fail);
    }
    ASSERT_OK(maycup_reader_tell(&sr, &cur), fail);
    ASSERT_EQ(cur, 11, fail);

    ASSERT_OK(maycup_stringreader_dtor(&sr), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_stringreader_dtor(&sr);
    return TEST_RESULT_FAIL;
}

TEST_CASE(stringreader_tell_illegal_arg) {
    MAYCUP_StringReader sr;
    const char *stringname = reader_str_lipsum_1;
    const size_t stringlen = reader_str_lipsum_1_len;
    long cur;

    ASSERT_OK(maycup_stringreader_ctor(&sr, stringname, stringlen), fail);
    ASSERT_EQ(maycup_reader_tell(NULL, &cur), MAYCUP_RESULT_ILLEGAL_ARGUMENT,
              fail);
    ASSERT_EQ(maycup_reader_tell(&sr, NULL), MAYCUP_RESULT_ILLEGAL_ARGUMENT,
              fail);
    ASSERT_EQ(sr.base.tell(NULL, &cur), MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);
    ASSERT_EQ(sr.base.tell((MAYCUP_Reader *)&sr, NULL),
              MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);

    ASSERT_OK(maycup_stringreader_dtor(&sr), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_stringreader_dtor(&sr);
    return TEST_RESULT_FAIL;
}

TEST_CASE(stringreader_seek_normal) {
    MAYCUP_StringReader sr;
    const char *stringname = reader_str_lipsum_1;
    const size_t stringlen = reader_str_lipsum_1_len;
    int cur;

    ASSERT_OK(maycup_stringreader_ctor(&sr, stringname, stringlen), fail);
    ASSERT_OK(maycup_reader_seek(&sr, 384), fail);
    ASSERT_OK(maycup_reader_get_char(&sr, &cur), fail);
    ASSERT_EQ(cur, 'u', fail);
    ASSERT_OK(maycup_reader_seek(&sr, 0), fail);
    ASSERT_OK(maycup_reader_get_char(&sr, &cur), fail);
    ASSERT_EQ(cur, 'L', fail);

    ASSERT_OK(maycup_stringreader_dtor(&sr), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_stringreader_dtor(&sr);
    return TEST_RESULT_FAIL;
}

TEST_CASE(stringreader_seek_illegal_arg) {
    MAYCUP_StringReader sr;
    const char *stringname = reader_str_lipsum_1;
    const size_t stringlen = reader_str_lipsum_1_len;

    ASSERT_OK(maycup_stringreader_ctor(&sr, stringname, stringlen), fail);
    ASSERT_EQ(maycup_reader_seek(NULL, 0), MAYCUP_RESULT_ILLEGAL_ARGUMENT,
              fail);
    ASSERT_EQ(maycup_reader_seek(&sr, -1), MAYCUP_RESULT_ILLEGAL_ARGUMENT,
              fail);
    ASSERT_EQ(maycup_reader_seek(&sr, stringlen + 1),
              MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);
    ASSERT_EQ(sr.base.seek(NULL, 0), MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);
    ASSERT_EQ(sr.base.seek((MAYCUP_Reader *)&sr, -1),
              MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);
    ASSERT_EQ(sr.base.seek((MAYCUP_Reader *)&sr, stringlen + 1),
              MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);

    ASSERT_OK(maycup_stringreader_dtor(&sr), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_stringreader_dtor(&sr);
    return TEST_RESULT_FAIL;
}

TEST_CASE(filereader_module_common) {
    MAYCUP_FileReader fr;
    FILE *fp;
    const char *input_filename = UTEST_FIXTURE_PATH_INPUT "reader_lipsum_1.txt";
    const char *expected_filename =
        UTEST_FIXTURE_PATH_EXPECTED "reader_lipsum_1.txt";
    const size_t threshold = 5;
    size_t accumulate = 0;
    long input_cp = 0;
    long expected_cp = 0;
    int input_cur;
    int expected_cur;

    ASSERT_OK(maycup_filereader_ctor(&fr, input_filename), fail);
    fp = fopen(expected_filename, "r");

    while (true) {
        if (accumulate == threshold) {
            accumulate = 0;
            ASSERT_OK(maycup_reader_seek(&fr, input_cp), fail);
            fseek(fp, expected_cp, SEEK_SET);
        }
        ASSERT_OK(maycup_reader_get_char(&fr, &input_cur), fail);
        expected_cur = getc(fp);
        ASSERT_EQ(input_cur, expected_cur, fail);

        ASSERT_OK(maycup_reader_tell(&fr, &input_cp), fail);
        expected_cp = ftell(fp);
        ASSERT_EQ(input_cp, expected_cp, fail);

        if (expected_cur == EOF && feof(fp)) {
            break;
        }

        ++accumulate;
    }

    ASSERT_OK(maycup_filereader_dtor(&fr), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_filereader_dtor(&fr);
    return TEST_RESULT_FAIL;
}

TEST_CASE(stringreader_module_common) {
    MAYCUP_StringReader sr;
    FILE *fp;
    const char *input_stringname = reader_str_lipsum_1;
    const size_t input_stringlen = reader_str_lipsum_1_len;
    const char *expected_filename =
        UTEST_FIXTURE_PATH_EXPECTED "reader_lipsum_1.txt";
    const size_t threshold = 5;
    size_t accumulate = 0;
    long input_cp = 0;
    long expected_cp = 0;
    int input_cur;
    int expected_cur;

    ASSERT_OK(maycup_stringreader_ctor(&sr, input_stringname, input_stringlen),
              fail);
    fp = fopen(expected_filename, "r");

    while (true) {
        if (accumulate == threshold) {
            accumulate = 0;
            ASSERT_OK(maycup_reader_seek(&sr, input_cp), fail);
            fseek(fp, expected_cp, SEEK_SET);
        }
        ASSERT_OK(maycup_reader_get_char(&sr, &input_cur), fail);
        expected_cur = getc(fp);
        ASSERT_EQ(input_cur, expected_cur, fail);

        ASSERT_OK(maycup_reader_tell(&sr, &input_cp), fail);
        expected_cp = ftell(fp);
        ASSERT_EQ(input_cp, expected_cp, fail);

        if (expected_cur == EOF && feof(fp)) {
            break;
        }

        ++accumulate;
    }

    ASSERT_OK(maycup_stringreader_dtor(&sr), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_stringreader_dtor(&sr);
    return TEST_RESULT_FAIL;
}

TEST_SUITE_BEGIN(reader)

TEST_CASE_ADD(filereader_ctor_normal);
TEST_CASE_ADD(filereader_ctor_fopen_fail);
TEST_CASE_ADD(filereader_ctor_illegal_arg);

TEST_CASE_ADD(filereader_dtor_normal);
TEST_CASE_ADD(filereader_dtor_double);
TEST_CASE_ADD(filereader_dtor_fclose_fail);
TEST_CASE_ADD(filereader_dtor_illegal_arg);

TEST_CASE_ADD(filereader_get_char_normal);
TEST_CASE_ADD(filereader_get_char_bulk);
TEST_CASE_ADD(filereader_get_char_feof_fail);
TEST_CASE_ADD(filereader_get_char_illegal_arg);

TEST_CASE_ADD(filereader_tell_normal);
TEST_CASE_ADD(filereader_tell_ftell_fail);
TEST_CASE_ADD(filereader_tell_illegal_arg);

TEST_CASE_ADD(filereader_seek_normal);
TEST_CASE_ADD(filereader_seek_fseek_fail);
TEST_CASE_ADD(filereader_seek_illegal_arg);

TEST_CASE_ADD(stringreader_ctor_normal);
TEST_CASE_ADD(stringreader_ctor_illegal_arg);

TEST_CASE_ADD(stringreader_dtor_normal);
TEST_CASE_ADD(stringreader_dtor_double);
TEST_CASE_ADD(stringreader_dtor_illegal_arg);

TEST_CASE_ADD(stringreader_get_char_normal);
TEST_CASE_ADD(stringreader_get_char_bulk);
TEST_CASE_ADD(stringreader_get_char_eof);
TEST_CASE_ADD(stringreader_get_char_illegal_arg);

TEST_CASE_ADD(stringreader_tell_normal);
TEST_CASE_ADD(stringreader_tell_illegal_arg);

TEST_CASE_ADD(stringreader_seek_normal);
TEST_CASE_ADD(stringreader_seek_illegal_arg);

TEST_CASE_ADD(filereader_module_common);
TEST_CASE_ADD(stringreader_module_common);

TEST_SUITE_END