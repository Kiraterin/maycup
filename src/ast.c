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
 * along with this program.  If not, see <https: //www.gnu.org/licenses/>.
 */

#include "ast.h"

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

    *head = 0;
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
    self->first_free = 1;

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
    if (ast->data[dest].next_sibling != -1) {
        ast->data[ast->data[dest].next_sibling].prev_sibling =
            ast->data[dest].prev_sibling;
    }
    if (ast->data[dest].prev_sibling != -1) {
        ast->data[ast->data[dest].prev_sibling].next_sibling =
            ast->data[dest].next_sibling;
    }
    if (ast->data[ast->data[dest].parent].child == dest) {
        ast->data[ast->data[dest].parent].child = ast->data[dest].prev_sibling;
    }

    M2H_VectorIdx stack;
    M2H_RELAY(M2H_vector_idx_ctor(&stack, M2H_DEFAULT_VEC_SIZE));
    if (ast->data[dest].child != -1) {
        M2H_RELAY(M2H_vector_idx_pushback(&stack, ast->data[dest].child));
    }
    M2H_astnode_dtor(&ast->data[dest]);
    ast->next_free[dest] = ast->first_free;
    ast->is_allocated[dest] = false;
    ast->first_free = dest;

    while (stack.len > 0) {
        ssize_t cur;
        M2H_RELAY(M2H_vector_idx_top(&stack, &cur));
        M2H_RELAY(M2H_vector_idx_popback(&stack));
        if (ast->data[cur].prev_sibling != -1) {
            M2H_RELAY(
                M2H_vector_idx_pushback(&stack, ast->data[cur].prev_sibling));
        }
        if (ast->data[cur].child != -1) {
            M2H_RELAY(M2H_vector_idx_pushback(&stack, ast->data[cur].child));
        }
        M2H_astnode_dtor(&ast->data[cur]);
        ast->next_free[cur] = ast->first_free;
        ast->is_allocated[cur] = false;
        ast->first_free = cur;
    }

    M2H_RELAY(M2H_vector_idx_dtor(&stack));
    return M2H_RESULT_OK;
}

M2H_Result M2H_astnode_data_text_ctor(M2H_OUT M2H_ASTNodeDataText *self,
                                      M2H_MOVE char *text,
                                      M2H_IN M2H_TextStyle style) {
    if (text == NULL) {
        return M2H_RESULT_ILLEGAL_ARGUMENT;
    }
    self->style = style;
    self->content = text;
    return M2H_RESULT_OK;
}

M2H_Result M2H_astnode_data_text_dtor(M2H_OUT M2H_ASTNodeDataText *self) {
    free(self->content);
    self->content = NULL;
    return M2H_RESULT_OK;
}