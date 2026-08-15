/**
 * @file test_lexer.c
 * @brief Test for lexer
 * @date 2026-08-04
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

#include "maycup/base/result.h"
#include "maycup/core/lexer.h"
#include "maycup/debug/objprint.h"
#include "maycup/io/reader.h"
#include "maycup/io/writer.h"
#include "test.h"
#include <stdlib.h>
#include <string.h>

TEST_CASE(token_dtor_normal) {
    MAYCUP_Token token =
        (MAYCUP_Token){.type = MAYCUP_TOKENTYPE_TEXT,
                       .text = (char *)malloc(12 * sizeof(char))};

    ASSERT_OK(maycup_token_dtor(&token), fail);
    ASSERT_EQ(token.type, MAYCUP_TOKENTYPE_NONE, fail);
    ASSERT_EQ(token.text, NULL, fail);

    return TEST_RESULT_PASS;
fail:
    maycup_token_dtor(&token);
    return TEST_RESULT_FAIL;
}

TEST_CASE(token_dtor_double) {
    MAYCUP_Token token =
        (MAYCUP_Token){.type = MAYCUP_TOKENTYPE_TEXT,
                       .text = (char *)malloc(12 * sizeof(char))};

    ASSERT_OK(maycup_token_dtor(&token), fail);
    ASSERT_OK(maycup_token_dtor(&token), fail);

    return TEST_RESULT_PASS;
fail:
    maycup_token_dtor(&token);
    return TEST_RESULT_FAIL;
}

TEST_CASE(token_dtor_illegal_arg) {
    ASSERT_EQ(maycup_token_dtor(NULL), MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);
    return TEST_RESULT_PASS;
fail:
    return TEST_RESULT_FAIL;
}

TEST_CASE(token_duplicate_normal) {
    MAYCUP_Token src =
        (MAYCUP_Token){.type = MAYCUP_TOKENTYPE_TEXT,
                       .text = (char *)malloc(12 * sizeof(char))};
    strcpy(src.text, "123");
    MAYCUP_Token dest = (MAYCUP_Token){.type = MAYCUP_TOKENTYPE_NONE};

    ASSERT_OK(maycup_token_duplicate(&dest, &src), fail);
    ASSERT_NEQ(dest.text, src.text, fail);
    ASSERT_EQ(src.type, MAYCUP_TOKENTYPE_TEXT, fail);
    ASSERT_EQ(dest.type, src.type, fail);

    ASSERT_OK(maycup_token_dtor(&src), fail);
    ASSERT_OK(maycup_token_dtor(&dest), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_token_dtor(&src);
    maycup_token_dtor(&dest);
    return TEST_RESULT_FAIL;
}

TEST_CASE(token_duplicate_malloc_fail) {
    MAYCUP_Token src =
        (MAYCUP_Token){.type = MAYCUP_TOKENTYPE_TEXT,
                       .text = (char *)malloc(12 * sizeof(char))};
    strcpy(src.text, "123");
    MAYCUP_Token dest = (MAYCUP_Token){.type = MAYCUP_TOKENTYPE_NONE};

    MOCK_ON(malloc);

    ASSERT_EQ(maycup_token_duplicate(&dest, &src), MAYCUP_RESULT_MALLOC_FAIL,
              fail);

    MOCK_OFF(malloc);
    ASSERT_OK(maycup_token_dtor(&src), fail);
    return TEST_RESULT_PASS;
fail:
    MOCK_OFF(malloc);
    maycup_token_dtor(&src);
    return TEST_RESULT_FAIL;
}

TEST_CASE(token_duplicate_illegal_arg) {
    MAYCUP_Token src =
        (MAYCUP_Token){.type = MAYCUP_TOKENTYPE_TEXT,
                       .text = (char *)malloc(12 * sizeof(char))};
    strcpy(src.text, "123");
    MAYCUP_Token dest = (MAYCUP_Token){.type = MAYCUP_TOKENTYPE_NONE};

    ASSERT_EQ(maycup_token_duplicate(NULL, &src),
              MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);
    ASSERT_EQ(maycup_token_duplicate(&dest, NULL),
              MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);
    dest.type = MAYCUP_TOKENTYPE_TEXT;
    ASSERT_EQ(maycup_token_duplicate(&dest, &src),
              MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);

    ASSERT_OK(maycup_token_dtor(&src), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_token_dtor(&src);
    return TEST_RESULT_FAIL;
}

TEST_CASE(lexer_ctor_normal) {
    MAYCUP_FileReader fr;
    MAYCUP_Lexer lexer;

    ASSERT_OK(
        maycup_filereader_ctor(&fr, UTEST_FIXTURE_PATH_INPUT "lexer_1.md"),
        fail);
    ASSERT_OK(maycup_lexer_ctor(&lexer, (MAYCUP_Reader *)&fr), fail);

    ASSERT_NEQ(lexer.checkpoint.ptr, NULL, fail);
    ASSERT_EQ(lexer.reader, (MAYCUP_Reader *)&fr, fail);

    ASSERT_OK(maycup_lexer_dtor(&lexer), fail);
    ASSERT_OK(maycup_filereader_dtor(&fr), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_lexer_dtor(&lexer);
    maycup_filereader_dtor(&fr);
    return TEST_RESULT_FAIL;
}

TEST_CASE(lexer_ctor_illegal_arg) {
    MAYCUP_FileReader fr;
    MAYCUP_Lexer lexer;

    ASSERT_OK(
        maycup_filereader_ctor(&fr, UTEST_FIXTURE_PATH_INPUT "lexer_1.md"),
        fail);

    ASSERT_EQ(maycup_lexer_ctor(NULL, (MAYCUP_Reader *)&fr),
              MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);
    ASSERT_EQ(maycup_lexer_ctor(&lexer, NULL), MAYCUP_RESULT_ILLEGAL_ARGUMENT,
              fail);

    ASSERT_OK(maycup_filereader_dtor(&fr), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_lexer_dtor(&lexer);
    maycup_filereader_dtor(&fr);
    return TEST_RESULT_FAIL;
}

TEST_CASE(lexer_dtor_normal) {
    MAYCUP_FileReader fr;
    MAYCUP_Lexer lexer;

    ASSERT_OK(
        maycup_filereader_ctor(&fr, UTEST_FIXTURE_PATH_INPUT "lexer_1.md"),
        fail);
    ASSERT_OK(maycup_lexer_ctor(&lexer, (MAYCUP_Reader *)&fr), fail);
    ASSERT_OK(maycup_lexer_dtor(&lexer), fail);
    ASSERT_OK(maycup_filereader_dtor(&fr), fail);

    ASSERT_EQ(lexer.reader, NULL, fail);
    ASSERT_EQ(lexer.checkpoint.ptr, NULL, fail);

    return TEST_RESULT_PASS;
fail:
    maycup_lexer_dtor(&lexer);
    maycup_filereader_dtor(&fr);
    return TEST_RESULT_FAIL;
}

TEST_CASE(lexer_dtor_double) {
    MAYCUP_FileReader fr;
    MAYCUP_Lexer lexer;

    ASSERT_OK(
        maycup_filereader_ctor(&fr, UTEST_FIXTURE_PATH_INPUT "lexer_1.md"),
        fail);
    ASSERT_OK(maycup_lexer_ctor(&lexer, (MAYCUP_Reader *)&fr), fail);
    ASSERT_OK(maycup_lexer_dtor(&lexer), fail);
    ASSERT_OK(maycup_filereader_dtor(&fr), fail);

    ASSERT_EQ(maycup_lexer_dtor(&lexer), MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);

    return TEST_RESULT_PASS;
fail:
    maycup_lexer_dtor(&lexer);
    maycup_filereader_dtor(&fr);
    return TEST_RESULT_FAIL;
}

TEST_CASE(lexer_dtor_illegal_arg) {
    MAYCUP_FileReader fr;
    MAYCUP_Lexer lexer;

    ASSERT_OK(
        maycup_filereader_ctor(&fr, UTEST_FIXTURE_PATH_INPUT "lexer_1.md"),
        fail);
    ASSERT_OK(maycup_lexer_ctor(&lexer, (MAYCUP_Reader *)&fr), fail);

    ASSERT_EQ(maycup_lexer_dtor(NULL), MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);

    ASSERT_OK(maycup_lexer_dtor(&lexer), fail);
    ASSERT_OK(maycup_filereader_dtor(&fr), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_lexer_dtor(&lexer);
    maycup_filereader_dtor(&fr);
    return TEST_RESULT_FAIL;
}

TEST_CASE(lexer_next_token_normal) {
    MAYCUP_FileReader fr;
    MAYCUP_StringWriter sw;
    MAYCUP_Lexer lexer;
    char *str;

    ASSERT_OK(
        maycup_filereader_ctor(&fr, UTEST_FIXTURE_PATH_INPUT "lexer_1.md"),
        fail);
    ASSERT_OK(maycup_stringwriter_ctor_flexible(
                  &sw, MAYCUP_DEFAULT_STRWRITER_FLEXBUF_SIZE),
              fail);
    ASSERT_OK(maycup_lexer_ctor(&lexer, (MAYCUP_Reader *)&fr), fail);

    MAYCUP_Token cur = (MAYCUP_Token){.type = MAYCUP_TOKENTYPE_NONE};

    while (cur.type != MAYCUP_TOKENTYPE_EOF) {
        ASSERT_OK(maycup_token_dtor(&cur), fail);
        cur = (MAYCUP_Token){.type = MAYCUP_TOKENTYPE_NONE};
        ASSERT_OK(maycup_next_token(&cur, &lexer), fail);
        ASSERT_NEQ(cur.type, MAYCUP_TOKENTYPE_NONE, fail);
        if (cur.type == MAYCUP_TOKENTYPE_TEXT) {
            ASSERT_NEQ(cur.text, NULL, fail);
        }
        str = maycup_token_tostr(&cur);
        maycup_writer_printf(&sw, "%s\n", str);
        free(str);
        str = NULL;
    }
    ASSERT_OK(maycup_token_dtor(&cur), fail);

    ASSERT_STR_EQ_FILE(sw.buf, UTEST_FIXTURE_PATH_EXPECTED "lexer_1.txt", fail);

    ASSERT_OK(maycup_lexer_dtor(&lexer), fail);
    ASSERT_OK(maycup_filereader_dtor(&fr), fail);
    ASSERT_OK(maycup_stringwriter_dtor(&sw, NULL), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_lexer_dtor(&lexer);
    maycup_filereader_dtor(&fr);
    maycup_stringwriter_dtor(&sw, NULL);
    return TEST_RESULT_FAIL;
}

TEST_CASE(lexer_next_token_illegal_arg) {
    MAYCUP_FileReader fr;
    MAYCUP_Lexer lexer;

    ASSERT_OK(
        maycup_filereader_ctor(&fr, UTEST_FIXTURE_PATH_INPUT "lexer_1.md"),
        fail);
    ASSERT_OK(maycup_lexer_ctor(&lexer, (MAYCUP_Reader *)&fr), fail);

    MAYCUP_Token cur = (MAYCUP_Token){.type = MAYCUP_TOKENTYPE_NONE};

    ASSERT_EQ(maycup_next_token(NULL, &lexer), MAYCUP_RESULT_ILLEGAL_ARGUMENT,
              fail);
    ASSERT_EQ(maycup_next_token(&cur, NULL), MAYCUP_RESULT_ILLEGAL_ARGUMENT,
              fail);
    cur = (MAYCUP_Token){.type = MAYCUP_TOKENTYPE_TEXT};
    ASSERT_EQ(maycup_next_token(&cur, &lexer), MAYCUP_RESULT_ILLEGAL_ARGUMENT,
              fail);

    ASSERT_OK(maycup_lexer_dtor(&lexer), fail);
    ASSERT_OK(maycup_filereader_dtor(&fr), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_lexer_dtor(&lexer);
    maycup_filereader_dtor(&fr);
    return TEST_RESULT_FAIL;
}

TEST_SUITE_BEGIN(lexer)

TEST_CASE_ADD(token_dtor_normal);
TEST_CASE_ADD(token_dtor_double);
TEST_CASE_ADD(token_dtor_illegal_arg);

TEST_CASE_ADD(token_duplicate_normal);
TEST_CASE_ADD(token_duplicate_malloc_fail);
TEST_CASE_ADD(token_duplicate_illegal_arg);

TEST_CASE_ADD(lexer_ctor_normal);
TEST_CASE_ADD(lexer_ctor_illegal_arg);

TEST_CASE_ADD(lexer_dtor_normal);
TEST_CASE_ADD(lexer_dtor_double);
TEST_CASE_ADD(lexer_dtor_illegal_arg);

TEST_CASE_ADD(lexer_next_token_normal);
TEST_CASE_ADD(lexer_next_token_illegal_arg);

// TODO: test cases for specific cases during tokenization (newline text eof
// etc.)

TEST_SUITE_END