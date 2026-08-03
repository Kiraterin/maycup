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

#include "md2html/core/ast.h"
#include "test.h"
#include <string.h>

TEST_CASE(ast_ctor_normal) {
    M2H_AST ast;
    ssize_t head = -1;

    ASSERT_OK(M2H_ast_ctor(&ast, &head), fail);
    ASSERT_EQ(head > 0, true, fail);

    ASSERT_OK(M2H_ast_dtor(&ast), fail);
    return TEST_RESULT_PASS;
fail:
    M2H_ast_dtor(&ast);
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
    M2H_ast_dtor(&ast);
    return TEST_RESULT_FAIL;
}

TEST_CASE(ast_dtor_illegal_arg) {
    ASSERT_EQ(M2H_ast_dtor(NULL), M2H_RESULT_ILLEGAL_ARGUMENT, fail);

    return TEST_RESULT_PASS;
fail:
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
    M2H_ast_dtor(&ast);
    return TEST_RESULT_FAIL;
}

TEST_CASE(ast_insert_normal) {
    M2H_AST ast;
    ssize_t head;

    ASSERT_OK(M2H_ast_ctor(&ast, &head), fail);

    ssize_t node[3];
    ASSERT_OK(M2H_insert_astnode(&node[0], &ast, head, M2H_ASTNODE_TYPE_NONE),
              fail);
    ASSERT_OK(
        M2H_insert_astnode(&node[1], &ast, node[0], M2H_ASTNODE_TYPE_HEADING),
        fail);

    ast.data[node[1]].heading.level = 2;

    ASSERT_EQ(ast.data[node[0]].parent, head, fail);
    ASSERT_EQ(ast.data[node[0]].child, node[1], fail);
    ASSERT_EQ(ast.data[node[0]].prev_sibling, -1, fail);
    ASSERT_EQ(ast.data[node[0]].next_sibling, -1, fail);
    ASSERT_EQ(ast.data[node[1]].parent, node[0], fail);
    ASSERT_EQ(ast.data[node[1]].child, -1, fail);
    ASSERT_EQ(ast.data[node[1]].prev_sibling, -1, fail);
    ASSERT_EQ(ast.data[node[1]].next_sibling, -1, fail);

    ASSERT_OK(
        M2H_insert_astnode(&node[2], &ast, node[0], M2H_ASTNODE_TYPE_NONE),
        fail);
    ASSERT_EQ(ast.data[node[0]].child, node[2], fail);
    ASSERT_EQ(ast.data[node[1]].parent, node[0], fail);
    ASSERT_EQ(ast.data[node[1]].next_sibling, node[2], fail);
    ASSERT_EQ(ast.data[node[2]].parent, node[0], fail);
    ASSERT_EQ(ast.data[node[2]].child, -1, fail);
    ASSERT_EQ(ast.data[node[2]].prev_sibling, node[1], fail);

    ASSERT_EQ(ast.data[node[1]].type, M2H_ASTNODE_TYPE_HEADING, fail);
    ASSERT_EQ(ast.data[node[1]].heading.level, 2, fail);

    ASSERT_OK(M2H_ast_dtor(&ast), fail);
    return TEST_RESULT_PASS;
fail:
    M2H_ast_dtor(&ast);
    return TEST_RESULT_FAIL;
}

TEST_CASE(ast_insert_bulk) {
    M2H_AST ast;
    ssize_t head;
    const size_t cnt_1st = 100;
    const size_t cnt_2nd = 500;
    ssize_t node_1st[cnt_1st];
    ssize_t node_2nd[cnt_1st][cnt_2nd];

    ASSERT_OK(M2H_ast_ctor(&ast, &head), fail);
    for (size_t i = 0; i < cnt_1st; ++i) {
        ASSERT_OK(M2H_insert_astnode(&node_1st[i], &ast, head,
                                     M2H_ASTNODE_TYPE_HEADING),
                  fail);
        ast.data[node_1st[i]].heading.level = i % UINT8_MAX;
        for (size_t j = 0; j < cnt_2nd; ++j) {
            ASSERT_OK(M2H_insert_astnode(&node_2nd[i][j], &ast, node_1st[i],
                                         M2H_ASTNODE_TYPE_HEADING),
                      fail);
            ast.data[node_2nd[i][j]].heading.level = (i + j) % UINT8_MAX;
        }
    }

    for (size_t i = 0; i < cnt_1st; ++i) {
        ASSERT_EQ(ast.data[node_1st[i]].heading.level, i, fail);
        for (size_t j = 0; j < cnt_2nd; ++j) {
            ASSERT_EQ(ast.data[node_2nd[i][j]].heading.level,
                      (i + j) % UINT8_MAX, fail);
        }
    }

    ASSERT_OK(M2H_ast_dtor(&ast), fail);
    return TEST_RESULT_PASS;
fail:
    M2H_ast_dtor(&ast);
    return TEST_RESULT_FAIL;
}

