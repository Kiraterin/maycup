/**
 * @file test_parser.c
 * @brief Test for parser
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

#include "maycup/base/result.h"
#include "maycup/core/lexer.h"
#include "maycup/core/parser.h"
#include "maycup/debug/objprint.h"
#include "maycup/io/reader.h"
#include "maycup/io/writer.h"
#include "test.h"
#include <stdlib.h>
#include <string.h>

TEST_CASE(parser_ctor_normal) {
    MAYCUP_Parser parser;

    ASSERT_OK(maycup_parser_ctor(&parser), fail);
    ASSERT_NEQ(parser.ast.data, NULL, fail);
    ASSERT_EQ(parser.cur_token.type, MAYCUP_TOKENTYPE_NONE, fail);

    ASSERT_OK(maycup_parser_dtor(&parser), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_parser_dtor(&parser);
    return TEST_RESULT_FAIL;
}

TEST_CASE(parser_ctor_illegal_arg) {
    ASSERT_EQ(maycup_parser_ctor(NULL), MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);
    return TEST_RESULT_PASS;
fail:
    return TEST_RESULT_FAIL;
}

TEST_CASE(parser_dtor_normal) {
    MAYCUP_Parser parser;

    ASSERT_OK(maycup_parser_ctor(&parser), fail);
    ASSERT_OK(maycup_parser_dtor(&parser), fail);

    return TEST_RESULT_PASS;
fail:
    maycup_parser_dtor(&parser);
    return TEST_RESULT_FAIL;
}

TEST_CASE(parser_dtor_double) {
    MAYCUP_Parser parser;

    ASSERT_OK(maycup_parser_ctor(&parser), fail);
    ASSERT_OK(maycup_parser_dtor(&parser), fail);
    ASSERT_EQ(maycup_parser_dtor(&parser), MAYCUP_RESULT_ILLEGAL_ARGUMENT,
              fail);

    return TEST_RESULT_PASS;
fail:
    maycup_parser_dtor(&parser);
    return TEST_RESULT_FAIL;
}

TEST_CASE(parser_dtor_illegal_arg) {
    ASSERT_EQ(maycup_parser_dtor(NULL), MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);
    return TEST_RESULT_PASS;
fail:
    return TEST_RESULT_FAIL;
}

TEST_CASE(parser_parse_normal) {
    MAYCUP_FileReader fr;
    MAYCUP_Lexer lexer;
    MAYCUP_Parser parser;
    char *ast_str = NULL;

    ASSERT_OK(
        maycup_filereader_ctor(&fr, UTEST_FIXTURE_PATH_INPUT "parser_1.md"),
        fail);
    ASSERT_OK(maycup_lexer_ctor(&lexer, (MAYCUP_Reader *)&fr), fail);
    ASSERT_OK(maycup_parser_ctor(&parser), fail);

    ASSERT_OK(maycup_parse(&parser, &lexer), fail);

    ast_str = maycup_ast_tostr(&parser.ast, parser.root_astnode);
    ASSERT_STR_EQ_FILE(ast_str, UTEST_FIXTURE_PATH_EXPECTED "parser_1.txt",
                       fail);
    free(ast_str);

    ASSERT_OK(maycup_parser_dtor(&parser), fail);
    ASSERT_OK(maycup_lexer_dtor(&lexer), fail);
    ASSERT_OK(maycup_filereader_dtor(&fr), fail);
    return TEST_RESULT_PASS;
fail:
    free(ast_str);
    maycup_parser_dtor(&parser);
    maycup_lexer_dtor(&lexer);
    maycup_filereader_dtor(&fr);
    return TEST_RESULT_FAIL;
}

TEST_CASE(parser_parse_illegal_arg) {
    MAYCUP_FileReader fr;
    MAYCUP_Lexer lexer;
    MAYCUP_Parser parser;

    ASSERT_OK(
        maycup_filereader_ctor(&fr, UTEST_FIXTURE_PATH_INPUT "parser_1.md"),
        fail);
    ASSERT_OK(maycup_lexer_ctor(&lexer, (MAYCUP_Reader *)&fr), fail);
    ASSERT_OK(maycup_parser_ctor(&parser), fail);

    ASSERT_EQ(maycup_parse(NULL, &lexer), MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);
    ASSERT_EQ(maycup_parse(&parser, NULL), MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);

    ASSERT_OK(maycup_parser_dtor(&parser), fail);
    ASSERT_OK(maycup_lexer_dtor(&lexer), fail);
    ASSERT_OK(maycup_filereader_dtor(&fr), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_parser_dtor(&parser);
    maycup_lexer_dtor(&lexer);
    maycup_filereader_dtor(&fr);
    return TEST_RESULT_FAIL;
}

TEST_SUITE_BEGIN(parser)

TEST_CASE_ADD(parser_ctor_normal);
TEST_CASE_ADD(parser_ctor_illegal_arg);

TEST_CASE_ADD(parser_dtor_normal);
TEST_CASE_ADD(parser_dtor_double);
TEST_CASE_ADD(parser_dtor_illegal_arg);

TEST_CASE_ADD(parser_parse_normal);
TEST_CASE_ADD(parser_parse_illegal_arg);

TEST_SUITE_END