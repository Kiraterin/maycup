/**
 * @file ast.c
 * @brief Abstract syntax tree
 * @date 2026-07-12
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

typedef ssize_t idx;
#define M2H_VEC_T idx
#define M2H_VEC_DISPT Idx
#include "md2html/base/vector.h"
#undef M2H_VEC_DISPT
#undef M2H_VEC_T

M2H_Result M2H_astnode_dtor(M2H_OUT M2H_ASTNode *self) {
    switch (self->type) {
    case M2H_ASTNODE_TYPE_TEXT:
        M2H_RELAY(M2H_astnode_data_text_dtor(&self->text));
        break;
    default:
        break;
    }
    return M2H_RESULT_OK;
}

M2H_Result M2H_ast_ctor(M2H_OUT M2H_AST *self, M2H_OUT ssize_t *head) {
    self->data =
        (M2H_ASTNode *)malloc(M2H_DEFAULT_AST_SIZE * sizeof(M2H_ASTNode));
    self->next_free = (ssize_t *)malloc(M2H_DEFAULT_AST_SIZE * sizeof(ssize_t));
    self->is_allocated = (bool *)malloc(M2H_DEFAULT_AST_SIZE * sizeof(bool));
    if (self->data == NULL || self->next_free == NULL ||
        self->is_allocated == NULL) {
        return M2H_RESULT_MALLOC_FAIL;
    }

    self->cap = M2H_DEFAULT_AST_SIZE;

    *head = 1;
    self->data[0] = (M2H_ASTNode){
        .type = M2H_ASTNODE_TYPE_NONE,
        .prev_sibling = -1,
        .next_sibling = -1,
        .parent = -1,
        .child = -1,
    };
    self->data[*head] = (M2H_ASTNode){
        .type = M2H_ASTNODE_TYPE_ROOT,
        .prev_sibling = -1,
        .next_sibling = -1,
        .parent = -1,
        .child = -1,
    };

    for (size_t i = 0; i < self->cap; ++i) {
        self->next_free[i] = i + 1;
        self->is_allocated[i] = false;
    }
    self->next_free[self->cap - 1] = -1;
    self->is_allocated[0] = true;
    self->is_allocated[*head] = true;
    self->first_free = *head + 1;

    return M2H_RESULT_OK;
}

M2H_Result M2H_ast_dtor(M2H_OUT M2H_AST *self) {
    for (size_t i = 0; i < self->cap; ++i) {
        if (self->is_allocated[i]) {
            M2H_ASTNode *cur = &self->data[i];
            switch (cur->type) {
            case M2H_ASTNODE_TYPE_TEXT:
                M2H_astnode_data_text_dtor(&cur->text);
                break;
            default:
                break;
            }
        }
    }

    free(self->data);
    self->data = NULL;
    free(self->next_free);
    self->next_free = NULL;
    free(self->is_allocated);
    self->is_allocated = NULL;
    self->cap = 0;
    self->first_free = -1;

    return M2H_RESULT_OK;
}

M2H_Result M2H_insert_astnode(M2H_OUT ssize_t *insertee, M2H_OUT M2H_AST *ast,
                              M2H_INOUT ssize_t parent,
                              M2H_IN M2H_ASTNodeType type) {
    if (ast->first_free == -1) {
        M2H_ASTNode *new_data = (M2H_ASTNode *)realloc(
            ast->data, ast->cap * 2 * sizeof(M2H_ASTNode));
        ssize_t *new_next_free =
            (ssize_t *)realloc(ast->next_free, ast->cap * 2 * sizeof(ssize_t));
        bool *new_is_allocated =
            (bool *)realloc(ast->is_allocated, ast->cap * 2 * sizeof(bool));
        if (new_data == NULL || new_next_free == NULL ||
            new_is_allocated == NULL) {
            return M2H_RESULT_MALLOC_FAIL;
        }
        ast->data = new_data;
        ast->next_free = new_next_free;
        ast->is_allocated = new_is_allocated;

        ast->first_free = ast->cap;
        ast->cap *= 2;

        for (size_t i = ast->first_free; i < ast->cap; ++i) {
            ast->next_free[i] = i + 1;
            ast->is_allocated[i] = false;
        }
        ast->next_free[ast->cap - 1] = -1;
    }

    if (ast->is_allocated[ast->first_free]) {
        return M2H_RESULT_ARENA_ERROR;
    }
    ssize_t _insertee = ast->first_free;
    ast->is_allocated[ast->first_free] = true;
    ast->first_free = ast->next_free[ast->first_free];

    ssize_t old_child = ast->data[parent].child;
    if (old_child != -1) {
        ast->data[old_child].next_sibling = _insertee;
    }
    ast->data[parent].child = _insertee;
    ast->data[_insertee] = (M2H_ASTNode){.type = type,
                                         .prev_sibling = old_child,
                                         .next_sibling = -1,
                                         .parent = parent,
                                         .child = -1};

    if (insertee != NULL) {
        *insertee = _insertee;
    }

    return M2H_RESULT_OK;
}

M2H_Result M2H_delete_astnode(M2H_OUT M2H_AST *ast, M2H_INOUT ssize_t dest) {
    M2H_ASTNode *const p_dest = &ast->data[dest];
    if (p_dest->next_sibling != -1) {
        ast->data[p_dest->next_sibling].prev_sibling =
            p_dest->prev_sibling;
    }
    if (p_dest->prev_sibling != -1) {
        ast->data[p_dest->prev_sibling].next_sibling =
            p_dest->next_sibling;
    }
    if (ast->data[p_dest->parent].child == dest) {
        ast->data[p_dest->parent].child = p_dest->prev_sibling;
    }

    M2H_VectorIdx stack;
    M2H_RELAY(M2H_vector_idx_ctor(&stack, M2H_DEFAULT_VEC_SIZE));
    if (p_dest->child != -1) {
        M2H_RELAY(M2H_vector_idx_pushback(&stack, p_dest->child));
    }
    M2H_RELAY(M2H_astnode_dtor(p_dest));
    ast->next_free[dest] = ast->first_free;
    ast->is_allocated[dest] = false;
    ast->first_free = dest;

    while (stack.len > 0) {
        ssize_t curidx;
        M2H_RELAY(M2H_vector_idx_top(&stack, &curidx));
        M2H_RELAY(M2H_vector_idx_popback(&stack));
        M2H_ASTNode *const cur = &ast->data[curidx];

        if (cur->prev_sibling != -1) {
            M2H_RELAY(M2H_vector_idx_pushback(&stack, cur->prev_sibling));
        }
        if (cur->child != -1) {
            M2H_RELAY(M2H_vector_idx_pushback(&stack, cur->child));
        }

        M2H_RELAY(M2H_astnode_dtor(&ast->data[curidx]));
        ast->next_free[curidx] = ast->first_free;
        ast->is_allocated[curidx] = false;
        ast->first_free = curidx;
    }

    M2H_RELAY(M2H_vector_idx_dtor(&stack));
    return M2H_RESULT_OK;
}

M2H_Result M2H_astnode_data_text_ctor(M2H_OUT M2H_ASTNodeDataText *self,
                                      M2H_MOVE char *text,
                                      M2H_IN M2H_TextStyle style,
                                      M2H_IN bool newline_tailed) {
    if (text == NULL) {
        return M2H_RESULT_ILLEGAL_ARGUMENT;
    }
    self->style = style;
    self->content = text;
    self->newline_tailed = newline_tailed;
    return M2H_RESULT_OK;
}

M2H_Result M2H_astnode_data_text_dtor(M2H_OUT M2H_ASTNodeDataText *self) {
    free(self->content);
    self->content = NULL;
    return M2H_RESULT_OK;
}

#ifdef DEBUG

#include <stdio.h>

void M2H_print_ast(M2H_IN M2H_AST *ast, M2H_IN ssize_t root) {
    M2H_VectorIdx stack;
    M2H_VectorIdx level;
    M2H_UNWRAP(M2H_vector_idx_ctor(&stack, M2H_DEFAULT_VEC_SIZE));
    M2H_UNWRAP(M2H_vector_idx_ctor(&level, M2H_DEFAULT_VEC_SIZE));
    M2H_UNWRAP(M2H_vector_idx_pushback(&stack, root));
    M2H_UNWRAP(M2H_vector_idx_pushback(&level, 0));
    while (stack.len > 0) {
        ssize_t cur, lvl;
        M2H_UNWRAP(M2H_vector_idx_top(&stack, &cur));
        M2H_UNWRAP(M2H_vector_idx_popback(&stack));
        M2H_UNWRAP(M2H_vector_idx_top(&level, &lvl));
        M2H_UNWRAP(M2H_vector_idx_popback(&level));

        ssize_t getter = ast->data[cur].child;
        while (getter != -1) {
            M2H_UNWRAP(M2H_vector_idx_pushback(&stack, getter));
            M2H_UNWRAP(M2H_vector_idx_pushback(&level, lvl + 1));
            getter = ast->data[getter].prev_sibling;
        }

        for (int i = 1; i <= lvl; ++i) {
            printf("  ");
        }

        switch (ast->data[cur].type) {
        case M2H_ASTNODE_TYPE_ROOT:
            printf("ROOT");
            break;
        case M2H_ASTNODE_TYPE_NONE:
            printf("NONE");
            break;
        case M2H_ASTNODE_TYPE_PARAGRAPH:
            printf("PARAGRAPH");
            break;
        case M2H_ASTNODE_TYPE_HEADING:
            printf("HEADING level=%d", ast->data[cur].heading.level);
            break;
        case M2H_ASTNODE_TYPE_TEXT:
            printf("TEXT text=\"%s\", style=", ast->data[cur].text.content);
            switch (ast->data[cur].text.style) {
            case M2H_TEXTSTYLE_PLAIN:
                printf("plain");
                break;
            case M2H_TEXTSTYLE_BOLD:
                printf("bold");
                break;
            case M2H_TEXTSTYLE_ITALIC:
                printf("italic");
                break;
            case M2H_TEXTSTYLE_BOLDITALIC:
                printf("bold & italic");
                break;
            case M2H_TEXTSTYLE_CODE:
                printf("code");
                break;
            }
        }
        putchar('\n');
    }

    M2H_UNWRAP(M2H_vector_idx_dtor(&stack));
    M2H_UNWRAP(M2H_vector_idx_dtor(&level));
}

#endif // DEBUG