TEST_CASE(ast_insert_illegal_arg) {
    M2H_AST ast;
    ssize_t head;

    ASSERT_OK(M2H_ast_ctor(&ast, &head), fail);

    ASSERT_EQ(M2H_insert_astnode(NULL, NULL, head, M2H_ASTNODE_TYPE_NONE),
              M2H_RESULT_ILLEGAL_ARGUMENT, fail);
    ASSERT_EQ(M2H_insert_astnode(NULL, &ast, -1, M2H_ASTNODE_TYPE_NONE),
              M2H_RESULT_ILLEGAL_ARGUMENT, fail);
    ASSERT_EQ(M2H_insert_astnode(NULL, &ast, 0, M2H_ASTNODE_TYPE_NONE),
              M2H_RESULT_ILLEGAL_ARGUMENT, fail);
    ASSERT_EQ(M2H_insert_astnode(NULL, &ast, M2H_DEFAULT_AST_SIZE - 1,
                                 M2H_ASTNODE_TYPE_NONE),
              M2H_RESULT_ILLEGAL_ARGUMENT, fail);

    ASSERT_OK(M2H_ast_dtor(&ast), fail);
    return TEST_RESULT_PASS;
fail:
    M2H_ast_dtor(&ast);
    return TEST_RESULT_FAIL;
}

TEST_CASE(ast_insert_realloc_fail) {
    M2H_AST ast;
    ssize_t head;
    const size_t cnt_1st = 2;
    const size_t cnt_2nd = 10;
    ssize_t node_1st[cnt_1st];
    ssize_t node_2nd[cnt_1st][cnt_2nd];
    M2H_Result res;

    TEST_CTX.mock_state.m_realloc = true;

    ASSERT_OK(M2H_ast_ctor(&ast, &head), fail);
    for (size_t i = 0; i < cnt_1st; ++i) {
        res =
            M2H_insert_astnode(&node_1st[i], &ast, head, M2H_ASTNODE_TYPE_NONE);
        if (res == M2H_RESULT_MALLOC_FAIL) {
            goto pass;
        }
        for (size_t j = 0; j < cnt_2nd; ++j) {
            res = M2H_insert_astnode(&node_2nd[i][j], &ast, node_1st[i],
                                     M2H_ASTNODE_TYPE_NONE);
            if (res == M2H_RESULT_MALLOC_FAIL) {
                goto pass;
            }
        }
    }

fail:
    ASSERT_OK(M2H_ast_dtor(&ast), fail);
    TEST_CTX.mock_state.m_realloc = false;
    return TEST_RESULT_FAIL;
pass:
    M2H_ast_dtor(&ast);
    TEST_CTX.mock_state.m_realloc = false;
    return TEST_RESULT_PASS;
}

TEST_CASE(ast_insert_maxcap_exceeded) {
    M2H_AST ast;
    ssize_t head;
    size_t original_cap;

    ASSERT_OK(M2H_ast_ctor(&ast, &head), fail);
    ast.first_free = -1;
    original_cap = ast.cap;
    ast.cap = M2H_MAX_AST_CAP + 1;
    ASSERT_EQ(M2H_insert_astnode(NULL, &ast, head, M2H_ASTNODE_TYPE_NONE),
              M2H_RESULT_MAX_CAP_EXCEEDED, fail);
    ast.cap = original_cap;

    ASSERT_OK(M2H_ast_dtor(&ast), fail);
    return TEST_RESULT_PASS;
fail:
    M2H_ast_dtor(&ast);
    return TEST_RESULT_FAIL;
}

