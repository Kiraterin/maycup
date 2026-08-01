/**
 * @file test_ast.c
 * @brief Test for AST
 * @date 2026-08-02
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

// this annotation is placed to interupt the include sorting
#include "md2html/core/ast.h"

TEST_CASE(ast_ctor_normal) {
    M2H_AST ast;
    ssize_t head = -1;

    ASSERT_OK(M2H_ast_ctor(&ast, &head), fail);
    ASSERT_EQ(head >= 0, true, fail);

    ASSERT_OK(M2H_ast_dtor(&ast), fail);
    return TEST_RESULT_PASS;
fail:
    ASSERT_OK(M2H_ast_dtor(&ast), fail);
    return TEST_RESULT_FAIL;
}

TEST_CASE(ast_ctor_illegal_arg) {
    M2H_AST ast;
    ssize_t head;

    ASSERT_EQ(M2H_ast_ctor(NULL, &head), M2H_RESULT_ILLEGAL_ARGUMENT, fail);
    ASSERT_EQ(M2H_ast_ctor(&ast, NULL), M2H_RESULT_ILLEGAL_ARGUMENT, fail);

    return TEST_RESULT_PASS;
fail:
    return TEST_RESULT_FAIL;
}

TEST_CASE(ast_ctor_malloc_fail) {
    M2H_AST ast;
    ssize_t head;

    TEST_CTX.mock_state.m_malloc = true;
    ASSERT_EQ(M2H_ast_ctor(&ast, &head), M2H_RESULT_MALLOC_FAIL, fail);

    TEST_CTX.mock_state.m_malloc = false;
    return TEST_RESULT_PASS;
fail:
    TEST_CTX.mock_state.m_malloc = false;
    return TEST_RESULT_FAIL;
}

TEST_CASE(ast_dtor_normal) {
    M2H_AST ast;
    ssize_t head;

    ASSERT_OK(M2H_ast_ctor(&ast, &head), fail);
    ASSERT_OK(M2H_ast_dtor(&ast), fail);

    ASSERT_EQ(ast.cap, 0, fail);
    ASSERT_EQ(ast.first_free, -1, fail);
    ASSERT_EQ(ast.next_free, NULL, fail);
    ASSERT_EQ(ast.data, NULL, fail);
    ASSERT_EQ(ast.is_allocated, NULL, fail);
    return TEST_RESULT_PASS;
fail:
    ASSERT_OK(M2H_ast_dtor(&ast), fail);
    return TEST_RESULT_FAIL;
}

TEST_CASE(ast_dtor_double) {
    M2H_AST ast;
    ssize_t head;

    ASSERT_OK(M2H_ast_ctor(&ast, &head), fail);

    ASSERT_OK(M2H_ast_dtor(&ast), fail);
    // allow double free
    ASSERT_OK(M2H_ast_dtor(&ast), fail);
    return TEST_RESULT_PASS;
fail:
    ASSERT_OK(M2H_ast_dtor(&ast), fail);
    return TEST_RESULT_FAIL;
}

TEST_SUITE_BEGIN(ast)

TEST_CASE_ADD(ast_ctor_normal);
TEST_CASE_ADD(ast_ctor_illegal_arg);
TEST_CASE_ADD(ast_ctor_malloc_fail);

TEST_CASE_ADD(ast_dtor_normal);
TEST_CASE_ADD(ast_dtor_double);

TEST_SUITE_END