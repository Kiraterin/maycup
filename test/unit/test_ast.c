/**
 * @file test_ast.c
 * @brief Test for AST
 * @date 2026-08-02
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

#include "maycup/core/ast.h"
#include "test.h"
#include <string.h>

TEST_CASE(ast_ctor_normal) {
    MAYCUP_AST ast;
    ssize_t root = -1;

    ASSERT_OK(maycup_ast_ctor(&ast, &root), fail);
    ASSERT_EQ(root > 0, true, fail);

    ASSERT_OK(maycup_ast_dtor(&ast), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_ast_dtor(&ast);
    return TEST_RESULT_FAIL;
}

TEST_CASE(ast_ctor_illegal_arg) {
    MAYCUP_AST ast;
    ssize_t root;

    ASSERT_EQ(maycup_ast_ctor(NULL, &root), MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);
    ASSERT_EQ(maycup_ast_ctor(&ast, NULL), MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);

    return TEST_RESULT_PASS;
fail:
    return TEST_RESULT_FAIL;
}

TEST_CASE(ast_ctor_malloc_fail) {
    MAYCUP_AST ast;
    ssize_t root;

    MOCK_ON(malloc);
    ASSERT_EQ(maycup_ast_ctor(&ast, &root), MAYCUP_RESULT_MALLOC_FAIL, fail);

    MOCK_OFF(malloc);
    return TEST_RESULT_PASS;
fail:
    MOCK_OFF(malloc);
    return TEST_RESULT_FAIL;
}

TEST_CASE(ast_dtor_normal) {
    MAYCUP_AST ast;
    ssize_t root;

    ASSERT_OK(maycup_ast_ctor(&ast, &root), fail);
    ASSERT_OK(maycup_ast_dtor(&ast), fail);

    ASSERT_EQ(ast.cap, 0, fail);
    ASSERT_EQ(ast.first_free, -1, fail);
    ASSERT_EQ(ast.next_free, NULL, fail);
    ASSERT_EQ(ast.data, NULL, fail);
    ASSERT_EQ(ast.is_allocated, NULL, fail);
    return TEST_RESULT_PASS;
fail:
    maycup_ast_dtor(&ast);
    return TEST_RESULT_FAIL;
}

TEST_CASE(ast_dtor_illegal_arg) {
    ASSERT_EQ(maycup_ast_dtor(NULL), MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);

    return TEST_RESULT_PASS;
fail:
    return TEST_RESULT_FAIL;
}

TEST_CASE(ast_dtor_double) {
    MAYCUP_AST ast;
    ssize_t root;

    ASSERT_OK(maycup_ast_ctor(&ast, &root), fail);

    ASSERT_OK(maycup_ast_dtor(&ast), fail);
    // allow double free
    ASSERT_OK(maycup_ast_dtor(&ast), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_ast_dtor(&ast);
    return TEST_RESULT_FAIL;
}

TEST_CASE(ast_insert_normal) {
    MAYCUP_AST ast;
    ssize_t root;

    ASSERT_OK(maycup_ast_ctor(&ast, &root), fail);

    ssize_t node[5];
    ASSERT_OK(maycup_insert_astnode(&node[0], &ast, root, MAYCUP_ASTNODE_TYPE_NONE),
              fail);
    ASSERT_OK(
        maycup_insert_astnode(&node[1], &ast, node[0], MAYCUP_ASTNODE_TYPE_HEADING),
        fail);

    ast.data[node[1]].heading.level = 2;

    ASSERT_EQ(ast.data[root].child, node[0], fail);

    ASSERT_EQ(ast.data[node[0]].parent, root, fail);
    ASSERT_EQ(ast.data[node[0]].child, node[1], fail);
    ASSERT_EQ(ast.data[node[0]].prev_sibling, -1, fail);
    ASSERT_EQ(ast.data[node[0]].next_sibling, -1, fail);

    ASSERT_EQ(ast.data[node[1]].parent, node[0], fail);
    ASSERT_EQ(ast.data[node[1]].child, -1, fail);
    ASSERT_EQ(ast.data[node[1]].prev_sibling, -1, fail);
    ASSERT_EQ(ast.data[node[1]].next_sibling, -1, fail);

    ASSERT_OK(
        maycup_insert_astnode(&node[2], &ast, node[0], MAYCUP_ASTNODE_TYPE_NONE),
        fail);

    ASSERT_EQ(ast.data[root].child, node[0], fail);

    ASSERT_EQ(ast.data[node[0]].parent, root, fail);
    ASSERT_EQ(ast.data[node[0]].child, node[2], fail);
    ASSERT_EQ(ast.data[node[0]].prev_sibling, -1, fail);
    ASSERT_EQ(ast.data[node[0]].next_sibling, -1, fail);

    ASSERT_EQ(ast.data[node[1]].parent, node[0], fail);
    ASSERT_EQ(ast.data[node[1]].child, -1, fail);
    ASSERT_EQ(ast.data[node[1]].prev_sibling, -1, fail);
    ASSERT_EQ(ast.data[node[1]].next_sibling, node[2], fail);

    ASSERT_EQ(ast.data[node[2]].parent, node[0], fail);
    ASSERT_EQ(ast.data[node[2]].child, -1, fail);
    ASSERT_EQ(ast.data[node[2]].prev_sibling, node[1], fail);
    ASSERT_EQ(ast.data[node[2]].next_sibling, -1, fail);

    ASSERT_OK(
        maycup_insert_astnode(&node[3], &ast, node[0], MAYCUP_ASTNODE_TYPE_NONE),
        fail);

    ASSERT_EQ(ast.data[root].child, node[0], fail);

    ASSERT_EQ(ast.data[node[0]].parent, root, fail);
    ASSERT_EQ(ast.data[node[0]].child, node[3], fail);
    ASSERT_EQ(ast.data[node[0]].prev_sibling, -1, fail);
    ASSERT_EQ(ast.data[node[0]].next_sibling, -1, fail);

    ASSERT_EQ(ast.data[node[1]].parent, node[0], fail);
    ASSERT_EQ(ast.data[node[1]].child, -1, fail);
    ASSERT_EQ(ast.data[node[1]].prev_sibling, -1, fail);
    ASSERT_EQ(ast.data[node[1]].next_sibling, node[2], fail);

    ASSERT_EQ(ast.data[node[2]].parent, node[0], fail);
    ASSERT_EQ(ast.data[node[2]].child, -1, fail);
    ASSERT_EQ(ast.data[node[2]].prev_sibling, node[1], fail);
    ASSERT_EQ(ast.data[node[2]].next_sibling, node[3], fail);

    ASSERT_EQ(ast.data[node[3]].parent, node[0], fail);
    ASSERT_EQ(ast.data[node[3]].child, -1, fail);
    ASSERT_EQ(ast.data[node[3]].prev_sibling, node[2], fail);
    ASSERT_EQ(ast.data[node[3]].next_sibling, -1, fail);

    ASSERT_OK(
        maycup_insert_astnode(&node[4], &ast, node[2], MAYCUP_ASTNODE_TYPE_NONE),
        fail);

    ASSERT_EQ(ast.data[root].child, node[0], fail);

    ASSERT_EQ(ast.data[node[0]].parent, root, fail);
    ASSERT_EQ(ast.data[node[0]].child, node[3], fail);
    ASSERT_EQ(ast.data[node[0]].prev_sibling, -1, fail);
    ASSERT_EQ(ast.data[node[0]].next_sibling, -1, fail);

    ASSERT_EQ(ast.data[node[1]].parent, node[0], fail);
    ASSERT_EQ(ast.data[node[1]].child, -1, fail);
    ASSERT_EQ(ast.data[node[1]].prev_sibling, -1, fail);
    ASSERT_EQ(ast.data[node[1]].next_sibling, node[2], fail);

    ASSERT_EQ(ast.data[node[2]].parent, node[0], fail);
    ASSERT_EQ(ast.data[node[2]].child, node[4], fail);
    ASSERT_EQ(ast.data[node[2]].prev_sibling, node[1], fail);
    ASSERT_EQ(ast.data[node[2]].next_sibling, node[3], fail);

    ASSERT_EQ(ast.data[node[3]].parent, node[0], fail);
    ASSERT_EQ(ast.data[node[3]].child, -1, fail);
    ASSERT_EQ(ast.data[node[3]].prev_sibling, node[2], fail);
    ASSERT_EQ(ast.data[node[3]].next_sibling, -1, fail);

    ASSERT_EQ(ast.data[node[4]].parent, node[2], fail);
    ASSERT_EQ(ast.data[node[4]].child, -1, fail);
    ASSERT_EQ(ast.data[node[4]].prev_sibling, -1, fail);
    ASSERT_EQ(ast.data[node[4]].next_sibling, -1, fail);

    ASSERT_EQ(ast.data[node[1]].type, MAYCUP_ASTNODE_TYPE_HEADING, fail);
    ASSERT_EQ(ast.data[node[1]].heading.level, 2, fail);

    ASSERT_OK(maycup_ast_dtor(&ast), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_ast_dtor(&ast);
    return TEST_RESULT_FAIL;
}

TEST_CASE(ast_insert_bulk) {
    MAYCUP_AST ast;
    ssize_t root;
    const size_t cnt_1st = 100;
    const size_t cnt_2nd = 500;
    ssize_t node_1st[cnt_1st];
    ssize_t node_2nd[cnt_1st][cnt_2nd];

    ASSERT_OK(maycup_ast_ctor(&ast, &root), fail);
    for (size_t i = 0; i < cnt_1st; ++i) {
        ASSERT_OK(maycup_insert_astnode(&node_1st[i], &ast, root,
                                     MAYCUP_ASTNODE_TYPE_HEADING),
                  fail);
        ast.data[node_1st[i]].heading.level = i % UINT8_MAX;
        for (size_t j = 0; j < cnt_2nd; ++j) {
            ASSERT_OK(maycup_insert_astnode(&node_2nd[i][j], &ast, node_1st[i],
                                         MAYCUP_ASTNODE_TYPE_HEADING),
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

    ASSERT_OK(maycup_ast_dtor(&ast), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_ast_dtor(&ast);
    return TEST_RESULT_FAIL;
}

TEST_CASE(ast_insert_illegal_arg) {
    MAYCUP_AST ast;
    ssize_t root;

    ASSERT_OK(maycup_ast_ctor(&ast, &root), fail);

    ASSERT_EQ(maycup_insert_astnode(NULL, NULL, root, MAYCUP_ASTNODE_TYPE_NONE),
              MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);
    ASSERT_EQ(maycup_insert_astnode(NULL, &ast, -1, MAYCUP_ASTNODE_TYPE_NONE),
              MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);
    ASSERT_EQ(maycup_insert_astnode(NULL, &ast, 0, MAYCUP_ASTNODE_TYPE_NONE),
              MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);
    ASSERT_EQ(maycup_insert_astnode(NULL, &ast, MAYCUP_DEFAULT_AST_SIZE - 1,
                                 MAYCUP_ASTNODE_TYPE_NONE),
              MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);

    ASSERT_OK(maycup_ast_dtor(&ast), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_ast_dtor(&ast);
    return TEST_RESULT_FAIL;
}

TEST_CASE(ast_insert_realloc_fail) {
    MAYCUP_AST ast;
    ssize_t root;
    const size_t cnt_1st = 2;
    const size_t cnt_2nd = 10;
    ssize_t node_1st[cnt_1st];
    ssize_t node_2nd[cnt_1st][cnt_2nd];
    MAYCUP_Result res;

    MOCK_ON(realloc);

    ASSERT_OK(maycup_ast_ctor(&ast, &root), fail);
    for (size_t i = 0; i < cnt_1st; ++i) {
        res =
            maycup_insert_astnode(&node_1st[i], &ast, root, MAYCUP_ASTNODE_TYPE_NONE);
        if (res == MAYCUP_RESULT_MALLOC_FAIL) {
            goto pass;
        }
        for (size_t j = 0; j < cnt_2nd; ++j) {
            res = maycup_insert_astnode(&node_2nd[i][j], &ast, node_1st[i],
                                     MAYCUP_ASTNODE_TYPE_NONE);
            if (res == MAYCUP_RESULT_MALLOC_FAIL) {
                goto pass;
            }
        }
    }

fail:
    ASSERT_OK(maycup_ast_dtor(&ast), fail);
    MOCK_OFF(realloc);
    return TEST_RESULT_FAIL;
pass:
    maycup_ast_dtor(&ast);
    MOCK_OFF(realloc);
    return TEST_RESULT_PASS;
}

TEST_CASE(ast_insert_maxcap_exceeded) {
    MAYCUP_AST ast;
    ssize_t root;
    size_t original_cap;

    ASSERT_OK(maycup_ast_ctor(&ast, &root), fail);
    ast.first_free = -1;
    original_cap = ast.cap;
    ast.cap = MAYCUP_MAX_AST_CAP + 1;
    ASSERT_EQ(maycup_insert_astnode(NULL, &ast, root, MAYCUP_ASTNODE_TYPE_NONE),
              MAYCUP_RESULT_MAX_CAP_EXCEEDED, fail);
    ast.cap = original_cap;

    ASSERT_OK(maycup_ast_dtor(&ast), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_ast_dtor(&ast);
    return TEST_RESULT_FAIL;
}

TEST_CASE(ast_delete_normal) {
    MAYCUP_AST ast;
    ssize_t root;
    const size_t cnt_1st = 10;
    const size_t cnt_2nd = 20;
    ssize_t node_1st[cnt_1st];
    ssize_t node_2nd[cnt_1st][cnt_2nd];

    ASSERT_OK(maycup_ast_ctor(&ast, &root), fail);
    for (size_t i = 0; i < cnt_1st - 1; ++i) {
        ASSERT_OK(maycup_insert_astnode(&node_1st[i], &ast, root,
                                     MAYCUP_ASTNODE_TYPE_HEADING),
                  fail);
        ast.data[node_1st[i]].heading.level = i % UINT8_MAX;
        for (size_t j = 0; j < cnt_2nd; ++j) {
            ASSERT_OK(maycup_insert_astnode(&node_2nd[i][j], &ast, node_1st[i],
                                         MAYCUP_ASTNODE_TYPE_NONE),
                      fail);
        }
    }

    ASSERT_OK(maycup_delete_astnode(&ast, node_1st[7]), fail);

    ASSERT_OK(maycup_insert_astnode(&node_1st[cnt_1st - 1], &ast, root,
                                 MAYCUP_ASTNODE_TYPE_HEADING),
              fail);
    ast.data[node_1st[cnt_1st - 1]].heading.level = (cnt_1st - 1) % UINT8_MAX;
    for (size_t j = 0; j < cnt_2nd; ++j) {
        ASSERT_OK(maycup_insert_astnode(&node_2nd[cnt_1st - 1][j], &ast,
                                     node_1st[cnt_1st - 1],
                                     MAYCUP_ASTNODE_TYPE_NONE),
                  fail);
    }

    for (size_t i = 0; i < cnt_1st; ++i) {
        if (i != 7) {
            ASSERT_EQ(ast.data[node_1st[i]].heading.level, i % UINT8_MAX, fail);
        }
    }

    ASSERT_OK(maycup_ast_dtor(&ast), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_ast_dtor(&ast);
    return TEST_RESULT_FAIL;
}

TEST_CASE(ast_delete_reuse) {
    MAYCUP_AST ast;
    ssize_t root;
    const size_t cnt_1st = 10;
    const size_t cnt_2nd = 20;
    ssize_t node_1st[cnt_1st];
    ssize_t node_2nd[cnt_1st][cnt_2nd];

    ASSERT_OK(maycup_ast_ctor(&ast, &root), fail);
    for (size_t i = 0; i < cnt_1st - 1; ++i) {
        ASSERT_OK(
            maycup_insert_astnode(&node_1st[i], &ast, root, MAYCUP_ASTNODE_TYPE_NONE),
            fail);
        for (size_t j = 0; j < cnt_2nd; ++j) {
            ASSERT_OK(maycup_insert_astnode(&node_2nd[i][j], &ast, node_1st[i],
                                         MAYCUP_ASTNODE_TYPE_NONE),
                      fail);
            ast.data[node_2nd[i][j]].heading.level = (i + j) % UINT8_MAX;
        }
    }

    ASSERT_OK(maycup_delete_astnode(&ast, node_1st[7]), fail);
    ASSERT_EQ(ast.is_allocated[node_1st[7]], false, fail);
    for (size_t j = 0; j < cnt_2nd; ++j) {
        ASSERT_EQ(ast.is_allocated[node_2nd[7][j]], false, fail);
    }

    ASSERT_OK(maycup_insert_astnode(&node_1st[cnt_1st - 1], &ast, root,
                                 MAYCUP_ASTNODE_TYPE_NONE),
              fail);
    for (size_t j = 0; j < cnt_2nd; ++j) {
        ASSERT_OK(maycup_insert_astnode(&node_2nd[cnt_1st - 1][j], &ast,
                                     node_1st[cnt_1st - 1],
                                     MAYCUP_ASTNODE_TYPE_NONE),
                  fail);
    }

    ASSERT_EQ(ast.is_allocated[node_1st[7]], true, fail);
    for (size_t j = 0; j < cnt_2nd; ++j) {
        ASSERT_EQ(ast.is_allocated[node_2nd[7][j]], true, fail);
    }

    ASSERT_OK(maycup_ast_dtor(&ast), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_ast_dtor(&ast);
    return TEST_RESULT_FAIL;
}

TEST_CASE(ast_delete_directnode_2layers) {
    MAYCUP_AST ast;
    ssize_t root;
    const size_t cnt_1st = 5;
    const size_t cnt_2nd = 6;
    const size_t dest_1st = 3;
    ssize_t node_1st[cnt_1st];
    ssize_t node_2nd[cnt_1st][cnt_2nd];

    ASSERT_OK(maycup_ast_ctor(&ast, &root), fail);

    for (size_t i = 0; i < cnt_1st; ++i) {
        ASSERT_OK(
            maycup_insert_astnode(&node_1st[i], &ast, root, MAYCUP_ASTNODE_TYPE_NONE),
            fail);
    }
    for (size_t i = 0; i < cnt_1st; ++i) {
        for (size_t j = 0; j < cnt_2nd; ++j) {
            ASSERT_OK(maycup_insert_astnode(&node_2nd[i][j], &ast, node_1st[i],
                                         MAYCUP_ASTNODE_TYPE_NONE),
                      fail);
        }
    }

    for (size_t i = 0; i < cnt_1st; ++i) {
        ASSERT_EQ(ast.data[node_1st[i]].parent, root, fail);
        ASSERT_EQ(ast.data[node_1st[i]].child, node_2nd[i][cnt_2nd - 1], fail);
        ASSERT_EQ(ast.data[node_1st[i]].prev_sibling,
                  i == 0 ? -1 : node_1st[i - 1], fail);
        ASSERT_EQ(ast.data[node_1st[i]].next_sibling,
                  i == cnt_1st - 1 ? -1 : node_1st[i + 1], fail);
    }
    for (size_t i = 0; i < cnt_1st; ++i) {
        for (size_t j = 0; j < cnt_2nd; ++j) {
            ASSERT_EQ(ast.data[node_2nd[i][j]].parent, node_1st[i], fail);
            ASSERT_EQ(ast.data[node_2nd[i][j]].child, -1, fail);
            ASSERT_EQ(ast.data[node_2nd[i][j]].prev_sibling,
                      j == 0 ? -1 : node_2nd[i][j - 1], fail);
            ASSERT_EQ(ast.data[node_2nd[i][j]].next_sibling,
                      j == cnt_2nd - 1 ? -1 : node_2nd[i][j + 1], fail);
        }
    }

    ASSERT_OK(maycup_delete_astnode(&ast, node_1st[dest_1st]), fail);

    for (size_t i = 0; i < cnt_1st; ++i) {
        if (i == dest_1st) {
            continue;
        }
        ASSERT_EQ(ast.data[node_1st[i]].parent, root, fail);
        ASSERT_EQ(ast.data[node_1st[i]].child, node_2nd[i][cnt_2nd - 1], fail);
        if (abs((int)i - (int)dest_1st) <= 1) {
            if (dest_1st != 0 && i == dest_1st - 1) {
                ASSERT_EQ(ast.data[node_1st[i]].prev_sibling,
                          i == 0 ? -1 : node_1st[i - 1], fail);
                ASSERT_EQ(ast.data[node_1st[i]].next_sibling,
                          i == cnt_1st - 1 ? -1 : node_1st[i + 2], fail);
            } else if (i == dest_1st + 1) {
                ASSERT_EQ(ast.data[node_1st[i]].prev_sibling,
                          i == 0 ? -1 : node_1st[i - 2], fail);
                ASSERT_EQ(ast.data[node_1st[i]].next_sibling,
                          i == cnt_1st - 1 ? -1 : node_1st[i + 1], fail);
            }
        } else {
            ASSERT_EQ(ast.data[node_1st[i]].prev_sibling,
                      i == 0 ? -1 : node_1st[i - 1], fail);
            ASSERT_EQ(ast.data[node_1st[i]].next_sibling,
                      i == cnt_1st - 1 ? -1 : node_1st[i + 1], fail);
        }
    }
    for (size_t i = 0; i < cnt_1st; ++i) {
        if (i == dest_1st) {
            continue;
        }
        for (size_t j = 0; j < cnt_2nd; ++j) {
            ASSERT_EQ(ast.data[node_2nd[i][j]].parent, node_1st[i], fail);
            ASSERT_EQ(ast.data[node_2nd[i][j]].child, -1, fail);
            ASSERT_EQ(ast.data[node_2nd[i][j]].prev_sibling,
                      j == 0 ? -1 : node_2nd[i][j - 1], fail);
            ASSERT_EQ(ast.data[node_2nd[i][j]].next_sibling,
                      j == cnt_2nd - 1 ? -1 : node_2nd[i][j + 1], fail);
        }
    }

    ASSERT_OK(maycup_ast_dtor(&ast), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_ast_dtor(&ast);
    return TEST_RESULT_FAIL;
}

TEST_CASE(ast_delete_directnode_3layers) {
    MAYCUP_AST ast;
    ssize_t root;
    const size_t cnt_1st = 5;
    const size_t cnt_2nd = 6;
    const size_t cnt_3rd = 5;
    const size_t dest_1st = 1;
    ssize_t node_1st[cnt_1st];
    ssize_t node_2nd[cnt_1st][cnt_2nd];
    ssize_t node_3rd[cnt_1st][cnt_2nd][cnt_3rd];

    ASSERT_OK(maycup_ast_ctor(&ast, &root), fail);

    for (size_t i = 0; i < cnt_1st; ++i) {
        ASSERT_OK(
            maycup_insert_astnode(&node_1st[i], &ast, root, MAYCUP_ASTNODE_TYPE_NONE),
            fail);
        for (size_t j = 0; j < cnt_2nd; ++j) {
            ASSERT_OK(maycup_insert_astnode(&node_2nd[i][j], &ast, node_1st[i],
                                         MAYCUP_ASTNODE_TYPE_NONE),
                      fail);
            for (size_t k = 0; k < cnt_3rd; ++k) {
                ASSERT_OK(maycup_insert_astnode(&node_3rd[i][j][k], &ast,
                                             node_2nd[i][j],
                                             MAYCUP_ASTNODE_TYPE_NONE),
                          fail);
            }
        }
    }

    for (size_t i = 0; i < cnt_1st; ++i) {
        ASSERT_EQ(ast.data[node_1st[i]].parent, root, fail);
        ASSERT_EQ(ast.data[node_1st[i]].child, node_2nd[i][cnt_2nd - 1], fail);
        ASSERT_EQ(ast.data[node_1st[i]].prev_sibling,
                  i == 0 ? -1 : node_1st[i - 1], fail);
        ASSERT_EQ(ast.data[node_1st[i]].next_sibling,
                  i == cnt_1st - 1 ? -1 : node_1st[i + 1], fail);
        for (size_t j = 0; j < cnt_2nd; ++j) {
            ASSERT_EQ(ast.data[node_2nd[i][j]].parent, node_1st[i], fail);
            ASSERT_EQ(ast.data[node_2nd[i][j]].child,
                      node_3rd[i][j][cnt_3rd - 1], fail);
            ASSERT_EQ(ast.data[node_2nd[i][j]].prev_sibling,
                      j == 0 ? -1 : node_2nd[i][j - 1], fail);
            ASSERT_EQ(ast.data[node_2nd[i][j]].next_sibling,
                      j == cnt_2nd - 1 ? -1 : node_2nd[i][j + 1], fail);
            for (size_t k = 0; k < cnt_3rd; ++k) {
                ASSERT_EQ(ast.data[node_3rd[i][j][k]].parent, node_2nd[i][j],
                          fail);
                ASSERT_EQ(ast.data[node_3rd[i][j][k]].child, -1, fail);
                ASSERT_EQ(ast.data[node_3rd[i][j][k]].prev_sibling,
                          k == 0 ? -1 : node_3rd[i][j][k - 1], fail);
                ASSERT_EQ(ast.data[node_3rd[i][j][k]].next_sibling,
                          k == cnt_3rd - 1 ? -1 : node_3rd[i][j][k + 1], fail);
            }
        }
    }

    ASSERT_OK(maycup_delete_astnode(&ast, node_1st[dest_1st]), fail);

    for (size_t i = 0; i < cnt_1st; ++i) {
        if (i == dest_1st) {
            continue;
        }
        ASSERT_EQ(ast.data[node_1st[i]].parent, root, fail);
        ASSERT_EQ(ast.data[node_1st[i]].child, node_2nd[i][cnt_2nd - 1], fail);
        if (abs((int)i - (int)dest_1st) <= 1) {
            if (dest_1st != 0 && i == dest_1st - 1) {
                ASSERT_EQ(ast.data[node_1st[i]].prev_sibling,
                          i == 0 ? -1 : node_1st[i - 1], fail);
                ASSERT_EQ(ast.data[node_1st[i]].next_sibling,
                          i == cnt_1st - 1 ? -1 : node_1st[i + 2], fail);
            } else if (i == dest_1st + 1) {
                ASSERT_EQ(ast.data[node_1st[i]].prev_sibling,
                          i == 0 ? -1 : node_1st[i - 2], fail);
                ASSERT_EQ(ast.data[node_1st[i]].next_sibling,
                          i == cnt_1st - 1 ? -1 : node_1st[i + 1], fail);
            }
        } else {
            ASSERT_EQ(ast.data[node_1st[i]].prev_sibling,
                      i == 0 ? -1 : node_1st[i - 1], fail);
            ASSERT_EQ(ast.data[node_1st[i]].next_sibling,
                      i == cnt_1st - 1 ? -1 : node_1st[i + 1], fail);
            for (size_t j = 0; j < cnt_2nd; ++j) {
                ASSERT_EQ(ast.data[node_2nd[i][j]].parent, node_1st[i], fail);
                ASSERT_EQ(ast.data[node_2nd[i][j]].child,
                          node_3rd[i][j][cnt_3rd - 1], fail);
                ASSERT_EQ(ast.data[node_2nd[i][j]].prev_sibling,
                          j == 0 ? -1 : node_2nd[i][j - 1], fail);
                ASSERT_EQ(ast.data[node_2nd[i][j]].next_sibling,
                          j == cnt_2nd - 1 ? -1 : node_2nd[i][j + 1], fail);
                for (size_t k = 0; k < cnt_3rd; ++k) {
                    ASSERT_EQ(ast.data[node_3rd[i][j][k]].parent,
                              node_2nd[i][j], fail);
                    ASSERT_EQ(ast.data[node_3rd[i][j][k]].child, -1, fail);
                    ASSERT_EQ(ast.data[node_3rd[i][j][k]].prev_sibling,
                              k == 0 ? -1 : node_3rd[i][j][k - 1], fail);
                    ASSERT_EQ(ast.data[node_3rd[i][j][k]].next_sibling,
                              k == cnt_3rd - 1 ? -1 : node_3rd[i][j][k + 1],
                              fail);
                }
            }
        }
    }

    ASSERT_OK(maycup_ast_dtor(&ast), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_ast_dtor(&ast);
    return TEST_RESULT_FAIL;
}

TEST_CASE(ast_delete_middlenode) {
    MAYCUP_AST ast;
    ssize_t root;
    const size_t cnt_1st = 5;
    const size_t cnt_2nd = 6;
    const size_t cnt_3rd = 5;
    const size_t dest_1st = 2;
    const size_t dest_2nd = 3;
    ssize_t node_1st[cnt_1st];
    ssize_t node_2nd[cnt_1st][cnt_2nd];
    ssize_t node_3rd[cnt_1st][cnt_2nd][cnt_3rd];

    ASSERT_OK(maycup_ast_ctor(&ast, &root), fail);

    for (size_t i = 0; i < cnt_1st; ++i) {
        ASSERT_OK(
            maycup_insert_astnode(&node_1st[i], &ast, root, MAYCUP_ASTNODE_TYPE_NONE),
            fail);
        for (size_t j = 0; j < cnt_2nd; ++j) {
            ASSERT_OK(maycup_insert_astnode(&node_2nd[i][j], &ast, node_1st[i],
                                         MAYCUP_ASTNODE_TYPE_NONE),
                      fail);
            for (size_t k = 0; k < cnt_3rd; ++k) {
                ASSERT_OK(maycup_insert_astnode(&node_3rd[i][j][k], &ast,
                                             node_2nd[i][j],
                                             MAYCUP_ASTNODE_TYPE_NONE),
                          fail);
            }
        }
    }

    for (size_t i = 0; i < cnt_1st; ++i) {
        ASSERT_EQ(ast.data[node_1st[i]].parent, root, fail);
        ASSERT_EQ(ast.data[node_1st[i]].child, node_2nd[i][cnt_2nd - 1], fail);
        ASSERT_EQ(ast.data[node_1st[i]].prev_sibling,
                  i == 0 ? -1 : node_1st[i - 1], fail);
        ASSERT_EQ(ast.data[node_1st[i]].next_sibling,
                  i == cnt_1st - 1 ? -1 : node_1st[i + 1], fail);
        for (size_t j = 0; j < cnt_2nd; ++j) {
            ASSERT_EQ(ast.data[node_2nd[i][j]].parent, node_1st[i], fail);
            ASSERT_EQ(ast.data[node_2nd[i][j]].child,
                      node_3rd[i][j][cnt_3rd - 1], fail);
            ASSERT_EQ(ast.data[node_2nd[i][j]].prev_sibling,
                      j == 0 ? -1 : node_2nd[i][j - 1], fail);
            ASSERT_EQ(ast.data[node_2nd[i][j]].next_sibling,
                      j == cnt_2nd - 1 ? -1 : node_2nd[i][j + 1], fail);
            for (size_t k = 0; k < cnt_3rd; ++k) {
                ASSERT_EQ(ast.data[node_3rd[i][j][k]].parent, node_2nd[i][j],
                          fail);
                ASSERT_EQ(ast.data[node_3rd[i][j][k]].child, -1, fail);
                ASSERT_EQ(ast.data[node_3rd[i][j][k]].prev_sibling,
                          k == 0 ? -1 : node_3rd[i][j][k - 1], fail);
                ASSERT_EQ(ast.data[node_3rd[i][j][k]].next_sibling,
                          k == cnt_3rd - 1 ? -1 : node_3rd[i][j][k + 1], fail);
            }
        }
    }

    ASSERT_OK(maycup_delete_astnode(&ast, node_2nd[dest_1st][dest_2nd]), fail);

    for (size_t i = 0; i < cnt_1st; ++i) {
        ASSERT_EQ(ast.data[node_1st[i]].parent, root, fail);
        ASSERT_EQ(ast.data[node_1st[i]].child, node_2nd[i][cnt_2nd - 1], fail);
        ASSERT_EQ(ast.data[node_1st[i]].prev_sibling,
                  i == 0 ? -1 : node_1st[i - 1], fail);
        ASSERT_EQ(ast.data[node_1st[i]].next_sibling,
                  i == cnt_1st - 1 ? -1 : node_1st[i + 1], fail);
        for (size_t j = 0; j < cnt_2nd; ++j) {
            if (j == dest_2nd) {
                continue;
            }
            ASSERT_EQ(ast.data[node_2nd[i][j]].parent, node_1st[i], fail);
            ASSERT_EQ(ast.data[node_2nd[i][j]].child,
                      node_3rd[i][j][cnt_3rd - 1], fail);
            if (i == dest_1st && abs((int)j - (int)dest_2nd) <= 1) {
                if (dest_2nd != 0 && j == dest_2nd - 1) {
                    ASSERT_EQ(ast.data[node_2nd[i][j]].prev_sibling,
                              j == 0 ? -1 : node_2nd[i][j - 1], fail);
                    ASSERT_EQ(ast.data[node_2nd[i][j]].next_sibling,
                              j == cnt_2nd - 1 ? -1 : node_2nd[i][j + 2], fail);
                } else if (j == dest_2nd + 1) {
                    ASSERT_EQ(ast.data[node_2nd[i][j]].prev_sibling,
                              j == 0 ? -1 : node_2nd[i][j - 2], fail);
                    ASSERT_EQ(ast.data[node_2nd[i][j]].next_sibling,
                              j == cnt_2nd - 1 ? -1 : node_2nd[i][j + 1], fail);
                }
            } else {
                ASSERT_EQ(ast.data[node_2nd[i][j]].prev_sibling,
                          j == 0 ? -1 : node_2nd[i][j - 1], fail);
                ASSERT_EQ(ast.data[node_2nd[i][j]].next_sibling,
                          j == cnt_2nd - 1 ? -1 : node_2nd[i][j + 1], fail);
                for (size_t k = 0; k < cnt_3rd; ++k) {
                    ASSERT_EQ(ast.data[node_3rd[i][j][k]].parent,
                              node_2nd[i][j], fail);
                    ASSERT_EQ(ast.data[node_3rd[i][j][k]].child, -1, fail);
                    ASSERT_EQ(ast.data[node_3rd[i][j][k]].prev_sibling,
                              k == 0 ? -1 : node_3rd[i][j][k - 1], fail);
                    ASSERT_EQ(ast.data[node_3rd[i][j][k]].next_sibling,
                              k == cnt_3rd - 1 ? -1 : node_3rd[i][j][k + 1],
                              fail);
                }
            }
        }
    }

    ASSERT_OK(maycup_ast_dtor(&ast), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_ast_dtor(&ast);
    return TEST_RESULT_FAIL;
}

TEST_CASE(ast_delete_leafnode) {
    MAYCUP_AST ast;
    ssize_t root;
    const size_t cnt_1st = 5;
    const size_t cnt_2nd = 6;
    const size_t dest_1st = 2;
    const size_t dest_2nd = 3;
    ssize_t node_1st[cnt_1st];
    ssize_t node_2nd[cnt_1st][cnt_2nd];

    ASSERT_OK(maycup_ast_ctor(&ast, &root), fail);

    for (size_t i = 0; i < cnt_1st; ++i) {
        ASSERT_OK(
            maycup_insert_astnode(&node_1st[i], &ast, root, MAYCUP_ASTNODE_TYPE_NONE),
            fail);
    }
    for (size_t i = 0; i < cnt_1st; ++i) {
        for (size_t j = 0; j < cnt_2nd; ++j) {
            ASSERT_OK(maycup_insert_astnode(&node_2nd[i][j], &ast, node_1st[i],
                                         MAYCUP_ASTNODE_TYPE_NONE),
                      fail);
        }
    }

    for (size_t i = 0; i < cnt_1st; ++i) {
        ASSERT_EQ(ast.data[node_1st[i]].parent, root, fail);
        ASSERT_EQ(ast.data[node_1st[i]].child, node_2nd[i][cnt_2nd - 1], fail);
        ASSERT_EQ(ast.data[node_1st[i]].prev_sibling,
                  i == 0 ? -1 : node_1st[i - 1], fail);
        ASSERT_EQ(ast.data[node_1st[i]].next_sibling,
                  i == cnt_1st - 1 ? -1 : node_1st[i + 1], fail);
    }
    for (size_t i = 0; i < cnt_1st; ++i) {
        for (size_t j = 0; j < cnt_2nd; ++j) {
            ASSERT_EQ(ast.data[node_2nd[i][j]].parent, node_1st[i], fail);
            ASSERT_EQ(ast.data[node_2nd[i][j]].child, -1, fail);
            ASSERT_EQ(ast.data[node_2nd[i][j]].prev_sibling,
                      j == 0 ? -1 : node_2nd[i][j - 1], fail);
            ASSERT_EQ(ast.data[node_2nd[i][j]].next_sibling,
                      j == cnt_2nd - 1 ? -1 : node_2nd[i][j + 1], fail);
        }
    }

    ASSERT_OK(maycup_delete_astnode(&ast, node_2nd[dest_1st][dest_2nd]), fail);

    for (size_t i = 0; i < cnt_1st; ++i) {
        ASSERT_EQ(ast.data[node_1st[i]].parent, root, fail);
        ASSERT_EQ(ast.data[node_1st[i]].child, node_2nd[i][cnt_2nd - 1], fail);
        ASSERT_EQ(ast.data[node_1st[i]].prev_sibling,
                  i == 0 ? -1 : node_1st[i - 1], fail);
        ASSERT_EQ(ast.data[node_1st[i]].next_sibling,
                  i == cnt_1st - 1 ? -1 : node_1st[i + 1], fail);
    }
    for (size_t i = 0; i < cnt_1st; ++i) {
        for (size_t j = 0; j < cnt_2nd; ++j) {
            if (j == dest_2nd) {
                continue;
            }
            ASSERT_EQ(ast.data[node_2nd[i][j]].parent, node_1st[i], fail);
            ASSERT_EQ(ast.data[node_2nd[i][j]].child, -1, fail);
            if (i == dest_1st && abs((int)j - (int)dest_2nd) <= 1) {
                if (dest_2nd != 0 && j == dest_2nd - 1) {
                    ASSERT_EQ(ast.data[node_2nd[i][j]].prev_sibling,
                              j == 0 ? -1 : node_2nd[i][j - 1], fail);
                    ASSERT_EQ(ast.data[node_2nd[i][j]].next_sibling,
                              j == cnt_2nd - 1 ? -1 : node_2nd[i][j + 2], fail);
                } else if (j == dest_2nd + 1) {
                    ASSERT_EQ(ast.data[node_2nd[i][j]].prev_sibling,
                              j == 0 ? -1 : node_2nd[i][j - 2], fail);
                    ASSERT_EQ(ast.data[node_2nd[i][j]].next_sibling,
                              j == cnt_2nd - 1 ? -1 : node_2nd[i][j + 1], fail);
                }
            } else {
                ASSERT_EQ(ast.data[node_2nd[i][j]].prev_sibling,
                          j == 0 ? -1 : node_2nd[i][j - 1], fail);
                ASSERT_EQ(ast.data[node_2nd[i][j]].next_sibling,
                          j == cnt_2nd - 1 ? -1 : node_2nd[i][j + 1], fail);
            }
        }
    }

    ASSERT_OK(maycup_ast_dtor(&ast), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_ast_dtor(&ast);
    return TEST_RESULT_FAIL;
}

TEST_CASE(ast_delete_illegal_arg) {
    MAYCUP_AST ast;
    ssize_t root;
    ASSERT_OK(maycup_ast_ctor(&ast, &root), fail);

    ASSERT_EQ(maycup_delete_astnode(NULL, root), MAYCUP_RESULT_ILLEGAL_ARGUMENT,
              fail);
    ASSERT_EQ(maycup_delete_astnode(&ast, 0), MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);
    ASSERT_EQ(maycup_delete_astnode(&ast, MAYCUP_MAX_AST_CAP + 10),
              MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);
    ASSERT_EQ(maycup_delete_astnode(&ast, MAYCUP_MAX_AST_CAP - 1),
              MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);

    ASSERT_OK(maycup_ast_dtor(&ast), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_ast_dtor(&ast);
    return TEST_RESULT_FAIL;
}

TEST_CASE(astnode_text_ctor_normal) {
    MAYCUP_ASTNodeDataText andt1;
    MAYCUP_ASTNodeDataText andt2;
    char *text1 = strdup("123");
    char *text2 = strdup("test");

    ASSERT_OK(
        maycup_astnode_data_text_ctor(&andt1, text1, MAYCUP_TEXTSTYLE_PLAIN, false),
        fail);
    ASSERT_EQ(strcmp(andt1.content, "123"), 0, fail);
    ASSERT_EQ(andt1.style, MAYCUP_TEXTSTYLE_PLAIN, fail);
    ASSERT_EQ(andt1.newline_tailed, false, fail);

    ASSERT_OK(maycup_astnode_data_text_ctor(&andt2, text2,
                                         MAYCUP_TEXTSTYLE_BOLDITALIC, true),
              fail);
    ASSERT_EQ(strcmp(andt2.content, "test"), 0, fail);
    ASSERT_EQ(andt2.style, MAYCUP_TEXTSTYLE_BOLDITALIC, fail);
    ASSERT_EQ(andt2.newline_tailed, true, fail);

    ASSERT_OK(maycup_astnode_data_text_dtor(&andt1), fail);
    ASSERT_OK(maycup_astnode_data_text_dtor(&andt2), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_astnode_data_text_dtor(&andt1);
    maycup_astnode_data_text_dtor(&andt2);
    return TEST_RESULT_FAIL;
}

TEST_CASE(astnode_text_ctor_illegal_arg) {
    MAYCUP_ASTNodeDataText andt;
    char *text = strdup("12345");

    ASSERT_EQ(
        maycup_astnode_data_text_ctor(NULL, text, MAYCUP_TEXTSTYLE_PLAIN, false),
        MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);
    ASSERT_EQ(
        maycup_astnode_data_text_ctor(&andt, NULL, MAYCUP_TEXTSTYLE_PLAIN, false),
        MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);

    free(text);
    return TEST_RESULT_PASS;
fail:
    free(text);
    return TEST_RESULT_FAIL;
}

TEST_CASE(astnode_text_dtor_normal) {
    MAYCUP_ASTNodeDataText andt;
    char *text = strdup("12345");

    ASSERT_OK(
        maycup_astnode_data_text_ctor(&andt, text, MAYCUP_TEXTSTYLE_PLAIN, false),
        fail);
    ASSERT_OK(maycup_astnode_data_text_dtor(&andt), fail);
    ASSERT_EQ(andt.content, NULL, fail);

    return TEST_RESULT_PASS;
fail:
    return TEST_RESULT_FAIL;
}

TEST_CASE(astnode_text_dtor_double) {
    MAYCUP_ASTNodeDataText andt;
    char *text = strdup("12345");

    ASSERT_OK(
        maycup_astnode_data_text_ctor(&andt, text, MAYCUP_TEXTSTYLE_PLAIN, false),
        fail);
    ASSERT_OK(maycup_astnode_data_text_dtor(&andt), fail);
    ASSERT_OK(maycup_astnode_data_text_dtor(&andt), fail);

    return TEST_RESULT_PASS;
fail:
    return TEST_RESULT_FAIL;
}

TEST_CASE(astnode_text_dtor_illegal_arg) {
    ASSERT_EQ(maycup_astnode_data_text_dtor(NULL), MAYCUP_RESULT_ILLEGAL_ARGUMENT,
              fail);
    return TEST_RESULT_PASS;
fail:
    return TEST_RESULT_FAIL;
}

TEST_CASE(astnode_dtor_normal) {
    MAYCUP_ASTNode node;
    node.type = MAYCUP_ASTNODE_TYPE_TEXT;

    ASSERT_OK(maycup_astnode_data_text_ctor(&node.text, strdup("12345"),
                                         MAYCUP_TEXTSTYLE_PLAIN, false),
              fail);
    ASSERT_OK(maycup_astnode_dtor(&node), fail);
    ASSERT_EQ(node.text.content, NULL, fail);

    return TEST_RESULT_PASS;
fail:
    return TEST_RESULT_FAIL;
}

TEST_CASE(astnode_dtor_double) {
    MAYCUP_ASTNode node;
    node.type = MAYCUP_ASTNODE_TYPE_TEXT;

    ASSERT_OK(maycup_astnode_data_text_ctor(&node.text, strdup("12345"),
                                         MAYCUP_TEXTSTYLE_PLAIN, false),
              fail);
    ASSERT_OK(maycup_astnode_dtor(&node), fail);
    ASSERT_OK(maycup_astnode_dtor(&node), fail);

    return TEST_RESULT_PASS;
fail:
    return TEST_RESULT_FAIL;
}

TEST_CASE(astnode_dtor_illegal_arg) {
    ASSERT_EQ(maycup_astnode_dtor(NULL), MAYCUP_RESULT_ILLEGAL_ARGUMENT, fail);
    return TEST_RESULT_PASS;
fail:
    return TEST_RESULT_FAIL;
}

TEST_CASE(ast_module_common) {
    MAYCUP_AST ast;
    ssize_t root;
    ssize_t node[8];

    ASSERT_OK(maycup_ast_ctor(&ast, &root), fail);

    ASSERT_OK(
        maycup_insert_astnode(&node[0], &ast, root, MAYCUP_ASTNODE_TYPE_HEADING),
        fail);
    ast.data[node[0]].heading.level = 5;
    ASSERT_OK(
        maycup_insert_astnode(&node[1], &ast, node[0], MAYCUP_ASTNODE_TYPE_TEXT),
        fail);
    ASSERT_OK(maycup_astnode_data_text_ctor(&ast.data[node[1]].text,
                                         strdup("heading"), MAYCUP_TEXTSTYLE_PLAIN,
                                         false),
              fail);
    ASSERT_OK(
        maycup_insert_astnode(&node[2], &ast, root, MAYCUP_ASTNODE_TYPE_PARAGRAPH),
        fail);
    ASSERT_OK(
        maycup_insert_astnode(&node[3], &ast, node[2], MAYCUP_ASTNODE_TYPE_TEXT),
        fail);
    ASSERT_OK(maycup_astnode_data_text_ctor(&ast.data[node[3]].text,
                                         strdup("text1"), MAYCUP_TEXTSTYLE_PLAIN,
                                         false),
              fail);
    ASSERT_OK(
        maycup_insert_astnode(&node[4], &ast, node[2], MAYCUP_ASTNODE_TYPE_TEXT),
        fail);
    ASSERT_OK(maycup_astnode_data_text_ctor(&ast.data[node[4]].text,
                                         strdup("text2"), MAYCUP_TEXTSTYLE_PLAIN,
                                         true),
              fail);

    ASSERT_EQ(ast.data[root].child, node[2], fail);

    ASSERT_EQ(ast.data[node[0]].parent, root, fail);
    ASSERT_EQ(ast.data[node[0]].child, node[1], fail);
    ASSERT_EQ(ast.data[node[0]].prev_sibling, -1, fail);
    ASSERT_EQ(ast.data[node[0]].next_sibling, node[2], fail);
    ASSERT_EQ(ast.data[node[0]].heading.level, 5, fail);
    ASSERT_EQ(ast.data[node[0]].type, MAYCUP_ASTNODE_TYPE_HEADING, fail);
    ASSERT_EQ(ast.data[node[1]].parent, node[0], fail);
    ASSERT_EQ(ast.data[node[1]].child, -1, fail);
    ASSERT_EQ(ast.data[node[1]].prev_sibling, -1, fail);
    ASSERT_EQ(ast.data[node[1]].next_sibling, -1, fail);
    ASSERT_EQ(ast.data[node[1]].type, MAYCUP_ASTNODE_TYPE_TEXT, fail);
    ASSERT_EQ(strcmp(ast.data[node[1]].text.content, "heading"), 0, fail);
    ASSERT_EQ(ast.data[node[1]].text.newline_tailed, false, fail);
    ASSERT_EQ(ast.data[node[1]].text.style, MAYCUP_TEXTSTYLE_PLAIN, fail);

    ASSERT_EQ(ast.data[node[2]].parent, root, fail);
    ASSERT_EQ(ast.data[node[2]].child, node[4], fail);
    ASSERT_EQ(ast.data[node[2]].prev_sibling, node[0], fail);
    ASSERT_EQ(ast.data[node[2]].next_sibling, -1, fail);
    ASSERT_EQ(ast.data[node[2]].type, MAYCUP_ASTNODE_TYPE_PARAGRAPH, fail);
    ASSERT_EQ(ast.data[node[3]].parent, node[2], fail);
    ASSERT_EQ(ast.data[node[3]].child, -1, fail);
    ASSERT_EQ(ast.data[node[3]].prev_sibling, -1, fail);
    ASSERT_EQ(ast.data[node[3]].next_sibling, node[4], fail);
    ASSERT_EQ(ast.data[node[3]].type, MAYCUP_ASTNODE_TYPE_TEXT, fail);
    ASSERT_EQ(strcmp(ast.data[node[3]].text.content, "text1"), 0, fail);
    ASSERT_EQ(ast.data[node[3]].text.newline_tailed, false, fail);
    ASSERT_EQ(ast.data[node[3]].text.style, MAYCUP_TEXTSTYLE_PLAIN, fail);
    ASSERT_EQ(ast.data[node[4]].parent, node[2], fail);
    ASSERT_EQ(ast.data[node[4]].child, -1, fail);
    ASSERT_EQ(ast.data[node[4]].prev_sibling, node[3], fail);
    ASSERT_EQ(ast.data[node[4]].next_sibling, -1, fail);
    ASSERT_EQ(ast.data[node[4]].type, MAYCUP_ASTNODE_TYPE_TEXT, fail);
    ASSERT_EQ(strcmp(ast.data[node[4]].text.content, "text2"), 0, fail);
    ASSERT_EQ(ast.data[node[4]].text.newline_tailed, true, fail);
    ASSERT_EQ(ast.data[node[4]].text.style, MAYCUP_TEXTSTYLE_PLAIN, fail);

    ASSERT_EQ(ast.is_allocated[node[2]], true, fail);
    ASSERT_EQ(ast.is_allocated[node[3]], true, fail);
    ASSERT_EQ(ast.is_allocated[node[4]], true, fail);

    ASSERT_OK(maycup_delete_astnode(&ast, node[2]), fail);

    ASSERT_EQ(ast.data[root].child, node[0], fail);

    ASSERT_EQ(ast.data[node[0]].parent, root, fail);
    ASSERT_EQ(ast.data[node[0]].child, node[1], fail);
    ASSERT_EQ(ast.data[node[0]].prev_sibling, -1, fail);
    ASSERT_EQ(ast.data[node[0]].next_sibling, -1, fail);
    ASSERT_EQ(ast.data[node[0]].heading.level, 5, fail);
    ASSERT_EQ(ast.data[node[0]].type, MAYCUP_ASTNODE_TYPE_HEADING, fail);
    ASSERT_EQ(ast.data[node[1]].parent, node[0], fail);
    ASSERT_EQ(ast.data[node[1]].child, -1, fail);
    ASSERT_EQ(ast.data[node[1]].prev_sibling, -1, fail);
    ASSERT_EQ(ast.data[node[1]].next_sibling, -1, fail);
    ASSERT_EQ(ast.data[node[1]].type, MAYCUP_ASTNODE_TYPE_TEXT, fail);
    ASSERT_EQ(strcmp(ast.data[node[1]].text.content, "heading"), 0, fail);
    ASSERT_EQ(ast.data[node[1]].text.newline_tailed, false, fail);
    ASSERT_EQ(ast.data[node[1]].text.style, MAYCUP_TEXTSTYLE_PLAIN, fail);

    ASSERT_EQ(ast.is_allocated[node[2]], false, fail);
    ASSERT_EQ(ast.is_allocated[node[3]], false, fail);
    ASSERT_EQ(ast.is_allocated[node[4]], false, fail);

    ASSERT_OK(
        maycup_insert_astnode(&node[5], &ast, root, MAYCUP_ASTNODE_TYPE_HEADING),
        fail);
    ast.data[node[5]].heading.level = 3;
    ASSERT_OK(
        maycup_insert_astnode(&node[6], &ast, node[5], MAYCUP_ASTNODE_TYPE_TEXT),
        fail);
    ASSERT_OK(maycup_astnode_data_text_ctor(&ast.data[node[6]].text,
                                         strdup("newheading"),
                                         MAYCUP_TEXTSTYLE_PLAIN, false),
              fail);
    ASSERT_OK(
        maycup_insert_astnode(&node[7], &ast, node[5], MAYCUP_ASTNODE_TYPE_TEXT),
        fail);
    ASSERT_OK(maycup_astnode_data_text_ctor(&ast.data[node[7]].text,
                                         strdup("newheading233"),
                                         MAYCUP_TEXTSTYLE_PLAIN, false),
              fail);

    ASSERT_EQ(ast.data[root].child, node[5], fail);

    ASSERT_EQ(ast.data[node[0]].parent, root, fail);
    ASSERT_EQ(ast.data[node[0]].child, node[1], fail);
    ASSERT_EQ(ast.data[node[0]].prev_sibling, -1, fail);
    ASSERT_EQ(ast.data[node[0]].next_sibling, node[5], fail);
    ASSERT_EQ(ast.data[node[0]].heading.level, 5, fail);
    ASSERT_EQ(ast.data[node[0]].type, MAYCUP_ASTNODE_TYPE_HEADING, fail);
    ASSERT_EQ(ast.data[node[1]].parent, node[0], fail);
    ASSERT_EQ(ast.data[node[1]].child, -1, fail);
    ASSERT_EQ(ast.data[node[1]].prev_sibling, -1, fail);
    ASSERT_EQ(ast.data[node[1]].next_sibling, -1, fail);
    ASSERT_EQ(ast.data[node[1]].type, MAYCUP_ASTNODE_TYPE_TEXT, fail);
    ASSERT_EQ(strcmp(ast.data[node[1]].text.content, "heading"), 0, fail);
    ASSERT_EQ(ast.data[node[1]].text.newline_tailed, false, fail);
    ASSERT_EQ(ast.data[node[1]].text.style, MAYCUP_TEXTSTYLE_PLAIN, fail);

    ASSERT_EQ(ast.data[node[5]].parent, root, fail);
    ASSERT_EQ(ast.data[node[5]].child, node[7], fail);
    ASSERT_EQ(ast.data[node[5]].prev_sibling, node[0], fail);
    ASSERT_EQ(ast.data[node[5]].next_sibling, -1, fail);
    ASSERT_EQ(ast.data[node[5]].heading.level, 3, fail);
    ASSERT_EQ(ast.data[node[5]].type, MAYCUP_ASTNODE_TYPE_HEADING, fail);
    ASSERT_EQ(ast.data[node[6]].parent, node[5], fail);
    ASSERT_EQ(ast.data[node[6]].child, -1, fail);
    ASSERT_EQ(ast.data[node[6]].prev_sibling, -1, fail);
    ASSERT_EQ(ast.data[node[6]].next_sibling, node[7], fail);
    ASSERT_EQ(ast.data[node[6]].type, MAYCUP_ASTNODE_TYPE_TEXT, fail);
    ASSERT_EQ(strcmp(ast.data[node[6]].text.content, "newheading"), 0, fail);
    ASSERT_EQ(ast.data[node[6]].text.newline_tailed, false, fail);
    ASSERT_EQ(ast.data[node[6]].text.style, MAYCUP_TEXTSTYLE_PLAIN, fail);
    ASSERT_EQ(ast.data[node[6]].parent, node[5], fail);
    ASSERT_EQ(ast.data[node[7]].child, -1, fail);
    ASSERT_EQ(ast.data[node[7]].prev_sibling, node[6], fail);
    ASSERT_EQ(ast.data[node[7]].next_sibling, -1, fail);
    ASSERT_EQ(ast.data[node[7]].type, MAYCUP_ASTNODE_TYPE_TEXT, fail);
    ASSERT_EQ(strcmp(ast.data[node[7]].text.content, "newheading233"), 0, fail);
    ASSERT_EQ(ast.data[node[7]].text.newline_tailed, false, fail);
    ASSERT_EQ(ast.data[node[7]].text.style, MAYCUP_TEXTSTYLE_PLAIN, fail);

    ASSERT_EQ(ast.is_allocated[node[2]], true, fail);
    ASSERT_EQ(ast.is_allocated[node[3]], true, fail);
    ASSERT_EQ(ast.is_allocated[node[4]], true, fail);

    ASSERT_OK(maycup_ast_dtor(&ast), fail);
    return TEST_RESULT_PASS;
fail:
    maycup_ast_dtor(&ast);
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
TEST_CASE_ADD(ast_delete_reuse);
TEST_CASE_ADD(ast_delete_directnode_2layers);
TEST_CASE_ADD(ast_delete_directnode_3layers);
TEST_CASE_ADD(ast_delete_middlenode);
TEST_CASE_ADD(ast_delete_leafnode);
TEST_CASE_ADD(ast_delete_illegal_arg);

TEST_CASE_ADD(astnode_text_ctor_normal);
TEST_CASE_ADD(astnode_text_ctor_illegal_arg);

TEST_CASE_ADD(astnode_text_dtor_normal);
TEST_CASE_ADD(astnode_text_dtor_double);
TEST_CASE_ADD(astnode_text_dtor_illegal_arg);

TEST_CASE_ADD(astnode_dtor_normal);
TEST_CASE_ADD(astnode_dtor_double);
TEST_CASE_ADD(astnode_dtor_illegal_arg);

TEST_CASE_ADD(ast_module_common);

TEST_SUITE_END