TEST_CASE(ast_delete_normal) {
    M2H_AST ast;
    ssize_t head;
    const size_t cnt_1st = 10;
    const size_t cnt_2nd = 20;
    ssize_t node_1st[cnt_1st];
    ssize_t node_2nd[cnt_1st][cnt_2nd];

    ASSERT_OK(M2H_ast_ctor(&ast, &head), fail);
    for (size_t i = 0; i < cnt_1st - 1; ++i) {
        ASSERT_OK(M2H_insert_astnode(&node_1st[i], &ast, head,
                                     M2H_ASTNODE_TYPE_HEADING),
                  fail);
        ast.data[node_1st[i]].heading.level = i % UINT8_MAX;
        for (size_t j = 0; j < cnt_2nd; ++j) {
            ASSERT_OK(M2H_insert_astnode(&node_2nd[i][j], &ast, node_1st[i],
                                         M2H_ASTNODE_TYPE_HEADING),
                      fail);
            ast.data[node_2nd[i][j]].heading.level = (i + j) % UINT8_MAX;
        }
    }

    ASSERT_OK(M2H_delete_astnode(&ast, node_1st[7]), fail);

    ASSERT_OK(M2H_insert_astnode(&node_1st[cnt_1st - 1], &ast, head,
                                 M2H_ASTNODE_TYPE_HEADING),
              fail);
    ast.data[node_1st[cnt_1st - 1]].heading.level = (cnt_1st - 1) % UINT8_MAX;
    for (size_t j = 0; j < cnt_2nd; ++j) {
        ASSERT_OK(M2H_insert_astnode(&node_2nd[cnt_1st - 1][j], &ast,
                                     node_1st[cnt_1st - 1],
                                     M2H_ASTNODE_TYPE_HEADING),
                  fail);
        ast.data[node_2nd[cnt_1st - 1][j]].heading.level =
            (cnt_1st - 1 + j) % UINT8_MAX;
    }

    for (size_t i = 0; i < cnt_1st; ++i) {
        if (i != 7) {
            ASSERT_EQ(ast.data[node_1st[i]].heading.level, i % UINT8_MAX, fail);
        }
    }

    ASSERT_OK(M2H_ast_dtor(&ast), fail);
    return TEST_RESULT_PASS;
fail:
    M2H_ast_dtor(&ast);
    return TEST_RESULT_FAIL;
}

TEST_CASE(ast_delete_illegal_arg) {
    M2H_AST ast;
    ssize_t head;
    ASSERT_OK(M2H_ast_ctor(&ast, &head), fail);

    ASSERT_EQ(M2H_delete_astnode(NULL, head), M2H_RESULT_ILLEGAL_ARGUMENT,
              fail);
    ASSERT_EQ(M2H_delete_astnode(&ast, 0), M2H_RESULT_ILLEGAL_ARGUMENT, fail);
    ASSERT_EQ(M2H_delete_astnode(&ast, M2H_MAX_AST_CAP + 10),
              M2H_RESULT_ILLEGAL_ARGUMENT, fail);
    ASSERT_EQ(M2H_delete_astnode(&ast, M2H_MAX_AST_CAP - 1),
              M2H_RESULT_ILLEGAL_ARGUMENT, fail);

    ASSERT_OK(M2H_ast_dtor(&ast), fail);
    return TEST_RESULT_PASS;
fail:
    M2H_ast_dtor(&ast);
    return TEST_RESULT_FAIL;
}

