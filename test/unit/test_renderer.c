/**
 * @file test_renderer.c
 * @brief Test for renderer
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

#include "debug/objprint.h"
#include "maycup/lexer.h"
#include "maycup/parser.h"
#include "maycup/reader.h"
#include "maycup/renderer.h"
#include "maycup/result.h"
#include "maycup/writer.h"
#include "test.h"
#include <string.h>

TEST_CASE(renderer_ctor_normal) {
    MAYCUP_StringWriter sw;
    MAYCUP_Renderer renderer;

    ASSERT_OK(maycup_stringwriter_ctor_flexible(
                  &sw, MAYCUP_DEFAULT_STRWRITER_FLEXBUF_SIZE),
              fail);
    ASSERT_OK(maycup_renderer_ctor(&renderer, (MAYCUP_Writer *)&sw), fail);
    ASSERT_NEQ(renderer.writer, NULL, fail);

    ASSERT_OK(maycup_stringwriter_dtor(&sw, NULL), fail);
    ASSERT_OK(maycup_renderer_dtor(&renderer), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_stringwriter_dtor(&sw, NULL);
    maycup_renderer_dtor(&renderer);
    return TEST_RESULT_FAIL;
}

TEST_CASE(renderer_ctor_illegal_arg) {
    MAYCUP_StringWriter sw;
    MAYCUP_Renderer renderer;

    ASSERT_OK(maycup_stringwriter_ctor_flexible(
                  &sw, MAYCUP_DEFAULT_STRWRITER_FLEXBUF_SIZE),
              fail);
    ASSERT_OK(maycup_renderer_ctor(&renderer, (MAYCUP_Writer *)&sw), fail);
    ASSERT_EQ(maycup_renderer_ctor(NULL, (MAYCUP_Writer *)&sw),
              MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);
    ASSERT_EQ(maycup_renderer_ctor(&renderer, NULL),
              MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);

    ASSERT_OK(maycup_stringwriter_dtor(&sw, NULL), fail);
    ASSERT_OK(maycup_renderer_dtor(&renderer), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_stringwriter_dtor(&sw, NULL);
    maycup_renderer_dtor(&renderer);
    return TEST_RESULT_FAIL;
}

TEST_CASE(renderer_dtor_normal) {
    MAYCUP_StringWriter sw;
    MAYCUP_Renderer renderer;

    ASSERT_OK(maycup_stringwriter_ctor_flexible(
                  &sw, MAYCUP_DEFAULT_STRWRITER_FLEXBUF_SIZE),
              fail);
    ASSERT_OK(maycup_renderer_ctor(&renderer, (MAYCUP_Writer *)&sw), fail);
    ASSERT_OK(maycup_renderer_dtor(&renderer), fail);
    ASSERT_EQ(renderer.writer, NULL, fail);

    ASSERT_OK(maycup_stringwriter_dtor(&sw, NULL), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_stringwriter_dtor(&sw, NULL);
    maycup_renderer_dtor(&renderer);
    return TEST_RESULT_FAIL;
}

TEST_CASE(renderer_dtor_double) {
    MAYCUP_StringWriter sw;
    MAYCUP_Renderer renderer;

    ASSERT_OK(maycup_stringwriter_ctor_flexible(
                  &sw, MAYCUP_DEFAULT_STRWRITER_FLEXBUF_SIZE),
              fail);
    ASSERT_OK(maycup_renderer_ctor(&renderer, (MAYCUP_Writer *)&sw), fail);
    ASSERT_OK(maycup_renderer_dtor(&renderer), fail);
    ASSERT_EQ(maycup_renderer_dtor(&renderer), MAYCUP_RESULT_ILLEGAL_ARGUMENT,
              fail);

    ASSERT_OK(maycup_stringwriter_dtor(&sw, NULL), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_stringwriter_dtor(&sw, NULL);
    maycup_renderer_dtor(&renderer);
    return TEST_RESULT_FAIL;
}

TEST_CASE(renderer_dtor_illegal_arg) {
    ASSERT_EQ(maycup_renderer_dtor(NULL), MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);
    return TEST_RESULT_PASS;
fail:
    return TEST_RESULT_FAIL;
}

TEST_CASE(renderer_render_normal) {
    MAYCUP_FileReader fr;
    MAYCUP_Lexer lexer;
    MAYCUP_Parser parser;
    MAYCUP_StringWriter sw;
    MAYCUP_Renderer renderer;

    ASSERT_OK(
        maycup_filereader_ctor(&fr, UTEST_FIXTURE_PATH_INPUT "renderer_1.md"),
        fail);
    ASSERT_OK(maycup_lexer_ctor(&lexer, (MAYCUP_Reader *)&fr), fail);
    ASSERT_OK(maycup_parser_ctor(&parser), fail);
    ASSERT_OK(maycup_stringwriter_ctor_flexible(
                  &sw, MAYCUP_DEFAULT_STRWRITER_FLEXBUF_SIZE),
              fail);
    ASSERT_OK(maycup_renderer_ctor(&renderer, (MAYCUP_Writer *)&sw), fail);

    ASSERT_OK(maycup_parse(&parser, &lexer), fail);
    ASSERT_OK(maycup_render(&renderer, &parser), fail);
    ASSERT_STR_EQ_FILE(sw.buf, UTEST_FIXTURE_PATH_EXPECTED "renderer_1.html",
                       fail);

    ASSERT_OK(maycup_renderer_dtor(&renderer), fail);
    ASSERT_OK(maycup_stringwriter_dtor(&sw, NULL), fail);
    ASSERT_OK(maycup_parser_dtor(&parser), fail);
    ASSERT_OK(maycup_lexer_dtor(&lexer), fail);
    ASSERT_OK(maycup_filereader_dtor(&fr), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_renderer_dtor(&renderer);
    maycup_stringwriter_dtor(&sw, NULL);
    maycup_parser_dtor(&parser);
    maycup_lexer_dtor(&lexer);
    maycup_filereader_dtor(&fr);
    return TEST_RESULT_FAIL;
}

TEST_CASE(renderer_render_illegal_arg) {
    MAYCUP_FileReader fr;
    MAYCUP_Lexer lexer;
    MAYCUP_Parser parser;
    MAYCUP_StringWriter sw;
    MAYCUP_Renderer renderer;

    ASSERT_OK(
        maycup_filereader_ctor(&fr, UTEST_FIXTURE_PATH_INPUT "renderer_1.md"),
        fail);
    ASSERT_OK(maycup_lexer_ctor(&lexer, (MAYCUP_Reader *)&fr), fail);
    ASSERT_OK(maycup_parser_ctor(&parser), fail);
    ASSERT_OK(maycup_stringwriter_ctor_flexible(
                  &sw, MAYCUP_DEFAULT_STRWRITER_FLEXBUF_SIZE),
              fail);
    ASSERT_OK(maycup_renderer_ctor(&renderer, (MAYCUP_Writer *)&sw), fail);

    ASSERT_EQ(maycup_render(NULL, &parser), MAYCUP_RESULT_ILLEGAL_ARGUMENT,
              fail);
    ASSERT_EQ(maycup_render(&renderer, NULL), MAYCUP_RESULT_ILLEGAL_ARGUMENT,
              fail);

    ASSERT_OK(maycup_renderer_dtor(&renderer), fail);
    ASSERT_OK(maycup_stringwriter_dtor(&sw, NULL), fail);
    ASSERT_OK(maycup_parser_dtor(&parser), fail);
    ASSERT_OK(maycup_lexer_dtor(&lexer), fail);
    ASSERT_OK(maycup_filereader_dtor(&fr), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_renderer_dtor(&renderer);
    maycup_stringwriter_dtor(&sw, NULL);
    maycup_parser_dtor(&parser);
    maycup_lexer_dtor(&lexer);
    maycup_filereader_dtor(&fr);
    return TEST_RESULT_FAIL;
}

TEST_SUITE_BEGIN(renderer)

TEST_CASE_ADD(renderer_ctor_normal);
TEST_CASE_ADD(renderer_ctor_illegal_arg);

TEST_CASE_ADD(renderer_dtor_normal);
TEST_CASE_ADD(renderer_dtor_double);
TEST_CASE_ADD(renderer_dtor_illegal_arg);

TEST_CASE_ADD(renderer_render_normal);
TEST_CASE_ADD(renderer_render_illegal_arg);

TEST_SUITE_END