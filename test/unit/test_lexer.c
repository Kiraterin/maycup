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

#include "debug/objprint.h"
#include "maycup/lexer.h"
#include "maycup/reader.h"
#include "maycup/result.h"
#include "maycup/writer.h"
#include "test.h"
#include <stddef.h>
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

TEST_CASE(lexer_checkpoint_normal) {
    MAYCUP_FileReader fr;
    MAYCUP_Lexer lexer;

    ASSERT_OK(
        maycup_filereader_ctor(&fr, UTEST_FIXTURE_PATH_INPUT "lexer_1.md"),
        fail);
    ASSERT_OK(maycup_lexer_ctor(&lexer, (MAYCUP_Reader *)&fr), fail);

    ASSERT_OK(maycup_lexer_checkpoint(&lexer), fail);
    ASSERT_OK(maycup_lexer_checkpoint(&lexer), fail);
    ASSERT_OK(maycup_lexer_checkpoint(&lexer), fail);
    ASSERT_EQ(lexer.checkpoint.len, 3, fail);

    ASSERT_OK(maycup_lexer_dtor(&lexer), fail);
    ASSERT_OK(maycup_filereader_dtor(&fr), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_lexer_dtor(&lexer);
    maycup_filereader_dtor(&fr);
    return TEST_RESULT_FAIL;
}

TEST_CASE(lexer_checkpoint_illegal_arg) {
    MAYCUP_FileReader fr;
    MAYCUP_Lexer lexer;

    ASSERT_OK(
        maycup_filereader_ctor(&fr, UTEST_FIXTURE_PATH_INPUT "lexer_1.md"),
        fail);
    ASSERT_OK(maycup_lexer_ctor(&lexer, (MAYCUP_Reader *)&fr), fail);

    ASSERT_EQ(maycup_lexer_checkpoint(NULL), MAYCUP_RESULT_ILLEGAL_ARGUMENT,
              fail);

    ASSERT_OK(maycup_lexer_dtor(&lexer), fail);
    ASSERT_OK(maycup_filereader_dtor(&fr), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_lexer_dtor(&lexer);
    maycup_filereader_dtor(&fr);
    return TEST_RESULT_FAIL;
}

TEST_CASE(lexer_restore_normal) {
    MAYCUP_FileReader fr;
    MAYCUP_Lexer lexer;
    const size_t TOKENS_SIZE = 10;
    MAYCUP_Token tokens[TOKENS_SIZE];

    ASSERT_OK(
        maycup_filereader_ctor(&fr, UTEST_FIXTURE_PATH_INPUT "lexer_1.md"),
        fail);
    ASSERT_OK(maycup_lexer_ctor(&lexer, (MAYCUP_Reader *)&fr), fail);

    ASSERT_OK(maycup_lexer_checkpoint(&lexer), fail);
    for (size_t i = 0; i < TOKENS_SIZE; ++i) {
        ASSERT_OK(maycup_next_token(&tokens[i], &lexer), fail);
    }
    ASSERT_OK(maycup_lexer_restore(&lexer), fail);
    for (size_t i = 0; i < TOKENS_SIZE; ++i) {
        MAYCUP_Token cur;
        ASSERT_OK(maycup_next_token(&cur, &lexer), fail);
        ASSERT_EQ(cur.type, tokens[i].type, fail);
        if (cur.type == MAYCUP_TOKENTYPE_TEXT) {
            ASSERT_EQ(strcmp(cur.text, tokens[i].text), 0, fail);
        } else if (cur.type == MAYCUP_TOKENTYPE_LITERAL) {
            ASSERT_EQ(cur.literal, tokens[i].literal, fail);
        }
        ASSERT_OK(maycup_token_dtor(&tokens[i]), fail);
        ASSERT_OK(maycup_token_dtor(&cur), fail);
    }

    ASSERT_OK(maycup_lexer_dtor(&lexer), fail);
    ASSERT_OK(maycup_filereader_dtor(&fr), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_lexer_dtor(&lexer);
    maycup_filereader_dtor(&fr);
    return TEST_RESULT_FAIL;
}

TEST_CASE(lexer_restore_illegal_arg) {
    MAYCUP_FileReader fr;
    MAYCUP_Lexer lexer;

    ASSERT_OK(
        maycup_filereader_ctor(&fr, UTEST_FIXTURE_PATH_INPUT "lexer_1.md"),
        fail);
    ASSERT_OK(maycup_lexer_ctor(&lexer, (MAYCUP_Reader *)&fr), fail);

    ASSERT_EQ(maycup_lexer_restore(NULL), MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);
    ASSERT_EQ(maycup_lexer_restore(&lexer), MAYCUP_RESULT_ILLEGAL_ARGUMENT,
              fail);

    ASSERT_OK(maycup_lexer_dtor(&lexer), fail);
    ASSERT_OK(maycup_filereader_dtor(&fr), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_lexer_dtor(&lexer);
    maycup_filereader_dtor(&fr);
    return TEST_RESULT_FAIL;
}

TEST_CASE(lexer_drop_checkpoint_normal) {
    MAYCUP_FileReader fr;
    MAYCUP_Lexer lexer;
    const size_t TOKENS_SIZE = 10;
    MAYCUP_Token tokens[TOKENS_SIZE];

    ASSERT_OK(
        maycup_filereader_ctor(&fr, UTEST_FIXTURE_PATH_INPUT "lexer_1.md"),
        fail);
    ASSERT_OK(maycup_lexer_ctor(&lexer, (MAYCUP_Reader *)&fr), fail);

    for (size_t i = 1; i <= TOKENS_SIZE; ++i) {
        ASSERT_OK(maycup_lexer_checkpoint(&lexer), fail);
        for (size_t j = 0; j < i; ++j) {
            ASSERT_OK(maycup_next_token(&tokens[j], &lexer), fail);
        }
        ASSERT_OK(maycup_lexer_restore(&lexer), fail);
        ASSERT_OK(maycup_lexer_drop_checkpoint(&lexer), fail);
        for (size_t j = 0; j < i; ++j) {
            MAYCUP_Token cur;
            ASSERT_OK(maycup_next_token(&cur, &lexer), fail);
            ASSERT_EQ(cur.type, tokens[j].type, fail);
            if (cur.type == MAYCUP_TOKENTYPE_TEXT) {
                ASSERT_EQ(strcmp(cur.text, tokens[j].text), 0, fail);
            } else if (cur.type == MAYCUP_TOKENTYPE_LITERAL) {
                ASSERT_EQ(cur.literal, tokens[j].literal, fail);
            }
            ASSERT_OK(maycup_token_dtor(&tokens[j]), fail);
            ASSERT_OK(maycup_token_dtor(&cur), fail);
        }
    }

    ASSERT_OK(maycup_lexer_dtor(&lexer), fail);
    ASSERT_OK(maycup_filereader_dtor(&fr), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_lexer_dtor(&lexer);
    maycup_filereader_dtor(&fr);
    return TEST_RESULT_FAIL;
}

TEST_CASE(lexer_drop_checkpoint_illegal_arg) {
    MAYCUP_FileReader fr;
    MAYCUP_Lexer lexer;

    ASSERT_OK(
        maycup_filereader_ctor(&fr, UTEST_FIXTURE_PATH_INPUT "lexer_1.md"),
        fail);
    ASSERT_OK(maycup_lexer_ctor(&lexer, (MAYCUP_Reader *)&fr), fail);

    ASSERT_EQ(maycup_lexer_drop_checkpoint(NULL),
              MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);
    ASSERT_EQ(maycup_lexer_drop_checkpoint(&lexer),
              MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);

    ASSERT_OK(maycup_lexer_dtor(&lexer), fail);
    ASSERT_OK(maycup_filereader_dtor(&fr), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_lexer_dtor(&lexer);
    maycup_filereader_dtor(&fr);
    return TEST_RESULT_FAIL;
}

TEST_CASE(lexer_module_common) {
    MAYCUP_FileReader fr;
    MAYCUP_StringWriter sw;
    MAYCUP_Lexer lexer;
    char *str;
    size_t token_cnt = 0;
    size_t cp_cnt = 0;

    ASSERT_OK(
        maycup_filereader_ctor(&fr, UTEST_FIXTURE_PATH_INPUT "lexer_2.md"),
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
        ++token_cnt;
        if (token_cnt == 10 && cp_cnt < 10) {
            ASSERT_OK(maycup_lexer_checkpoint(&lexer), fail);
            ++cp_cnt;
            token_cnt = 0;
        }
        if (cp_cnt == 10) {
            break;
        }
    }

    while (cp_cnt--) {
        ASSERT_OK(maycup_lexer_restore(&lexer), fail);
        ASSERT_OK(maycup_lexer_drop_checkpoint(&lexer), fail);
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
    ASSERT_OK(maycup_reader_seek(&fr, 0), fail);

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

    ASSERT_STR_EQ_FILE(sw.buf, UTEST_FIXTURE_PATH_EXPECTED "lexer_2.txt", fail);

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

TEST_CASE_ADD(lexer_checkpoint_normal);
TEST_CASE_ADD(lexer_checkpoint_illegal_arg);

TEST_CASE_ADD(lexer_restore_normal);
TEST_CASE_ADD(lexer_restore_illegal_arg);

TEST_CASE_ADD(lexer_drop_checkpoint_normal);
TEST_CASE_ADD(lexer_drop_checkpoint_illegal_arg);

TEST_CASE_ADD(lexer_module_common);

TEST_SUITE_END