TEST_CASE(astnode_text_ctor_normal) {
    M2H_ASTNodeDataText andt1;
    M2H_ASTNodeDataText andt2;
    char *text1 = strdup("123");
    char *text2 = strdup("test");

    ASSERT_OK(
        M2H_astnode_data_text_ctor(&andt1, text1, M2H_TEXTSTYLE_PLAIN, false),
        fail);
    ASSERT_EQ(strcmp(andt1.content, "123"), 0, fail);
    ASSERT_EQ(andt1.style, M2H_TEXTSTYLE_PLAIN, fail);
    ASSERT_EQ(andt1.newline_tailed, false, fail);

    ASSERT_OK(M2H_astnode_data_text_ctor(&andt2, text2,
                                         M2H_TEXTSTYLE_BOLDITALIC, true),
              fail);
    ASSERT_EQ(strcmp(andt2.content, "test"), 0, fail);
    ASSERT_EQ(andt2.style, M2H_TEXTSTYLE_BOLDITALIC, fail);
    ASSERT_EQ(andt2.newline_tailed, true, fail);

    ASSERT_OK(M2H_astnode_data_text_dtor(&andt1), fail);
    ASSERT_OK(M2H_astnode_data_text_dtor(&andt2), fail);
    return TEST_RESULT_PASS;
fail:
    M2H_astnode_data_text_dtor(&andt1);
    M2H_astnode_data_text_dtor(&andt2);
    return TEST_RESULT_FAIL;
}

TEST_CASE(astnode_text_ctor_illegal_arg) {
    M2H_ASTNodeDataText andt;
    char *text = strdup("12345");

    ASSERT_EQ(
        M2H_astnode_data_text_ctor(NULL, text, M2H_TEXTSTYLE_PLAIN, false),
        M2H_RESULT_ILLEGAL_ARGUMENT, fail);
    ASSERT_EQ(
        M2H_astnode_data_text_ctor(&andt, NULL, M2H_TEXTSTYLE_PLAIN, false),
        M2H_RESULT_ILLEGAL_ARGUMENT, fail);

    free(text);
    return TEST_RESULT_PASS;
fail:
    free(text);
    return TEST_RESULT_FAIL;
}

TEST_CASE(astnode_text_dtor_normal) {
    M2H_ASTNodeDataText andt;
    char *text = strdup("12345");

    ASSERT_OK(
        M2H_astnode_data_text_ctor(&andt, text, M2H_TEXTSTYLE_PLAIN, false),
        fail);
    ASSERT_OK(M2H_astnode_data_text_dtor(&andt), fail);
    ASSERT_EQ(andt.content, NULL, fail);

    return TEST_RESULT_PASS;
fail:
    return TEST_RESULT_FAIL;
}

TEST_CASE(astnode_text_dtor_double) {
    M2H_ASTNodeDataText andt;
    char *text = strdup("12345");

    ASSERT_OK(
        M2H_astnode_data_text_ctor(&andt, text, M2H_TEXTSTYLE_PLAIN, false),
        fail);
    ASSERT_OK(M2H_astnode_data_text_dtor(&andt), fail);
    ASSERT_OK(M2H_astnode_data_text_dtor(&andt), fail);

    return TEST_RESULT_PASS;
fail:
    return TEST_RESULT_FAIL;
}

TEST_CASE(astnode_text_dtor_illegal_arg) {
    ASSERT_EQ(M2H_astnode_data_text_dtor(NULL), M2H_RESULT_ILLEGAL_ARGUMENT,
              fail);
    return TEST_RESULT_PASS;
fail:
    return TEST_RESULT_FAIL;
}

TEST_SUITE_BEGIN(ast)

TEST_CASE_ADD(ast_ctor_normal);
TEST_CASE_ADD(ast_ctor_illegal_arg);
TEST_CASE_ADD(ast_ctor_malloc_fail);

TEST_CASE_ADD(ast_dtor_normal);
TEST_CASE_ADD(ast_dtor_illegal_arg);
TEST_CASE_ADD(ast_dtor_double);

TEST_CASE_ADD(ast_insert_normal);
TEST_CASE_ADD(ast_insert_bulk);
TEST_CASE_ADD(ast_insert_illegal_arg);
TEST_CASE_ADD(ast_insert_realloc_fail);
TEST_CASE_ADD(ast_insert_maxcap_exceeded);

TEST_CASE_ADD(ast_delete_normal);
TEST_CASE_ADD(ast_delete_illegal_arg);

TEST_CASE_ADD(astnode_text_ctor_normal);
TEST_CASE_ADD(astnode_text_ctor_illegal_arg);

TEST_CASE_ADD(astnode_text_dtor_normal);
TEST_CASE_ADD(astnode_text_dtor_double);
TEST_CASE_ADD(astnode_text_dtor_illegal_arg);

TEST_SUITE_END